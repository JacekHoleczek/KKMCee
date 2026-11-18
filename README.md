> This is a fork of [KrakowHEPSoft/KKMCee](https://github.com/KrakowHEPSoft/KKMCee) with the changes used to build KKMCee in the Key4hep stack.
> The `LCG` branch started from the upstream [`v5.00.02` tag](https://github.com/KrakowHEPSoft/KKMCee/releases/tag/v5.00.02).

# KKMCee-dev
KKMCee Development

```bash
# ... clone the KKMCee git repository, e.g.:
git clone --depth=1 --branch FCC_release_cpp https://github.com/KrakowHEPSoft/KKMCee.git KKMCee-dev
# ... and then ...
cd KKMCee-dev
rm -f autoreconf.out.txt configure.out.txt make.out.txt
ln -f -s dizet-6.45 dizet # or take dizet-6.42 or dizet-6.21
nice autoreconf -f -i 2>&1 | tee -a autoreconf.out.txt
# see "./configure --help" for available options
nice ./configure 2>&1 | tee -a configure.out.txt
nice make 2>&1 | tee -a make.out.txt
```
