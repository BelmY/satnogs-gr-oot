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
#include "frame_decoder_impl.h"

namespace gr
{
namespace satnogs
{

frame_decoder::sptr
frame_decoder::make (decoder::decoder_sptr decoder_object, int input_size)
{
  return gnuradio::get_initial_sptr (new frame_decoder_impl (decoder_object,
                                                             input_size));
}

/*
 * The private constructor
 */
frame_decoder_impl::frame_decoder_impl (decoder::decoder_sptr decoder_object,
                                        int input_size) :
        gr::sync_block ("frame_decoder",
                        gr::io_signature::make (1, 1, input_size),
                        gr::io_signature::make (0, 0, 0)),
        d_decoder (decoder_object)
{
  if (input_size != decoder_object->sizeof_input_item ()) {
    throw std::invalid_argument (
        "Size mismatch between the block input and the decoder");
  }

  message_port_register_in (pmt::mp ("reset"));
  message_port_register_out (pmt::mp ("out"));

  d_frame = new uint8_t[decoder_object->max_frame_len () * 8];

  set_msg_handler (pmt::mp ("reset"),
          boost::bind (&frame_decoder_impl::reset, this, _1));
}

/*
 * Our virtual destructor.
 */
frame_decoder_impl::~frame_decoder_impl ()
{
}

void
frame_decoder_impl::reset (pmt::pmt_t m)
{
  d_decoder->reset();
}

int
frame_decoder_impl::work (int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
{
  const void *in = input_items[0];
  pmt::pmt_t res = pmt::make_dict();

  decoder_status_t status = d_decoder->decode (d_frame, in, noutput_items);
  if (status.decoded_bits > 0) {
    if (status.packed) {
      res = pmt::dict_add(res, pmt::mp("pdu"),
                          pmt::make_blob (d_frame, (size_t)status.decoded_bits / 8));
    }
    else {
      res = pmt::dict_add (
          res, pmt::mp ("pdu"),
          pmt::make_blob (d_frame, (size_t) status.decoded_bits));
    }
    res = pmt::dict_add (res, pmt::mp ("metadata"), status.metadata);
    message_port_pub (pmt::mp ("out"), res);
  }

  // Tell runtime system how many output items we produced.
  return status.consumed;
}

} /* namespace satnogs */
} /* namespace gr */

