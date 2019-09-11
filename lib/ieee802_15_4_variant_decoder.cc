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
#include <satnogs/ieee802_15_4_variant_decoder.h>


namespace gr
{
namespace satnogs
{

ieee802_15_4_variant_decoder::ieee802_15_4_variant_decoder (
    const std::vector<uint8_t> &preamble, size_t preamble_threshold,
    const std::vector<uint8_t> &sync, crc::crc_t crc,
    whitening::whitening_sptr descrambler)
{
}

ieee802_15_4_variant_decoder::~ieee802_15_4_variant_decoder ()
{
}

} /* namespace satnogs */
} /* namespace gr */

