#!/usr/bin/env python3
"""
Extract data from binary files and save in MATLAB format
"""
import sys
import numpy as np
import matplotlib.pyplot as plt
from MDSplus import Tree

mdsTreeName = 'rtappisttok'
#mdsPulseNumber = int(sys.argv[1])
mdsPulseNumber = 1

try:
    tree = Tree(mdsTreeName, mdsPulseNumber)
except:
    print(f'Failed opening {mdsTreeName} for pulse number {mdsPulseNumber:d}')
    exit()

