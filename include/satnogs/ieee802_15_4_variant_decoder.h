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

#ifndef INCLUDED_SATNOGS_IEEE802_15_4_VARIANT_DECODER_H
#define INCLUDED_SATNOGS_IEEE802_15_4_VARIANT_DECODER_H

#include <satnogs/api.h>
#include <satnogs/whitening.h>
#include <satnogs/crc.h>

namespace gr
{
namespace satnogs
{

/*!
 * \brief A IEEE 802.15.4 like decoder
 *
 * The IEEE 802.15.4 uses the well known preamble + sync word synchronization
 * scheme. Many popular on Cubesats ICs like the Texas Instruments CC1xxx family
 * or the AXxxxx of On Semiconductors follow this scheme. This decoder
 * class provides a generic way to decode signals following this framing
 * scheme.
 *
 */
class SATNOGS_API ieee802_15_4_variant_decoder
{
public:
  ieee802_15_4_variant_decoder (const std::vector<uint8_t> &preamble,
                                size_t preamble_threshold,
                                const std::vector<uint8_t> &sync,
                                crc::crc_t crc,
                                whitening::whitening_sptr descrambler);
  ~ieee802_15_4_variant_decoder ();
private:
};

} // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_IEEE802_15_4_VARIANT_DECODER_H */

