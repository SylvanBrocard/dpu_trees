# -*- coding: utf-8 -*-
from ._core import __doc__, __version__, add, subtract, MyClass

from ._checksum import test_checksum


def add2(x: int, y: int) -> int:
    return add(x, y)
