# -*- coding: utf-8 -*-

import os.path

# from . import call_home
from . import dpu_test, checksum

# def printbin():
#     file_to_read = os.path.dirname(__file__) + "/mockup_dpu_binary"
#     return call_home(file_to_read)


def test_dpu_bin():
    file_to_read = os.path.dirname(__file__) + "/dpu_program/helloworld"
    return dpu_test(file_to_read)


def test_checksum():
    file_to_read = os.path.dirname(__file__) + "/dpu_program/trivial_checksum_example"
    return f"{checksum(file_to_read):#0{10}x}"
