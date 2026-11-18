> This is a fork of [KrakowHEPSoft/KKMCee](https://github.com/KrakowHEPSoft/KKMCee) with the changes used to build KKMCee in the Key4hep stack.
> The `LCG` branch started from the upstream [`v5.00.02` tag](https://github.com/KrakowHEPSoft/KKMCee/releases/tag/v5.00.02).

# Standard user download and installation intructions
The current version is KKMCee 5.02 which can be downloaded from: 
https://github.com/KrakowHEPSoft/KKMCee/releases

For installation, we recommend following the [wiki installation page]( https://github.com/KrakowHEPSoft/KKMCee/wiki)

# KKMCee-dev
KKMCee Development

```bash
# ... clone the KKMCee git repository, e.g.:
git clone --depth 1 --branch FCC_release_cpp https://github.com/KrakowHEPSoft/KKMCee.git KKMCee-dev
# ... and then ...
cd KKMCee-dev
rm -f autoreconf.out.txt configure.out.txt make.out.txt
ln -f -s dizet-6.45 dizet # or take dizet-6.42 or dizet-6.21
# warning: on Debian / Ubuntu, one needs to compile and use the standard "GNU Libtool"
nice autoreconf -f -i |& tee -a autoreconf.out.txt
# see "./configure --help" for available options
# note: GCC < 6.1 may need, e.g.: nice ./configure CXXFLAGS="-std=c++11 -g -O2" ...
nice ./configure |& tee -a configure.out.txt
nice make |& tee -a make.out.txt
```

