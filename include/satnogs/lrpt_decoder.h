/* -*- c++ -*- */
/*
 * gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, Libre Space Foundation <http://librespacefoundation.org/>
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

#ifndef INCLUDED_SATNOGS_LRPT_DECODER_H
#define INCLUDED_SATNOGS_LRPT_DECODER_H

#include <satnogs/api.h>
#include <gnuradio/block.h>

namespace gr
{
namespace satnogs
{

/*!
 * \brief <+description of block+>
 * \ingroup satnogs
 *
 */
class SATNOGS_API lrpt_decoder : virtual public gr::block
{
public:
  typedef boost::shared_ptr<lrpt_decoder> sptr;

  /*!
   * \brief Return a shared_ptr to a new instance of satnogs::lrpt_decoder.
   *
   * To avoid accidental use of raw pointers, satnogs::lrpt_decoder's
   * constructor is in a private implementation
   * class. satnogs::lrpt_decoder::make is the public interface for
   * creating new instances.
   */
  static sptr
  make ();
};

} // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_LRPT_DECODER_H */

