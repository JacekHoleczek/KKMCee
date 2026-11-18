#!/bin/bash

TOPDIR=${PWD}
KKMC=${PWD}/../../ProdRun//MainCluster
PHYSVAL=${PWD}/../physics-val
PLOTS=${PWD}/../plots

IGNORE_HIST=("KK2f"
			 "MBrA"
			 "Kar"
			 "Weight"
			 "YFSfin"
			 "nbar"
			 "photon_"
			 "k_")


IGNORE=""
for ig in "${IGNORE_HIST[@]}"; do IGNORE="${IGNORE} -M ${ig}"; done;


PASS=0
function CheckYodas(){
	refdir=${PHYSVAL}/${dir}/ref-results/${refmode}
	newdir=${PHYSVAL}/${dir}/yodafiles/${refmode}
	for ref in $(ls $refdir); do
		if [[ -f ${newdir}/$ref ]]; then
			yodadiff -q -M "/_.*,/RAW/.*" -d ${PHYSVAL}/${dir}/deviations.yoda ${refdir}/$ref ${newdir}/$ref
			python3 fit-gauss.py ${PHYSVAL}/${dir}/deviations.yoda ${refmode}-${ref}
			python3 check-xs.py  ${refdir}/$ref  ${newdir}/$ref ${refmode}-${ref}
			if [[ $? -eq 1 ]]; then
				PASS=1
			fi
		fi
	done
}

function MakePlots(){
	refdir=${PHYSVAL}/${dir}/ref-results/${refmode}
	newdir=${PHYSVAL}/${dir}/yodafiles/${refmode}
	for ref in $(ls $refdir); do
		if [[ -f ${newdir}/$ref ]]; then
			order=${ref//.yoda/}
			rivet-mkhtml-mpl --no-errs  ${IGNORE} -c plots.conf ${refdir}/$ref:"Fortran v.4.3" ${newdir}/$ref:"Commit ${CI_COMMIT_SHA}" -o ${PLOTS}/${OS}${V}/${refmode}-${order}
		fi
	done
}




for dir in $(ls $PHYSVAL/); do
	for refmode in $(ls ${PHYSVAL}/${dir}/ref-results); do
		MakePlots
	done
	for refmode in $(ls ${PHYSVAL}/${dir}/ref-results); do
		if [[ "$refmode" == *"ceex-isr-fsr-qed"* ]]; then
			echo "Skipping $refmode"
			continue
		fi
		CheckYodas
	done
done

exit $PASS
