#!/usr/bin/env bash
set -e

export S=${S:-123}
export P=${P:-32}

while getopts ":m:n:s:p:M:Q" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    s) export S=$OPTARG ;;
    p) export P=$OPTARG ;;
    M) make="$OPTARG" ;;
    Q) quiet='yes' ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1 ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1 ;;
  esac
done

job_name="MSP_${M}x${N}_S-${S}_H-`hostname`"
if [[ -n $make && -e "$make/$job_name.err" ]]; then
  echo "Report file present, skipping job $job_name"
  exit 0
fi

if [[ $quiet != 'yes' ]]; then
  echo "$job_name"
fi
mpirun -np ${P} ./msp-par.exe ${M} ${N} ${S} 1>$job_name.out 2>$job_name.err

