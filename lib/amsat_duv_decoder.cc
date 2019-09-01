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
#include <satnogs/amsat_duv_decoder.h>
#include <satnogs/metadata.h>
#include <gnuradio/blocks/count_bits.h>

extern "C" {
  #include <fec.h>
}

namespace gr
{
namespace satnogs
{

/**
 * Creates a shared pointer to a amsat_duv_decoder object
 * @param control_symbol the control symbol indicating the start of a frame
 * @param max_frame_len the maximum frame length
 * @return a shared pointer to a amsat_duv_decoder object
 */
decoder::decoder_sptr
amsat_duv_decoder::make (const std::string& control_symbol,
                         size_t max_frame_len)
{
  return decoder::decoder_sptr(new amsat_duv_decoder(control_symbol,
                                                     max_frame_len));
}

/**
 *
 * Creates a decoder for the 80b10 DUV coding scheme of the AMSAT FOX satellites
 * @param control_symbol the control symbol indicating the start of a frame
 * @param max_frame_len the maximum frame length
 */
amsat_duv_decoder::amsat_duv_decoder (const std::string& control_symbol,
                                      size_t max_frame_len) :
        decoder (1, max_frame_len),
        d_erasure_cnt (0),
        d_control_symbol_pos (0),
        d_control_symbol_neg (0),
        d_data_reg (0),
        d_wrong_bits (0),
        d_wrong_bits_neg (0),
        d_nwrong (0),
        d_nwrong_neg (0),
        d_word_cnt (0),
        d_state (IN_SYNC)
{
  d_8b_words = new uint8_t[max_frame_len];
  d_erasures_indexes = new int[max_frame_len];

  if (!set_access_code (control_symbol)) {
    throw std::out_of_range ("control_symbol is not 10 bits");
  }
}

bool
amsat_duv_decoder::set_access_code (const std::string &control_symbol)
{
  unsigned len = control_symbol.length ();      // # of bytes in string

  /* if the control sequence is not 10-bit then throw exception */
  if (len != 10) {
    return false;
  }

  for (size_t i = 0; i < len; i++) {
    d_control_symbol_pos = (d_control_symbol_pos << 1)
        | (control_symbol[i] & 0x1);
  }
  d_control_symbol_neg = (~d_control_symbol_pos) & 0x3FF;
  return true;
}

amsat_duv_decoder::~amsat_duv_decoder ()
{
  delete[] d_8b_words;
  delete[] d_erasures_indexes;
}

void
amsat_duv_decoder::process_10b (uint16_t word, int write_pos)
{
  uint16_t diff_bits = 0;
  uint8_t min_pos = 0;
  uint8_t min_dist = 11;
  uint8_t curr_dist = 0;
  size_t i = 0;

  /* Check for the disparity -1 */
  while ((i < 256) && (min_dist > 0)) {

    diff_bits = (word ^ (d_lookup_8b10b[0][i])) & 0x3FF;
    curr_dist = gr::blocks::count_bits16 (diff_bits);

    if (curr_dist < min_dist) {
      min_dist = curr_dist;
      min_pos = i;
    }
    i++;
  }

  /* Check for the disparity +1 */
  i = 0;
  while ((i < 256) && (min_dist > 0)) {

    diff_bits = (word ^ (d_lookup_8b10b[1][i])) & 0x3FF;
    curr_dist = gr::blocks::count_bits16 (diff_bits);

    if (curr_dist < min_dist) {
      min_dist = curr_dist;
      min_pos = i;
    }
    i++;
  }

  /* report that there is erasure to this 10 bits */
  d_8b_words[write_pos] = min_pos;

  /* If we did not found a perfect match, mark this index as erasure */
  if (min_dist != 0) {
    d_erasures_indexes[d_erasure_cnt++] = write_pos;
  }
}

static inline uint16_t
pack_10b_word (const uint8_t *in)
{
  return (((uint16_t) in[0] & 0x1) << 9) | (((uint16_t) in[1] & 0x1) << 8)
      | (((uint16_t) in[2] & 0x1) << 7) | (((uint16_t) in[3] & 0x1) << 6)
      | (((uint16_t) in[4] & 0x1) << 5) | (((uint16_t) in[5] & 0x1) << 4)
      | (((uint16_t) in[6] & 0x1) << 3) | (((uint16_t) in[7] & 0x1) << 2)
      | (((uint16_t) in[8] & 0x1) << 1) | (in[9] & 0x1);
}

decoder_status_t
amsat_duv_decoder::decode (const void *in, int len)
{
  const uint8_t *input = (const uint8_t *) in;
  decoder_status_t status;
  int ret;
  uint16_t word;

  /* Search for the Comma symbol */
  if (d_state == IN_SYNC) {
    for (int i = 0; i < len; i++) {
      d_data_reg = (d_data_reg << 1) | (input[i] & 0x1);
      d_wrong_bits = (d_data_reg ^ d_control_symbol_pos) & 0x3FF;
      d_wrong_bits_neg = (d_data_reg ^ d_control_symbol_neg) & 0x3FF;
      d_nwrong = gr::blocks::count_bits16 (d_wrong_bits);
      d_nwrong_neg = gr::blocks::count_bits16 (d_wrong_bits_neg);

      /* we found the controls symbol */
      if ((d_nwrong == 0) || (d_nwrong_neg == 0)) {
        d_erasure_cnt = 0;
        d_word_cnt = 0;
        d_state = DECODING;
        status.consumed = i + 1;
        return status;
      }
    }
    status.consumed = len;
    return status;
  }

  /*
   * From now one, we have a SYNC so we process the data
   * in chunks of 10 bits
   */
  for (int i = 0; i < len / 10; i++) {
    word = pack_10b_word (&input[i * 10]);

    /* Revert 10b to 8b and accumulate! */
    process_10b (word, d_word_cnt);
    d_word_cnt++;

    if (d_word_cnt == max_frame_len()) {
      d_state = IN_SYNC;

      if (d_erasure_cnt > 0) {
        ret = decode_rs_8(d_8b_words, d_erasures_indexes, d_erasure_cnt,
                          255 - max_frame_len());
      }
      else{
        ret = decode_rs_8(d_8b_words, NULL, 0, 255 - max_frame_len());
      }
      if(ret > -1) {
        status.data = pmt::dict_add(status.data,
                                    pmt::mp(metadata::value(metadata::PDU)),
                                    pmt::make_blob(d_8b_words, 223));
        metadata::add_symbol_erasures(status.data, d_erasure_cnt);
        metadata::add_corrected_bits(status.data, ret);
        status.decode_success = true;
      }
      status.consumed =  (i + 1) * 10;
      return status;
    }
  }
  status.consumed =  (len / 10) * 10;
  return status;
}

void
amsat_duv_decoder::reset ()
{
  d_erasure_cnt = 0;
  d_word_cnt = 0;
  d_state = IN_SYNC;
}


} /* namespace satnogs */
} /* namespace gr */

