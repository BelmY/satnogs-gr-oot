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
#include <satnogs/ax25_decoder.h>
#include <satnogs/ax25.h>
#include <satnogs/metadata.h>

namespace gr {
namespace satnogs {

decoder::decoder_sptr
ax25_decoder::make(const std::string &addr, uint8_t ssid, bool promisc,
                   bool descramble, bool crc_check, size_t max_frame_len)
{
  return decoder::decoder_sptr(
           new ax25_decoder(addr, ssid, promisc, descramble, crc_check,
                            max_frame_len));
}

ax25_decoder::ax25_decoder(const std::string &addr, uint8_t ssid, bool promisc,
                           bool descramble, bool crc_check, size_t max_frame_len) :
  decoder(sizeof(uint8_t), 2 * max_frame_len * 8),
  d_promisc(promisc),
  d_descramble(descramble),
  d_crc_check(crc_check),
  d_max_frame_len(max_frame_len),
  d_state(NO_SYNC),
  d_shift_reg(0x0),
  d_dec_b(0x0),
  d_prev_bit_nrzi(0),
  d_received_bytes(0),
  d_decoded_bits(0),
  d_lfsr(0x21, 0x0, 16),
  d_frame_buffer(
    new uint8_t[max_frame_len + AX25_MAX_ADDR_LEN + AX25_MAX_CTRL_LEN
                              + sizeof(uint16_t)]),
  d_start_idx(0),
  d_frame_start(0),
  d_sample_cnt(0)
{
}

decoder_status_t
ax25_decoder::decode(const void *in, int len)
{
  const uint8_t *input = (const uint8_t *) in;
  decoder_status_t status;
  if (d_descramble) {
    for (int i = 0; i < len; i++) {
      /* Perform NRZI decoding */
      uint8_t b = (~((input[i] - d_prev_bit_nrzi) % 2)) & 0x1;
      d_prev_bit_nrzi = input[i];
      b = d_lfsr.next_bit_descramble(b);
      d_bitstream.push_back(b);
    }
  }
  else {
    for (int i = 0; i < len; i++) {
      /* Perform NRZI decoding */
      uint8_t b = (~((input[i] - d_prev_bit_nrzi) % 2)) & 0x1;
      d_prev_bit_nrzi = input[i];
      d_bitstream.push_back(b);
    }
  }
  /*
   * The decoder always consumes all the available input bits and stores them
   * internally
   */
  _decode(status);
  status.consumed = len;
  return status;
}

void
ax25_decoder::reset()
{
  reset_state();
}

bool
ax25_decoder::_decode(decoder_status_t &status)
{
  while (1) {
    bool cont = false;
    switch (d_state) {
    case NO_SYNC:
      for (size_t i = 0; i < d_bitstream.size(); i++) {
        decode_1b(d_bitstream[i]);
        if (d_shift_reg == AX25_SYNC_FLAG) {
          d_bitstream.erase(d_bitstream.begin(),
                            d_bitstream.begin() + i + 1);
          /* Increment the number of items read so far */
          incr_nitems_read(i);
          enter_sync_state();
          /* Mark possible start of the frame */
          d_frame_start = nitems_read();
          d_start_idx = 0;
          cont = true;
          break;
        }
      }
      if (cont) {
        continue;
      }
      incr_nitems_read(d_bitstream.size());
      d_bitstream.clear();
      return false;
    case IN_SYNC:
      /*
       * Most of the transmitters repeat several times the AX.25 SYNC
       * In case of G3RUH this is mandatory to allow the self synchronizing
       * scrambler to settle
       */
      for (size_t i = d_start_idx; i < d_bitstream.size(); i++) {
        decode_1b(d_bitstream[i]);
        d_decoded_bits++;
        if (d_decoded_bits == 8) {
          /* Perhaps we are in frame! */
          if (d_shift_reg != AX25_SYNC_FLAG) {
            d_start_idx = i + 1;
            enter_decoding_state();
            cont = true;
            break;
          }
          d_decoded_bits = 0;
        }
      }
      if (cont) {
        continue;
      }
      d_start_idx = d_bitstream.size();
      return false;
    case DECODING:
      for (size_t i = d_start_idx; i < d_bitstream.size(); i++) {
        decode_1b(d_bitstream[i]);
        if (d_shift_reg == AX25_SYNC_FLAG) {
          d_sample_cnt = nitems_read() + i - d_frame_start;
          LOG_DEBUG("Found frame end");
          if (enter_frame_end(status)) {
            d_bitstream.erase(d_bitstream.begin(),
                              d_bitstream.begin() + i + 1);
            /* Increment the number of items read so far */
            incr_nitems_read(i);
            d_start_idx = d_bitstream.size();
            return true;
          }
          cont = true;
          break;
        }
        else if ((d_shift_reg & 0xfc) == 0x7c) {
          /*This was a stuffed bit */
          d_dec_b <<= 1;
        }
        else if ((d_shift_reg & 0xfe) == 0xfe) {
          LOG_DEBUG("Invalid shift register value %u", d_received_bytes);
          reset_state();
          cont = true;
          break;
        }
        else {
          d_decoded_bits++;
          if (d_decoded_bits == 8) {
            d_frame_buffer[d_received_bytes++] = d_dec_b;
            d_decoded_bits = 0;

            /*Check if the frame limit was reached */
            if (d_received_bytes >= d_max_frame_len) {
              LOG_DEBUG("Wrong size");
              reset_state();
              cont = true;
              break;
            }
          }
        }
      }
      if (cont) {
        continue;
      }
      d_start_idx = d_bitstream.size();
      return false;
    default:
      LOG_ERROR("Invalid decoding state");
      reset_state();
      return false;
    }
  }
}


ax25_decoder::~ax25_decoder()
{
  delete[] d_frame_buffer;
}

void
ax25_decoder::reset_state()
{
  d_state = NO_SYNC;
  d_dec_b = 0x0;
  d_shift_reg = 0x0;
  d_decoded_bits = 0;
  d_received_bytes = 0;
}

void
ax25_decoder::enter_sync_state()
{
  d_state = IN_SYNC;
  d_dec_b = 0x0;
  d_shift_reg = 0x0;
  d_decoded_bits = 0;
  d_received_bytes = 0;
}

void
ax25_decoder::enter_decoding_state()
{
  uint8_t tmp;
  d_state = DECODING;
  d_decoded_bits = 0;
  d_received_bytes = 0;

  /*
   * Due to the possibility of bit stuffing on the first byte some special
   * handling is necessary
   */
  if ((d_shift_reg & 0xfc) == 0x7c) {
    /*This was a stuffed bit */
    d_dec_b <<= 1;
    d_decoded_bits = 7;
  }
  else {
    d_frame_buffer[0] = d_dec_b;
    d_decoded_bits = 0;
    d_received_bytes = 1;
  }
}

bool
ax25_decoder::enter_frame_end(decoder_status_t &status)
{
  uint16_t fcs;
  uint16_t recv_fcs = 0x0;

  /* First check if the size of the frame is valid */
  if (d_received_bytes < AX25_MIN_ADDR_LEN + sizeof(uint16_t)) {
    reset_state();
    return false;
  }

  /*
   * Check if the frame is correct using the FCS field
   * Using this field also try to correct up to 2 error bits
   */
  if (frame_check()) {
    metadata::add_pdu(status.data, d_frame_buffer, d_received_bytes - sizeof(uint16_t));
    metadata::add_time_iso8601(status.data);
    metadata::add_crc_valid(status.data, true);
    metadata::add_sample_start(status.data, d_frame_start);
    metadata::add_sample_cnt(status.data, d_sample_cnt);
    status.decode_success = true;
    reset_state();
    return true;
  }
  else if (!d_crc_check) {
    metadata::add_pdu(status.data, d_frame_buffer, d_received_bytes - sizeof(uint16_t));
    metadata::add_time_iso8601(status.data);
    metadata::add_crc_valid(status.data, false);
    status.decode_success = true;
    LOG_DEBUG("Wrong crc");
    reset_state();
    return false;
  }
  return false;
}


inline void
ax25_decoder::decode_1b(uint8_t in)
{

  /* In AX.25 the LS bit is sent first */
  d_shift_reg = (d_shift_reg >> 1) | (in << 7);
  d_dec_b = (d_dec_b >> 1) | (in << 7);
}

bool
ax25_decoder::frame_check()
{
  uint16_t fcs;
  uint16_t recv_fcs = 0x0;
  uint8_t orig_byte;

  /* Check if the frame is correct using the FCS field */
  fcs = ax25_fcs(d_frame_buffer, d_received_bytes - sizeof(uint16_t));
  recv_fcs = (((uint16_t) d_frame_buffer[d_received_bytes - 1]) << 8)
             | d_frame_buffer[d_received_bytes - 2];
  if (fcs == recv_fcs) {
    return true;
  }
  return false;
}

} /* namespace satnogs */
} /* namespace gr */

