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

#include <satnogs/metadata.h>
#include <stdexcept>
namespace gr {

namespace satnogs {

std::string
metadata::value(const key_t& k)
{
  switch(k) {
    case PDU:
      return "PDU";
    case CRC_VALID:
      return "CRC_VALID";
    case FREQ_OFFSET:
      return "FREQ_OFFSET";
    case CORRECTED_BITS:
      return "CORRECTED_BITS";
    default:
      throw std::invalid_argument("metadata: invalid key");
  }
}

}  // namespace satnogs

}  // namespace gr
