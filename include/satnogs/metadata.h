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

#ifndef INCLUDE_SATNOGS_METADATA_H_
#define INCLUDE_SATNOGS_METADATA_H_

#include <string>

namespace gr {

namespace satnogs {


class metadata
{
public:
  typedef enum {
    PDU,
    CRC_VALID,
    FREQ_OFFSET,
    CORRECTED_BITS
  } key_t;

  static std::string
  value(const key_t& k);
};

}  // namespace satnogs

}  // namespace gr


#endif /* INCLUDE_SATNOGS_METADATA_H_ */
