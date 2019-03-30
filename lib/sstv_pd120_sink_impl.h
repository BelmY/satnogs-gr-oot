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


#ifndef INCLUDED_SATNOGS_SSTV_PD120_SINK_IMPL_H
#define INCLUDED_SATNOGS_SSTV_PD120_SINK_IMPL_H

#include <satnogs/sstv_pd120_sink.h>

namespace gr {
  namespace satnogs {

    class sstv_pd120_sink_impl : public sstv_pd120_sink
    {
     private:
      std::string d_filename_png;
      bool d_split;
      bool d_has_sync;

      float *d_line;

      float to_frequency(float sample);
      bool is_sync(size_t pos, const float *samples);

     public:
      sstv_pd120_sink_impl(const char *filename_png, bool split);
      ~sstv_pd120_sink_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace satnogs
} // namespace gr

#endif /* INCLUDED_SATNOGS_SSTV_PD120_SINK_IMPL_H */
