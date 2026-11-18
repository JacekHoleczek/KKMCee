#! /bin/bash

set -e
export RPM_ARCH=$(uname -m)
export RPM_PACKAGE_VERSION=1.0
export RPM_PACKAGE_RELEASE=1
export RPM_PACKAGE_NAME="KKMC"
set -x
TOPDIR=${PWD}
KKMC=${PWD}/../../ProdRun/MainCluster
PHYSVAL=${PWD}/../physics-val
export RIVET_ANALYSIS_PATH=${PWD}

rivet_major_version=$(rivet --version | sed -n 's/[^0-9]*\([0-9]*\)\..*/\1/p')

if [ "$rivet_major_version" -eq 3 ]; then
  echo "Rivet major version is $rivet_major_version."
  rivet-build leptonsRivet3.cc
elif [ "$rivet_major_version" -eq 4 ]; then
  echo "Rivet major version is $rivet_major_version."
  rivet-build leptons.cc
else
  echo "Rivet major version is $rivet_major_version, which is not supported"
fi


function RunKKMCEEX(){
	mkdir -p $PHYSVAL/${dir}/yodafiles/${mode} $PHYSVAL/${dir}/logfiles/${mode}/
	if [[ -p "kkmc.hepmc3" ]]; then
	 rm -rf kkmc.hepmc3
	fi
	cp ${PHYSVAL}/$dir/runcards/${mode}-alpha${order}.input  pro.input
	cp ${PHYSVAL}/$dir/runcards/KKMCee_defaults  .
	cp ${PHYSVAL}/$dir/runcards/DIZET-table1  .
	mkfifo kkmc.hepmc3
	if [[ "ceex" == *"$mode"* ]]; then
    	    NEVENTS=100000
	else
    	    NEVENTS=100000
	fi
	${KKMC} -n $NEVENTS | tee  $PHYSVAL/${dir}/logfiles/${mode}/alpha${order}.log  &
	rivet --ignore-beams -a kkmc-leptons -o $PHYSVAL/${dir}/yodafiles/${mode}/alpha${order}.yoda kkmc.hepmc3
	}

for dir in $(ls ${PHYSVAL}); do
	for input in $(ls ${PHYSVAL}/$dir/runcards/*.input); do
		mode=$(basename $input)
		mode=${mode//.input}
		order=${mode: -1}
		mode=${mode//-alpha$order}
		echo $mode
		cwd=${PWD}
		mkdir -p ../run-dir/${mode}
		cd ../run-dir/${mode}
		RunKKMCEEX $mode
		cd ${cwd}
	done 
done

wait
./check-histos.sh
