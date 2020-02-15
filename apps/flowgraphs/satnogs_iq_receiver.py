#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: satnogs_iq_receiver
# Author: Manolis Surligas (surligas@gmail.com)
# Description: Generic IQ receiver with arbitrary output sampling rate
# GNU Radio version: 3.8.0.0

from gnuradio import gr
from gnuradio.filter import firdes
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import math
import satnogs
import soapy

class satnogs_iq_receiver(gr.top_block):

    def __init__(self, antenna="", bw=0.0, dev_args="", doppler_correction_per_sec=20, enable_doppler_correction=1, enable_iq_dump=0, file_path="test.wav", gain=0.0, iq_file_path="/tmp/iq.dat", lo_offset=100e3, out_samp_rate=0.0, rigctl_port=4532, rx_freq=100e6, samp_rate_rx=0.0, soapy_rx_device="driver=invalid", waterfall_file_path="/tmp/waterfall.dat"):
        gr.top_block.__init__(self, "satnogs_iq_receiver")

        ##################################################
        # Parameters
        ##################################################
        self.antenna = antenna
        self.bw = bw
        self.dev_args = dev_args
        self.doppler_correction_per_sec = doppler_correction_per_sec
        self.enable_doppler_correction = enable_doppler_correction
        self.enable_iq_dump = enable_iq_dump
        self.file_path = file_path
        self.gain = gain
        self.iq_file_path = iq_file_path
        self.lo_offset = lo_offset
        self.out_samp_rate = out_samp_rate
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.samp_rate_rx = samp_rate_rx
        self.soapy_rx_device = soapy_rx_device
        self.waterfall_file_path = waterfall_file_path

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
            self.soapy_source_0 = soapy.source(1, dev, f, samp_rate_rx, "fc32")
        else:
            self.soapy_source_0 = soapy.source(1, dev, dev_args, samp_rate_rx, "fc32")

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

        if "False" == 'True':
            self.soapy_source_0.set_gain(0,gain)
            self.soapy_source_0.set_gain(1,10)
        else:
            if "custom" == 'uhd' or "custom" == 'sidekiq' or "custom" == 'bladerf' or "custom" == 'lime':
                self.soapy_source_0.set_gain(0,"PGA", 24)
                self.soapy_source_0.set_gain(1,"PGA", 24)
            else:
                if "custom" == 'custom':
                    # If we're here and we're custom, let's still call overall gain
                    self.soapy_source_0.set_gain(0,gain)
                    self.soapy_source_0.set_gain(1,10)
                elif "custom" == 'hackrf':
                    self.soapy_source_0.set_gain(0,"LNA", 10)
                    self.soapy_source_0.set_gain(0,"VGA", 10)
                    # Only hackrf uses "AMP", so just ch0
                    self.soapy_source_0.set_gain(0,"AMP", 0)
                else:
                    self.soapy_source_0.set_gain(0,"LNA", 10)
                    self.soapy_source_0.set_gain(1,"LNA", 10)
                    self.soapy_source_0.set_gain(0,"TIA", 0)
                    self.soapy_source_0.set_gain(1,"TIA", 0)
                    self.soapy_source_0.set_gain(0,"MIX", 10)
                    self.soapy_source_0.set_gain(1,"MIX", 10)
                    self.soapy_source_0.set_gain(0,"VGA", 10)
                    self.soapy_source_0.set_gain(1,"VGA", 10)
                    # Only rtl-sdr uses TUNER, so just ch0
                    self.soapy_source_0.set_gain(0,"TUNER", 10)
                    # Only sdrplay uses IFGR so just ch0 for each
                    self.soapy_source_0.set_gain(0,"IFGR", 59)
                    self.soapy_source_0.set_gain(0,"RFGR", 9)
        self.satnogs_waterfall_sink_0 = satnogs.waterfall_sink(out_samp_rate, rx_freq, 10, 1024, waterfall_file_path, 1)
        self.satnogs_tcp_rigctl_msg_source_0 = satnogs.tcp_rigctl_msg_source("127.0.0.1", rigctl_port, False, int(1000.0/doppler_correction_per_sec) + 1, 1500)
        self.satnogs_iq_sink_0 = satnogs.iq_sink(16768, iq_file_path, False, enable_iq_dump)
        self.satnogs_doppler_compensation_0 = satnogs.doppler_compensation(samp_rate_rx, rx_freq, lo_offset, out_samp_rate, enable_doppler_correction, 0)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.satnogs_tcp_rigctl_msg_source_0, 'freq'), (self.satnogs_doppler_compensation_0, 'doppler'))
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

    def get_dev_args(self):
        return self.dev_args

    def set_dev_args(self, dev_args):
        self.dev_args = dev_args

    def get_doppler_correction_per_sec(self):
        return self.doppler_correction_per_sec

    def set_doppler_correction_per_sec(self, doppler_correction_per_sec):
        self.doppler_correction_per_sec = doppler_correction_per_sec

    def get_enable_doppler_correction(self):
        return self.enable_doppler_correction

    def set_enable_doppler_correction(self, enable_doppler_correction):
        self.enable_doppler_correction = enable_doppler_correction

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
        self.soapy_source_0.set_gain(0,self.gain)

    def get_iq_file_path(self):
        return self.iq_file_path

    def set_iq_file_path(self, iq_file_path):
        self.iq_file_path = iq_file_path

    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset
        self.soapy_source_0.set_frequency(0, self.rx_freq - self.lo_offset)

    def get_out_samp_rate(self):
        return self.out_samp_rate

    def set_out_samp_rate(self, out_samp_rate):
        self.out_samp_rate = out_samp_rate

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

    def get_waterfall_file_path(self):
        return self.waterfall_file_path

    def set_waterfall_file_path(self, waterfall_file_path):
        self.waterfall_file_path = waterfall_file_path


def argument_parser():
    description = 'Generic IQ receiver with arbitrary output sampling rate'
    parser = ArgumentParser(description=description)
    parser.add_argument(
        "--antenna", dest="antenna", type=str, default="",
        help="Set antenna [default=%(default)r]")
    parser.add_argument(
        "--bw", dest="bw", type=eng_float, default="0.0",
        help="Set Bandwidth [default=%(default)r]")
    parser.add_argument(
        "--dev-args", dest="dev_args", type=str, default="",
        help="Set Device arguments [default=%(default)r]")
    parser.add_argument(
        "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type=intx, default=20,
        help="Set doppler_correction_per_sec [default=%(default)r]")
    parser.add_argument(
        "--enable-doppler-correction", dest="enable_doppler_correction", type=intx, default=1,
        help="Set enable_doppler_correction [default=%(default)r]")
    parser.add_argument(
        "--enable-iq-dump", dest="enable_iq_dump", type=intx, default=0,
        help="Set enable_iq_dump [default=%(default)r]")
    parser.add_argument(
        "--file-path", dest="file_path", type=str, default="test.wav",
        help="Set file_path [default=%(default)r]")
    parser.add_argument(
        "--gain", dest="gain", type=eng_float, default="0.0",
        help="Set gain [default=%(default)r]")
    parser.add_argument(
        "--iq-file-path", dest="iq_file_path", type=str, default="/tmp/iq.dat",
        help="Set iq_file_path [default=%(default)r]")
    parser.add_argument(
        "--lo-offset", dest="lo_offset", type=eng_float, default="100.0k",
        help="Set lo_offset [default=%(default)r]")
    parser.add_argument(
        "--out-samp-rate", dest="out_samp_rate", type=eng_float, default="0.0",
        help="Set Output sampling rate [default=%(default)r]")
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
        "--soapy-rx-device", dest="soapy_rx_device", type=str, default="driver=invalid",
        help="Set soapy_rx_device [default=%(default)r]")
    parser.add_argument(
        "--waterfall-file-path", dest="waterfall_file_path", type=str, default="/tmp/waterfall.dat",
        help="Set waterfall_file_path [default=%(default)r]")
    return parser


def main(top_block_cls=satnogs_iq_receiver, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(antenna=options.antenna, bw=options.bw, dev_args=options.dev_args, doppler_correction_per_sec=options.doppler_correction_per_sec, enable_doppler_correction=options.enable_doppler_correction, enable_iq_dump=options.enable_iq_dump, file_path=options.file_path, gain=options.gain, iq_file_path=options.iq_file_path, lo_offset=options.lo_offset, out_samp_rate=options.out_samp_rate, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, samp_rate_rx=options.samp_rate_rx, soapy_rx_device=options.soapy_rx_device, waterfall_file_path=options.waterfall_file_path)

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
