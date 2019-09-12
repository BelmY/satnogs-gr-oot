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
#include <satnogs/decoder.h>

namespace gr {
namespace satnogs {

int decoder::base_unique_id = 1;

/**
 * @note: For internal GNU Radio companion usage. DO NOT EDIT!
 * @return the unique id of the decoder object
 */
int
decoder::unique_id()
{
  return d_id;
}

/**
 * Creates a generic decoder object
 * @param max_frame_len the maximum allowed frame size in bytes
 */
decoder::decoder(int input_item_size, size_t max_frame_len)
  : d_sizeof_in(input_item_size),
    d_max_frame_len(max_frame_len),
    d_id(base_unique_id++)
{
}

decoder::~decoder()
{
}

/**
 * By default, the number of input items can be arbitrary (multiple of 1).
 * Decoders that require the number of input items to be a multiple of a
 * specific number, should override this method.
 *
 * The frame_decoder block, will use the number returned from this method
 * to properly alter the behavior of the GNU Radio scheduler
 *
 * @return the number that input items should be multiple
 */
size_t
decoder::input_multiple() const
{
  return 1;
}

/**
 *
 * @return the maximum allowed frame length
 */
size_t
decoder::max_frame_len() const
{
  return d_max_frame_len;
}

int
decoder::sizeof_input_item() const
{
  return d_sizeof_in;
}

} /* namespace satnogs */
} /* namespace gr */

