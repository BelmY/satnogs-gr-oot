/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2019, Libre Space Foundation <http://libre.space>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <satnogs/log.h>
#include <satnogs/ieee802_15_4_variant_decoder.h>
#include <satnogs/metadata.h>

#include <arpa/inet.h>


namespace gr {
namespace satnogs {

decoder::decoder_sptr
ieee802_15_4_variant_decoder::make(const std::vector<uint8_t> &preamble,
                                   size_t preamble_threshold,
                                   const std::vector<uint8_t> &sync,
                                   size_t sync_threshold,
                                   crc::crc_t crc,
                                   whitening::whitening_sptr descrambler,
                                   bool var_len,
                                   size_t max_len)
{
  return decoder::decoder_sptr(
           new ieee802_15_4_variant_decoder(preamble, preamble_threshold,
               sync, sync_threshold, crc,
               descrambler, var_len, max_len));
}

ieee802_15_4_variant_decoder::ieee802_15_4_variant_decoder(
  const std::vector<uint8_t> &preamble,
  size_t preamble_threshold,
  const std::vector<uint8_t> &sync,
  size_t sync_threshold,
  crc::crc_t crc,
  whitening::whitening_sptr descrambler,
  bool var_len, size_t max_len) :
  decoder(sizeof(uint8_t), max_len),
  d_preamble(preamble.size() * 8),
  d_preamble_shift_reg(preamble.size() * 8),
  d_preamble_len(preamble.size() * 8),
  d_preamble_thrsh(preamble_threshold),
  d_sync(sync.size() * 8),
  d_sync_shift_reg(sync.size() * 8),
  d_sync_len(sync.size() * 8),
  d_sync_thrsh(sync_threshold),
  d_descrambler(descrambler),
  d_crc(crc),
  d_var_len(var_len),
  d_len(max_len + crc::crc_size(crc)),
  d_length_field_len(0),
  d_state(SEARCHING),
  d_cnt(0),
  d_pdu(new uint8_t[max_len])
{
  for (uint8_t b : preamble) {
    d_preamble <<= (b >> 7);
    d_preamble <<= ((b >> 6) & 0x1);
    d_preamble <<= ((b >> 5) & 0x1);
    d_preamble <<= ((b >> 4) & 0x1);
    d_preamble <<= ((b >> 3) & 0x1);
    d_preamble <<= ((b >> 2) & 0x1);
    d_preamble <<= ((b >> 1) & 0x1);
    d_preamble <<= (b & 0x1);
  }
  for (uint8_t b : sync) {
    d_sync <<= (b >> 7);
    d_sync <<= ((b >> 6) & 0x1);
    d_sync <<= ((b >> 5) & 0x1);
    d_sync <<= ((b >> 4) & 0x1);
    d_sync <<= ((b >> 3) & 0x1);
    d_sync <<= ((b >> 2) & 0x1);
    d_sync <<= ((b >> 1) & 0x1);
    d_sync <<= (b & 0x1);
  }

  /* Parameters checking */
  if (max_len == 0) {
    throw std::invalid_argument(
      "The maximum frame size should be at least 1 byte");
  }

  if (d_sync_len < 8) {
    throw std::invalid_argument("SYNC word should be at least 8 bits");
  }

  if (d_preamble_len < 8) {
    throw std::invalid_argument("Preamble should be at least 8 bits");
  }

  if (d_preamble_len < 2 * d_preamble_thrsh) {
    throw std::invalid_argument(
      "Too many error bits are allowed for the preamble."
      "Consider lowering the threshold");
  }

  if (d_sync_len < 2 * d_sync_thrsh) {
    throw std::invalid_argument(
      "Too many error bits are allowed for the sync word. "
      "Consider lowering the threshold");
  }

  if (var_len) {
    d_length_field_len = 1;
  }
}

ieee802_15_4_variant_decoder::~ieee802_15_4_variant_decoder()
{
  delete [] d_pdu;
}

decoder_status_t
ieee802_15_4_variant_decoder::decode(const void *in, int len)
{
  if (d_var_len) {
    return decode_var_len(in, len);
  }
  return decode_const_len(in, len);
}

void
ieee802_15_4_variant_decoder::reset()
{
  if (d_descrambler) {
    d_descrambler->reset();
  }
  d_cnt = 0;
  d_state = SEARCHING;
  d_preamble_shift_reg.reset();
  d_sync_shift_reg.reset();
}

decoder_status_t
ieee802_15_4_variant_decoder::decode_var_len(const void *in, int len)
{
  decoder_status_t status;
  switch (d_state) {
  case SEARCHING:
    status.consumed = search_preamble((const uint8_t *) in, len);
    break;
  case SEARCHING_SYNC:
    status.consumed = search_sync((const uint8_t *) in, len);
    break;
  case DECODING_GENERIC_FRAME_LEN:
    status.consumed = decode_frame_len((const uint8_t *) in);
    break;
  case DECODING_PAYLOAD:
    decode_payload(status, (const uint8_t *) in, len);
    break;
  default:
    throw std::runtime_error("ieee802_15_4_variant_decoder: Invalid state");
  }
  return status;
}

decoder_status_t
ieee802_15_4_variant_decoder::decode_const_len(const void *in, int len)
{
  decoder_status_t status;
  switch (d_state) {
  case SEARCHING:
    status.consumed = search_preamble((const uint8_t *) in, len);
    break;
  case SEARCHING_SYNC:
    status.consumed = search_preamble((const uint8_t *) in, len);
    break;
  case DECODING_PAYLOAD:
    decode_payload(status, (const uint8_t *) in, len);
    break;
  default:
    throw std::runtime_error("ieee802_15_4_variant_decoder: Invalid state");
  }
  return status;
}

/**
 * To greatly simplify the logic, the decoder requests that the number of
 * input items should be a multiple of 8
 * @return 8
 */
size_t
ieee802_15_4_variant_decoder::input_multiple() const
{
  return 8;
}

int
ieee802_15_4_variant_decoder::search_preamble(const uint8_t *in, int len)
{
  for (int i = 0; i < len; i++) {
    d_preamble_shift_reg <<= in[i];
    shift_reg tmp = d_preamble_shift_reg ^ d_preamble;
    if (tmp.count() <= d_preamble_thrsh) {
      d_state = SEARCHING_SYNC;
      d_cnt = 0;
      return i + 1;
    }
  }
  return len;
}

int
ieee802_15_4_variant_decoder::search_sync(const uint8_t *in, int len)
{
  for (int i = 0; i < len; i++) {
    d_sync_shift_reg <<= in[i];
    shift_reg tmp = d_sync_shift_reg ^ d_sync;
    d_cnt++;
    if (tmp.count() <= d_sync_thrsh) {
      if (d_var_len) {
        d_state = DECODING_GENERIC_FRAME_LEN;
      }
      else {
        d_state = DECODING_PAYLOAD;
      }
      d_cnt = 0;
      return i + 1;
    }

    /* The sync word should be available by now */
    if (d_cnt > d_preamble_len * 2 + d_sync_len) {
      reset();
      return i + 1;
    }
  }
  return len;
}

int
ieee802_15_4_variant_decoder::decode_frame_len(const uint8_t *in)
{
  uint8_t b = 0x0;
  b |= in[0] << 7;
  b |= in[1] << 6;
  b |= in[2] << 5;
  b |= in[3] << 4;
  b |= in[4] << 3;
  b |= in[5] << 2;
  b |= in[6] << 1;
  b |= in[7];

  if (d_descrambler) {
    uint8_t descrambled = 0x0;
    d_descrambler->descramble((uint8_t *)&descrambled,
                              (const uint8_t *)&b, 1, false);
    d_len = descrambled;
    d_pdu[0] = descrambled;
  }
  else {
    d_len = b;
    d_pdu[0] = b;
  }
  d_len += crc::crc_size(d_crc);
  /* Length field is needed for the CRC calculation */
  d_cnt = 1;
  d_state = DECODING_PAYLOAD;
  return 8;
}

void
ieee802_15_4_variant_decoder::decode_payload(decoder_status_t &status,
    const uint8_t *in, int len)
{
  const int s = len / 8;
  for (int i = 0; i < s; i++) {
    uint8_t b = 0x0;
    b = in[i * 8] << 7;
    b |= in[i * 8 + 1] << 6;
    b |= in[i * 8 + 2] << 5;
    b |= in[i * 8 + 3] << 4;
    b |= in[i * 8 + 4] << 3;
    b |= in[i * 8 + 5] << 2;
    b |= in[i * 8 + 6] << 1;
    b |= in[i * 8 + 7];
    d_pdu[d_cnt++] = b;
    if (d_cnt == d_len + d_length_field_len) {
      if (d_descrambler) {
        d_descrambler->descramble(d_pdu + d_length_field_len,
                                  d_pdu + d_length_field_len, d_len, false);
      }

      status.decode_success = true;
      status.consumed = (i + 1) * 8;
      if (check_crc()) {
        metadata::add_time_iso8601(status.data);
        metadata::add_pdu(status.data, d_pdu + d_length_field_len,
                          d_len - crc::crc_size(d_crc));
        metadata::add_crc_valid(status.data, true);
      }
      else {
        metadata::add_pdu(status.data, d_pdu + d_length_field_len, d_len);
        metadata::add_crc_valid(status.data, false);
      }
      reset();
      return;
    }
  }
  status.consumed = s * 8;
}

bool
ieee802_15_4_variant_decoder::check_crc()
{
  uint16_t crc16_c;
  uint16_t crc16_received;
  switch (d_crc) {
  case crc::CRC_NONE:
    return true;
  case crc::CRC16_CCITT:
    crc16_c = crc::crc16_ccitt(d_pdu, d_len + d_length_field_len - 2);
    memcpy(&crc16_received, d_pdu + d_length_field_len + d_len - 2, 2);
    crc16_received = ntohs(crc16_received);
    LOG_DEBUG("Received: 0x%02x Computed: 0x%02x", crc16_received, crc16_c);
    if (crc16_c == crc16_received) {
      return true;
    }
    return false;
  case crc::CRC16_CCITT_REVERSED:
    crc16_c = crc::crc16_ccitt_reversed(d_pdu, d_len + d_length_field_len - 2);
    memcpy(&crc16_received, d_pdu + d_length_field_len + d_len - 2, 2);
    crc16_received = ntohs(crc16_received);
    LOG_DEBUG("Received: 0x%02x Computed: 0x%02x", crc16_received, crc16_c);
    if (crc16_c == crc16_received) {
      return true;
    }
    return false;
  case crc::CRC16_IBM:
    crc16_c = crc::crc16_ibm(d_pdu, d_len + d_length_field_len - 2);
    memcpy(&crc16_received, d_pdu + d_length_field_len + d_len - 2, 2);
    crc16_received = ntohs(crc16_received);
    LOG_WARN("Received: 0x%02x Computed: 0x%02x", crc16_received, crc16_c);
    if (crc16_c == crc16_received) {
      return true;
    }
    return false;
  default:
    throw std::runtime_error("ieee802_15_4_variant_decoder: Invalid CRC");
  }
}

} /* namespace satnogs */
} /* namespace gr */

