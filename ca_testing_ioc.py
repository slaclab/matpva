# ---------------------------------------------------------------------------
# Title      : ca test ioc
# ---------------------------------------------------------------------------
# File       : ca_testing_ioc.py
# Author     : Kuktae Kim, ktkim@slac.stanford.edu
# Created    : 2025-03-04
# Last update: 2025-03-04
# ---------------------------------------------------------------------------
# Description:
#    Create various Channel Access PVs using caproto.
# ---------------------------------------------------------------------------
# This file is part of matpva. It is subject to the license terms in the 
# LICENSE.txt file found in the top-level directory of this distribution
# and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
# No part of matpva, including this file, may be copied, modified, 
# propagated, or distributed except according to the terms contained in 
# the LICENSE.txt file.
# ---------------------------------------------------------------------------
import time
import random
import threading
import numpy as np
from textwrap import dedent
from caproto.server import pvproperty, PVGroup, ioc_arg_parser, run


def gaussian_2d(x, y, x0, y0, xsig, ysig):
    """Return a 2D Gaussian."""
    return np.exp(-0.5 * (((x - x0) / xsig) ** 2 + ((y - y0) / ysig) ** 2))


class MyPVGroup(PVGroup):
    """PV Group for Channel Access test IOC."""
    
    # Scalar PVs
    #IntValue = pvproperty(value=5, alarm_low=2, warning_low=3, alarm_high=9, warning_high=8)
    IntValue = pvproperty(value=5)
    IntValue2 = pvproperty(value=3)
    uIntValue = pvproperty(value=1333333333)
    ByteValue = pvproperty(value=12)
    uByteValue = pvproperty(value=77)
    ShortValue = pvproperty(value=1)
    uShortValue = pvproperty(value=10000)
    LongValue = pvproperty(value=12345)
    uLongValue = pvproperty(value=1234567890123456789)
    FloatValue = pvproperty(value=0.0)
    DoubleValue = pvproperty(value=0.0)
    BoolValue = pvproperty(value=False)
    StringValue = pvproperty(value='Test!', dtype=str, string_encoding='utf-8', report_as_string=True, max_length=255,)

    # Array PVs
    IntArray = pvproperty(value=[1, 2, 3, 4, 5])
    uIntArray = pvproperty(value=[5, 4, 3, 2, 1])
    ByteArray = pvproperty(value=[0, 1, 1, 0, 0])
    uByteArray = pvproperty(value=[0, 1, 1, 0, 0])
    ShortArray = pvproperty(value=[0, 1, 1, 0, 0, 0, 1, 1])
    uShortArray = pvproperty(value=[0, 123, 123, 0, 0, 0, 123, 123])
    LongArray = pvproperty(value=[123, 234, 345])
    uLongArray = pvproperty(value=[123456, 234567, 345678])
    FloatArray = pvproperty(value=[1.5, 2.5, 3.5, 4.5, 5.5])
    DoubleArray = pvproperty(value=[1.5, 2.5, 3.5, 4.5, 5.5])
    Waveform = pvproperty(value=[1.0, 2.0, 3.0, 4.0])
    BoolArray = pvproperty(value=[True, False, True, False])
	# CA doesn't surrpot string array PVs
    #StringArray = pvproperty(value=["One", "Two", "Three"], dtype=str, max_length=255,)

    # Image PV as a flattened array of uint8
    Image = pvproperty(value=np.zeros(1).tolist())

    # We don't have a direct equivalent for NTTable in caproto, but we can create
    # array PVs that represent the table columns
    #nt_table_names = pvproperty(value=['device_1', 'device_2'])
    #nt_table_numbers = pvproperty(value=[1.1, 1.2])


def update_loop(group: MyPVGroup):
    """Continuously update some of the PVs periodically."""
    while True:
        time.sleep(0.7)

        # Update some scalar values with random data
        group.int_value.write(random.randint(1, 100))
        group.float_value.write(random.uniform(1, 100))
        group.bool_value.write(random.choice([True, False]))

        # Update float_array with new random values
        group.float_array.write((np.random.rand(5) * 10).tolist())

        # Generate a 2D Gaussian image (simulate a dynamic image)
        x = np.linspace(-5.0, 5.0, 512)
        y = np.linspace(-5.0, 5.0, 512)
        x0 = 0.5 * (np.random.rand() - 0.5)
        y0 = 0.5 * (np.random.rand() - 0.5)
        xsig = 0.8 - 0.2 * np.random.rand()
        ysig = 0.8 - 0.2 * np.random.rand()
        xgrid, ygrid = np.meshgrid(x, y)
        z = gaussian_2d(xgrid, ygrid, x0, y0, xsig, ysig)
        image_data = np.abs(256.0 * z).flatten().astype(np.uint8).tolist()
        group.image.write(image_data)


if __name__ == '__main__':
    ioc_options, run_options = ioc_arg_parser(
        default_prefix = 'TEST:CA:',
		desc=dedent(MyPVGroup.__doc__))

    # Create the PV group with the specified prefix
    ioc = MyPVGroup(**ioc_options)

    # Start an update thread for dynamic PV updates
    #updater = threading.Thread(target=update_loop, args=(pvgroup,), daemon=True)
    #updater.start()

    print(f'Starting Channel Access server with caproto...')
    print(f"PV prefix: '{ioc_options['prefix']}'")
    #print(f'run_options: {run_options}')

    # Run the server
    run(ioc.pvdb, **run_options)

