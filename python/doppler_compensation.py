#! /usr/bin/python3
#
# gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
#
#  Copyright (C) 2019
#  Libre Space Foundation <http://libre.space>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#

from gnuradio import gr
from gnuradio import filter
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from . import satnogs_swig as satnogs
import weakref


class doppler_compensation(gr.hier_block2):
    """ Doppler compensation and resampling block

    This block performs Doppler compensation and resampling.
    Rather than using directly the available Doppler correction blocks,
    based on the user parameters of the incoming sampling rate and the 
    desired target sampling rate, it applies proper decimation on the signal
    so the frequency shift to be applied to a smaller sampling rate,
    reducing significantly the CPU resources required. At the previous 
    architecture (gr-satnogs 1.x.x) we used seperate blocks for the doppler
    correction and the LO digital shift, operating at the device sampling rate.
    These two blocks, performing almost the same operation, contributed to a
    30% CPU usage of the overall application. Now the LO is compensated by
    the Doppler correction block, taking into account at the same time the
    Doppler drift.
    
    After the digital LO shift, the Doppler corrected signal is passed through
    an Polyphase Arbitrary Resampler, to match exactly the sampling rate
    requested by the user.
    
    Parameters
    ----------
    samp_rate : double
        The sampling rate of the input signal
    sat_freq : double
        The target frequency of the satellite. This highly depends on the 
        Doppler frequency messages. Some blocks (e.g rigctld) may produce 
        the target observed RF frequency, whereas others (e.g gr-leo) are 
        sending messages containing only the frequency drift. In the first 
        case, this field should contain the actual frequency of the satellite.
        On the other hand, for the second case this field should be 0.
    lo_offset : double
        The LO offset from the actuall observation frequency. In most cases,
        we use a LO offset to avoid the DC spikes. This offset should be positive
        if the hardware RF frequency is less than the target frequency, negative
        otherwise.
    compensate : bool
        This parameter instructs the Doppler correction block to apply doppler
        compensation. The LO offset compensation is still applied regardless of
        the value of this field.
    """

    def __init__(self, samp_rate, sat_freq, lo_offset, out_samp_rate,
                 compensate=True):
        gr.hier_block2.__init__(self,
                                "doppler_compensation",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(1, 1, gr.sizeof_gr_complex))

        self.message_port_register_hier_in('doppler')

        # Decimate the incoming signal using the rational resampler first.
        # Then perform the doppler correctio and a fractional resampler
        # to a lower rate to save some CPU cycles
        if(out_samp_rate > samp_rate):
            gr.log.info("satnogs.doppler_compensation: Output sampling rate sould be "
                        "less or equal the device sampling rate")
            raise AttributeError
        
        if(lo_offset > samp_rate // 4):
            gr.log.info("satnogs.doppler_compensation: The LO offset frequency "
                        "should be > samp_rate / 4")
            raise AttributeError

        self.decimation = 1
        min_s = max(abs(2 * lo_offset) + 40e3, out_samp_rate + 2 * abs(lo_offset))
        while(samp_rate / (self.decimation + 1) > min_s):
            self.decimation = self.decimation + 1

        print(self.decimation)
        if(self.decimation > 1):
            self.dec = filter.rational_resampler_ccc(interpolation=1,
                                                     decimation=self.decimation,
                                                     taps=None,
                                                     fractional_bw=None)

        # Even with no doppler compensation enabled we need this
        # block to correct the LO offset
        self.doppler = satnogs.coarse_doppler_correction_cc(sat_freq,
                                                            lo_offset,
                                                            samp_rate / self.decimation)

        self.pfb_rs = pfb.arb_resampler_ccf(
            out_samp_rate / (samp_rate / self.decimation),
            taps=None,
            flt_size=32)
        self.pfb_rs.declare_sample_delay(0)

        if(self.decimation > 1):
            self.connect((self, 0), (self.dec, 0),
                         (self.doppler, 0), (self.pfb_rs, 0), (self, 0))
        else:
            self.connect((self, 0), (self.doppler, 0),
                         (self.pfb_rs, 0),  (self, 0))

        if(compensate):
            self.msg_connect(weakref.proxy(self), "doppler",
                             self.doppler, "freq")