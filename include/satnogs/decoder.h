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

#ifndef INCLUDED_SATNOGS_DECODER_H
#define INCLUDED_SATNOGS_DECODER_H

#include <satnogs/api.h>
#include <cstdint>
#include <cstdlib>
#include <pmt/pmt.h>

namespace gr {
namespace satnogs {

/**
 * The status of the decoder
 */
class decoder_status {
public:
  int                   consumed;       /**< The number of input items consumed */
  bool                  decode_success; /**< Indicated if there was a successful decoding */
  pmt::pmt_t            data;           /**< a dictionary with the PDU with of decoded data and the corresponding metadata for the decoded frame */

  decoder_status() :
    consumed(0),
    decode_success(false),
    data(pmt::make_dict())
  {
  }
};

typedef class decoder_status decoder_status_t;


/**
 * \brief Abstract class that provided the API for the c decoders
 *
 * This is an abstract class providing the API for the SatNOGS decoders.
 *
 * The gr-satnogs module tries to provide a unified decoding framework,
 * for various satellites.
 * Specialization is performed by passing to the generic decoding block the
 * appropriate decoder class that implements this abstract class API.
 *
 */
class SATNOGS_API decoder {
public:
  typedef boost::shared_ptr<decoder> decoder_sptr;

  static int base_unique_id;

  int
  unique_id();

  decoder(int input_item_size, size_t max_frame_len = 8192);
  virtual ~decoder();

  /**
   * Decodes a buffer. The difference with the decoder::decode_once() is that
   * this method does not reset the internal state of the decoder.
   * Therefore, it can be called again and continue the decoding when
   * data are available.
   *
   * @param out the output buffer that will hold the decoded data
   *
   * @param in the input items
   *
   * @param len the length of the input buffer in <b>bits</b>
   *
   * @return the number of decoded <b>bits</b>. Due to the fact that
   * some coding schemes may not produce an output that is a multiple of
   * 8 bits, it is necessary to count in a per bit basis. If the result
   * is not a multiple of 8 bits, the LS bits are padded with zeros.
   * If an error occurred an appropriate negative error code is returned
   */
  virtual decoder_status_t
  decode(const void *in, int len) = 0;


  /**
   * Resets the internal state of the decoder to the initial defaults
   *
   */
  virtual void
  reset() = 0;

  size_t
  max_frame_len() const;

  int
  sizeof_input_item() const;

private:
  const int     d_sizeof_in;
  const size_t  d_max_frame_len;
  int           d_id;
};

} // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_DECODER_H */

