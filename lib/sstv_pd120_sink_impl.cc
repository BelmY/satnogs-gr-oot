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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "sstv_pd120_sink_impl.h"

namespace gr {
  namespace satnogs {

    const size_t image_width = 640;
    const size_t sync_length = 105;
    const size_t sync_thresh = 100;
    const size_t porch_length = 10;
    const size_t line_length = sync_length + porch_length + 4 * image_width;

    const float max_dev = 600;
    const float center = 1750;
    const float min_freq = 1200;
    const float max_freq = 2300;

    const float color_low = 1500;
    const float color_high = max_freq;


    sstv_pd120_sink::sptr
    sstv_pd120_sink::make(const char *filename_png, bool split)
    {
      return gnuradio::get_initial_sptr
        (new sstv_pd120_sink_impl(filename_png, split));
    }

    /*
     * The private constructor
     */
    sstv_pd120_sink_impl::sstv_pd120_sink_impl(const char *filename_png, bool split)
      : gr::sync_block("sstv_pd120_sink",
              gr::io_signature::make (1, 1, sizeof(float)),
              gr::io_signature::make (0, 0, 0)),
        d_filename_png (filename_png),
        d_split (split),
        d_has_sync(false)
    {
        set_history(sync_length);
        d_line = new float[line_length];
    }

    /*
     * Our virtual destructor.
     */
    sstv_pd120_sink_impl::~sstv_pd120_sink_impl()
    {
        delete[] d_line;
    }

    float
    sstv_pd120_sink_impl::to_frequency(float sample) {
        float freq = center + sample * max_dev;
        freq = std::max(min_freq, freq);
        freq = std::min(max_freq, freq);
        return freq;
    }

    bool
    sstv_pd120_sink_impl::is_sync(size_t pos, const float *samples) {
        size_t count = 0;
        for(size_t i = 0; i < sync_length; i++) {
            float sample = to_frequency(samples[pos - (sync_length - 1) + i]);
            if(sample < color_low) {
                count += 1;
            }
        }

        bool res = count > sync_length && !d_has_sync;
        d_has_sync = count > sync_length;

        std::cout << "Count: " << count << " " << d_has_sync << std::endl;

        return res;
    }

    int
    sstv_pd120_sink_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];

        std::cout << "foo" << std::endl;
        for (size_t i = sync_length - 1;
            i < noutput_items + sync_length - 1; i++) {

            if(is_sync(i, in)) {
                std::cout << "Sync: " << i << std::endl;
            }
        }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace satnogs */
} /* namespace gr */
