#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: AMSAT FOX DUV Decoder
# Author: Thanos Giolias (agiolias@csd.uoc.gr), Nikos Karamolegos (karamolegkos.n@gmail.com), Manolis Surligas (surligas@gmail.com)
# Description: A DUV Decoder for the AMSAT FOX satellites
# GNU Radio version: 3.8.0.0

from gnuradio import analog
import math
from gnuradio import digital
from gnuradio import filter
from gnuradio.filter import firdes
from gnuradio import gr
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import satnogs
import soapy

class satnogs_amsat_fox_duv_decoder(gr.top_block):

    def __init__(self, antenna='', bw=0.0, decoded_data_file_path='/tmp/.satnogs/data/data', dev_args='', doppler_correction_per_sec=20, enable_iq_dump=0, file_path='test.wav', gain=0.0, iq_file_path='/tmp/iq.dat', lo_offset=100e3, rigctl_port=4532, rx_freq=100e6, samp_rate_rx=0.0, soapy_rx_device='driver=invalid', udp_IP='127.0.0.1', udp_port=16887, waterfall_file_path='/tmp/waterfall.dat'):
        gr.top_block.__init__(self, "AMSAT FOX DUV Decoder")

        ##################################################
        # Parameters
        ##################################################
        self.antenna = antenna
        self.bw = bw
        self.decoded_data_file_path = decoded_data_file_path
        self.dev_args = dev_args
        self.doppler_correction_per_sec = doppler_correction_per_sec
        self.enable_iq_dump = enable_iq_dump
        self.file_path = file_path
        self.gain = gain
        self.iq_file_path = iq_file_path
        self.lo_offset = lo_offset
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.samp_rate_rx = samp_rate_rx
        self.soapy_rx_device = soapy_rx_device
        self.udp_IP = udp_IP
        self.udp_port = udp_port
        self.waterfall_file_path = waterfall_file_path

        ##################################################
        # Variables
        ##################################################
        self.variable_amsat_duv_decoder_0 = variable_amsat_duv_decoder_0 = satnogs.amsat_duv_decoder_make('0011111010', 96)
        self.max_modulation_freq = max_modulation_freq = 3000
        self.deviation = deviation = 5000
        self.audio_samp_rate = audio_samp_rate = 48000

        ##################################################
        # Blocks
        ##################################################
        self.soapy_source_0 = None
        if "custom" == 'custom':
            dev = soapy_rx_device
        else:
            dev = 'driver=' + "custom"
        if "custom" == 'sdrplay':
            f = 'if_mode=' + "Zero-IF" + ',' + 'agc_setpoint=' + str("-30") + ',' + 'biasT_ctrl=' + "True".lower() + ',' + 'rfnotch_ctrl=' + "False".lower() + ',' + 'dabnotch_ctrl=' + "False".lower() + ',' + str(dev_args)
            f = f.replace('\"', '')
            f = f.replace("\\'", '')
            f = f.strip(',')
            self.soapy_source_0 = soapy.source(1, dev, "custom", f, samp_rate_rx, "fc32")
        else:
            self.soapy_source_0 = soapy.source(1, dev, "custom", dev_args, samp_rate_rx, "fc32")

        if 0 != 0:
            self.soapy_source_0.set_master_clock_rate(0)

        if len('') > 0:
            self.soapy_source_0.set_clock_source('')

        # Set up dc offsets
        if "custom" != 'uhd':
            if (self.soapy_source_0.hasDCOffset(0)):
                self.soapy_source_0.set_dc_offset(0,0,False == 'True')

            if (self.soapy_source_0.hasDCOffset(1)):
                self.soapy_source_0.set_dc_offset(1,0,False == 'True')

        # Setup IQ Balance
        if "custom" != 'uhd':
            if (self.soapy_source_0.hasIQBalance(0)):
                self.soapy_source_0.set_iq_balance(0,0)

            if (self.soapy_source_0.hasIQBalance(1)):
                self.soapy_source_0.set_iq_balance(1,0)

        # Setup Frequency correction
        if (self.soapy_source_0.hasFrequencyCorrection(0)):
            self.soapy_source_0.set_frequency_correction(0,0)

        if (self.soapy_source_0.hasFrequencyCorrection(1)):
            self.soapy_source_0.set_frequency_correction(1,0)

        self.soapy_source_0.set_gain_mode(0,False)
        self.soapy_source_0.set_gain_mode(1,False)

        self.soapy_source_0.set_frequency(0, rx_freq - lo_offset)
        self.soapy_source_0.set_frequency(1, 0)

        # Made antenna sanity check more generic
        antList = self.soapy_source_0.listAntennas(0)

        if len(antList) > 1:
            # If we have more than 1 possible antenna
            if len(antenna) == 0 or antenna not in antList:
                print("ERROR: Please define ant0 to an allowed antenna name.")
                strAntList = str(antList).lstrip('(').rstrip(')').rstrip(',')
                print("Allowed antennas: " + strAntList)
                exit(0)

            self.soapy_source_0.set_antenna(0,antenna)

        if 1 > 1:
            antList = self.soapy_source_0.listAntennas(1)
            # If we have more than 1 possible antenna
            if len(antList) > 1:
                if len('RX2') == 0 or 'RX2' not in antList:
                    print("ERROR: Please define ant1 to an allowed antenna name.")
                    strAntList = str(antList).lstrip('(').rstrip(')').rstrip(',')
                    print("Allowed antennas: " + strAntList)
                    exit(0)

                self.soapy_source_0.set_antenna(1,'RX2')

        self.soapy_source_0.set_overall_gain(0,gain, False  )
        self.soapy_source_0.set_overall_gain(1,10, True  )

        # Prevent some weird double-gain setting issues for systems with an overall_gain setting
        # noticed weird behavior with uhd
        if "custom" == 'uhd' or "custom" == 'sidekiq' or "custom" == 'bladerf' or "custom" == 'lime':
            self.soapy_source_0.set_gain(0,"PGA", 24, False  )
            self.soapy_source_0.set_gain(1,"PGA", 24, True  )
        else:
            if "custom" == 'custom':
                # If we're here and we're custom, let's still call overall gain
                self.soapy_source_0.set_overall_gain(0,gain, False  )
                self.soapy_source_0.set_overall_gain(1,10, True  )

            self.soapy_source_0.set_gain(0,"LNA", 10, False  )
            self.soapy_source_0.set_gain(1,"LNA", 10, True  )
            self.soapy_source_0.set_gain(0,"TIA", 0, False  )
            self.soapy_source_0.set_gain(1,"TIA", 0, True  )
            self.soapy_source_0.set_gain(0,"MIX", 10, False  )
            self.soapy_source_0.set_gain(1,"MIX", 10, True  )
            self.soapy_source_0.set_gain(0,"VGA", 10, False  )
            self.soapy_source_0.set_gain(1,"VGA", 10, True  )
            # Only rtl-sdr uses TUNER, so just ch0
            self.soapy_source_0.set_gain(0,"TUNER", 10, False  )
            # Only hackrf uses "AMP", so just ch0
            self.soapy_source_0.set_gain(0,"AMP", 0, False  )
            # Only sdrplay uses IFGR so just ch0 for each
            self.soapy_source_0.set_gain(0,"IFGR", 59, False  )
            self.soapy_source_0.set_gain(0,"RFGR", 9, False  )
        self.satnogs_waterfall_sink_0 = satnogs.waterfall_sink(audio_samp_rate, rx_freq, 10, 1024, waterfall_file_path, 1)
        self.satnogs_udp_msg_sink_0_0 = satnogs.udp_msg_sink(udp_IP, udp_port, 1500)
        self.satnogs_tcp_rigctl_msg_source_0 = satnogs.tcp_rigctl_msg_source("127.0.0.1", rigctl_port, False, int(1000.0/doppler_correction_per_sec) + 1, 1500)
        self.satnogs_ogg_encoder_0 = satnogs.ogg_encoder(file_path, audio_samp_rate, 1.0)
        self.satnogs_json_converter_0 = satnogs.json_converter()
        self.satnogs_iq_sink_0 = satnogs.iq_sink(16768, iq_file_path, False, 1)
        self.satnogs_frame_file_sink_0_1_0 = satnogs.frame_file_sink(decoded_data_file_path, 0)
        self.satnogs_frame_decoder_0 = satnogs.frame_decoder(variable_amsat_duv_decoder_0, 1 * 1)
        self.satnogs_doppler_compensation_0 = satnogs.doppler_compensation(samp_rate_rx, rx_freq, lo_offset, audio_samp_rate, 1)
        self.root_raised_cosine_filter_0 = filter.fir_filter_fff(
            1,
            firdes.root_raised_cosine(
                1,
                1,
                2.4,
                0.5,
                512))
        self.low_pass_filter_1 = filter.fir_filter_fff(
            100,
            firdes.low_pass(
                1,
                audio_samp_rate,
                195,
                10,
                firdes.WIN_HAMMING,
                6.76))
        self.low_pass_filter_0 = filter.fir_filter_ccf(
            1,
            firdes.low_pass(
                1,
                audio_samp_rate,
                deviation+max_modulation_freq,
                3000,
                firdes.WIN_HAMMING,
                6.76))
        self.digital_clock_recovery_mm_xx_0_0_0 = digital.clock_recovery_mm_ff((audio_samp_rate/100.0) / 200, 0.25*0.175*0.175, 0.5, 0.175, 0.005)
        self.digital_binary_slicer_fb_0_0 = digital.binary_slicer_fb()
        self.dc_blocker_xx_0 = filter.dc_blocker_ff(1024, True)
        self.analog_quadrature_demod_cf_0_0 = analog.quadrature_demod_cf(1.2)
        self.analog_quadrature_demod_cf_0 = analog.quadrature_demod_cf(1.0)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_frame_decoder_0, 'out'), (self.satnogs_json_converter_0, 'in'))
        self.msg_connect((self.satnogs_json_converter_0, 'out'), (self.satnogs_frame_file_sink_0_1_0, 'frame'))
        self.msg_connect((self.satnogs_json_converter_0, 'out'), (self.satnogs_udp_msg_sink_0_0, 'in'))
        self.msg_connect((self.satnogs_tcp_rigctl_msg_source_0, 'freq'), (self.satnogs_doppler_compensation_0, 'doppler'))
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.satnogs_ogg_encoder_0, 0))
        self.connect((self.analog_quadrature_demod_cf_0_0, 0), (self.low_pass_filter_1, 0))
        self.connect((self.dc_blocker_xx_0, 0), (self.root_raised_cosine_filter_0, 0))
        self.connect((self.digital_binary_slicer_fb_0_0, 0), (self.satnogs_frame_decoder_0, 0))
        self.connect((self.digital_clock_recovery_mm_xx_0_0_0, 0), (self.digital_binary_slicer_fb_0_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.analog_quadrature_demod_cf_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.analog_quadrature_demod_cf_0_0, 0))
        self.connect((self.low_pass_filter_1, 0), (self.dc_blocker_xx_0, 0))
        self.connect((self.root_raised_cosine_filter_0, 0), (self.digital_clock_recovery_mm_xx_0_0_0, 0))
        self.connect((self.satnogs_doppler_compensation_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.satnogs_doppler_compensation_0, 0), (self.satnogs_iq_sink_0, 0))
        self.connect((self.satnogs_doppler_compensation_0, 0), (self.satnogs_waterfall_sink_0, 0))
        self.connect((self.soapy_source_0, 0), (self.satnogs_doppler_compensation_0, 0))

    def get_antenna(self):
        return self.antenna

    def set_antenna(self, antenna):
        self.antenna = antenna
        self.soapy_source_0.set_antenna(0,self.antenna)

    def get_bw(self):
        return self.bw

    def set_bw(self, bw):
        self.bw = bw
        self.soapy_source_0.set_bandwidth(0,self.bw)

    def get_decoded_data_file_path(self):
        return self.decoded_data_file_path

    def set_decoded_data_file_path(self, decoded_data_file_path):
        self.decoded_data_file_path = decoded_data_file_path

    def get_dev_args(self):
        return self.dev_args

    def set_dev_args(self, dev_args):
        self.dev_args = dev_args

    def get_doppler_correction_per_sec(self):
        return self.doppler_correction_per_sec

    def set_doppler_correction_per_sec(self, doppler_correction_per_sec):
        self.doppler_correction_per_sec = doppler_correction_per_sec

    def get_enable_iq_dump(self):
        return self.enable_iq_dump

    def set_enable_iq_dump(self, enable_iq_dump):
        self.enable_iq_dump = enable_iq_dump

    def get_file_path(self):
        return self.file_path

    def set_file_path(self, file_path):
        self.file_path = file_path

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.soapy_source_0.set_overall_gain(0,self.gain, False  )

    def get_iq_file_path(self):
        return self.iq_file_path

    def set_iq_file_path(self, iq_file_path):
        self.iq_file_path = iq_file_path
        self.satnogs_iq_sink_0.open(self.iq_file_path)

    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset
        self.soapy_source_0.set_frequency(0, self.rx_freq - self.lo_offset)

    def get_rigctl_port(self):
        return self.rigctl_port

    def set_rigctl_port(self, rigctl_port):
        self.rigctl_port = rigctl_port

    def get_rx_freq(self):
        return self.rx_freq

    def set_rx_freq(self, rx_freq):
        self.rx_freq = rx_freq
        self.soapy_source_0.set_frequency(0, self.rx_freq - self.lo_offset)

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx

    def get_soapy_rx_device(self):
        return self.soapy_rx_device

    def set_soapy_rx_device(self, soapy_rx_device):
        self.soapy_rx_device = soapy_rx_device

    def get_udp_IP(self):
        return self.udp_IP

    def set_udp_IP(self, udp_IP):
        self.udp_IP = udp_IP

    def get_udp_port(self):
        return self.udp_port

    def set_udp_port(self, udp_port):
        self.udp_port = udp_port

    def get_waterfall_file_path(self):
        return self.waterfall_file_path

    def set_waterfall_file_path(self, waterfall_file_path):
        self.waterfall_file_path = waterfall_file_path

    def get_variable_amsat_duv_decoder_0(self):
        return self.variable_amsat_duv_decoder_0

    def set_variable_amsat_duv_decoder_0(self, variable_amsat_duv_decoder_0):
        self.variable_amsat_duv_decoder_0 = variable_amsat_duv_decoder_0

    def get_max_modulation_freq(self):
        return self.max_modulation_freq

    def set_max_modulation_freq(self, max_modulation_freq):
        self.max_modulation_freq = max_modulation_freq
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.audio_samp_rate, self.deviation+self.max_modulation_freq, 3000, firdes.WIN_HAMMING, 6.76))

    def get_deviation(self):
        return self.deviation

    def set_deviation(self, deviation):
        self.deviation = deviation
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.audio_samp_rate, self.deviation+self.max_modulation_freq, 3000, firdes.WIN_HAMMING, 6.76))

    def get_audio_samp_rate(self):
        return self.audio_samp_rate

    def set_audio_samp_rate(self, audio_samp_rate):
        self.audio_samp_rate = audio_samp_rate
        self.digital_clock_recovery_mm_xx_0_0_0.set_omega((self.audio_samp_rate/100.0) / 200)
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.audio_samp_rate, self.deviation+self.max_modulation_freq, 3000, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_1.set_taps(firdes.low_pass(1, self.audio_samp_rate, 195, 10, firdes.WIN_HAMMING, 6.76))


def argument_parser():
    description = 'A DUV Decoder for the AMSAT FOX satellites'
    parser = ArgumentParser(description=description)
    parser.add_argument(
        "--antenna", dest="antenna", type=str, default='',
        help="Set antenna [default=%(default)r]")
    parser.add_argument(
        "--bw", dest="bw", type=eng_float, default="0.0",
        help="Set Bandwidth [default=%(default)r]")
    parser.add_argument(
        "--decoded-data-file-path", dest="decoded_data_file_path", type=str, default='/tmp/.satnogs/data/data',
        help="Set decoded_data_file_path [default=%(default)r]")
    parser.add_argument(
        "--dev-args", dest="dev_args", type=str, default='',
        help="Set Device arguments [default=%(default)r]")
    parser.add_argument(
        "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type=intx, default=20,
        help="Set doppler_correction_per_sec [default=%(default)r]")
    parser.add_argument(
        "--enable-iq-dump", dest="enable_iq_dump", type=intx, default=0,
        help="Set enable_iq_dump [default=%(default)r]")
    parser.add_argument(
        "--file-path", dest="file_path", type=str, default='test.wav',
        help="Set file_path [default=%(default)r]")
    parser.add_argument(
        "--gain", dest="gain", type=eng_float, default="0.0",
        help="Set gain [default=%(default)r]")
    parser.add_argument(
        "--iq-file-path", dest="iq_file_path", type=str, default='/tmp/iq.dat',
        help="Set iq_file_path [default=%(default)r]")
    parser.add_argument(
        "--lo-offset", dest="lo_offset", type=eng_float, default="100.0k",
        help="Set lo_offset [default=%(default)r]")
    parser.add_argument(
        "--rigctl-port", dest="rigctl_port", type=intx, default=4532,
        help="Set rigctl_port [default=%(default)r]")
    parser.add_argument(
        "--rx-freq", dest="rx_freq", type=eng_float, default="100.0M",
        help="Set rx_freq [default=%(default)r]")
    parser.add_argument(
        "--samp-rate-rx", dest="samp_rate_rx", type=eng_float, default="0.0",
        help="Set Device Sampling rate [default=%(default)r]")
    parser.add_argument(
        "--soapy-rx-device", dest="soapy_rx_device", type=str, default='driver=invalid',
        help="Set soapy_rx_device [default=%(default)r]")
    parser.add_argument(
        "--udp-IP", dest="udp_IP", type=str, default='127.0.0.1',
        help="Set udp_IP [default=%(default)r]")
    parser.add_argument(
        "--udp-port", dest="udp_port", type=intx, default=16887,
        help="Set udp_port [default=%(default)r]")
    parser.add_argument(
        "--waterfall-file-path", dest="waterfall_file_path", type=str, default='/tmp/waterfall.dat',
        help="Set waterfall_file_path [default=%(default)r]")
    return parser


def main(top_block_cls=satnogs_amsat_fox_duv_decoder, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(antenna=options.antenna, bw=options.bw, decoded_data_file_path=options.decoded_data_file_path, dev_args=options.dev_args, doppler_correction_per_sec=options.doppler_correction_per_sec, enable_iq_dump=options.enable_iq_dump, file_path=options.file_path, gain=options.gain, iq_file_path=options.iq_file_path, lo_offset=options.lo_offset, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, samp_rate_rx=options.samp_rate_rx, soapy_rx_device=options.soapy_rx_device, udp_IP=options.udp_IP, udp_port=options.udp_port, waterfall_file_path=options.waterfall_file_path)

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()
        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
