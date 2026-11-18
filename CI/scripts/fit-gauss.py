#!/usr/bin/env python

import yoda
import sys
from math import sqrt
import numpy as np
from scipy.optimize import curve_fit

PASS=True

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'




def gaussian(x, a, sigma):
    return np.exp(-(x - a)**2 / (2 * sigma**2)) / (sigma * sqrt(2 * np.pi))

files = sys.argv[1:]
Mode=files[1]
Mode=Mode.strip(".yoda")

dev_h = list(yoda.read(files[0]).values())[0]
dev_x = []
dev_y = []
sumW = 0
for bin in dev_h:
    if bin.sumW() == 0 or np.isinf(bin.xMin()):
        continue
    dev_x.append(bin.xMean())
    dev_y.append(bin.sumW() / bin.xWidth())
    sumW += bin.sumW()

dev_x = np.array(dev_x)
dev_y = np.array(dev_y) / sumW


if len(dev_y)<=2:
    print(f"No histogram found for {Mode}")
    sys.exit(0)

coeff, var_matrix = curve_fit(gaussian, dev_x, dev_y)
mean, std = coeff

print("The mean for"+ bcolors.OKBLUE + f" {Mode} is"+\
       bcolors.OKGREEN + f" {mean:.2f} " + bcolors.ENDC +\
      " with standard deviation " + bcolors.OKGREEN + f" {std:.2f} "+ bcolors.ENDC)


if std > 1.2 or abs(mean) > 0.5:
    print(bcolors.FAIL+"ERROR"+bcolors.ENDC+" Large deviation in " + bcolors.OKBLUE + f"{Mode}"+ bcolors.ENDC)
    # sys.exit(1)
    PASS=False

if PASS:
    sys.exit(0)
else:
    sys.exit(1)
