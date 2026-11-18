#!/usr/bin/env python

import yoda
import sys

PASS = True


class bcolors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


def GetResults(hist):
    # if hist.type() == "Histo1D":
    xs = 0
    err = 0
    if int(yoda.version()[0]) >= 2:
        xs = hist.bins()[0].sumW()
        err = hist.bins()[0].errW()
    else:
        xs = hist.bins()[0].height()
        err = hist.bins()[0].heightErr()
    return xs, err


reffile = sys.argv[1]
file = sys.argv[2]
Mode = sys.argv[3]
Mode = Mode.strip(".yoda")

ref_h = yoda.read(reffile)
check_h = yoda.read(file)

# check for xs

if "/kkmc-leptons/xs" not in ref_h:
    print(f"No cross-section found in reference for {Mode}")
    exit(1)

if "/kkmc-leptons/xs" not in check_h:
    print(f"No cross-section found in generated yoda for {Mode}")
    exit(1)

ref_xs, ref_err = GetResults(ref_h["/kkmc-leptons/xs"])
check_xs, check_xserr = GetResults(check_h["/kkmc-leptons/xs"])

check_min = check_xs - check_xserr
check_max = check_xs + check_xserr

if ref_xs > check_min and ref_xs < check_max:
    good = True
else:
    good = False

if not good:
    print(
        bcolors.FAIL
        + "ERROR"
        + bcolors.ENDC
        + " Large deviation in "
        + bcolors.OKBLUE
        + f"{Mode}"
        + bcolors.ENDC
    )
    print(bcolors.OKBLUE + f"Fortan v4.30:" + bcolors.OKGREEN + f" {ref_xs}")
    print(
        bcolors.OKBLUE
        + f"C++ Current:"
        + bcolors.OKGREEN
        + f"{check_xs} +- {check_xserr}"
        + bcolors.ENDC
    )
    PASS = False


else:
    print(f"Cross Sections agree within statistical errors for {Mode}")
    print(bcolors.OKBLUE + f"Fortan v4.30:" + bcolors.OKGREEN + f" {ref_xs}")
    print(
        bcolors.OKBLUE
        + f"C++ Current:"
        + bcolors.OKGREEN
        + f"{check_xs} +- {check_xserr}"
        + bcolors.ENDC
    )

print("=" * 20)

if PASS:
    sys.exit(0)
else:
    sys.exit(1)
