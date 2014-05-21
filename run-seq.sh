#!/usr/bin/env bash
set -e

export S=${S:-123}

while getopts ":m:n:s:v:M:Q" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    s) export S=$OPTARG ;;
    v) export V=$OPTARG ;;
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

if [[ -z "$V" ]]; then
  echo "Missing version specification."
  exit 1
fi

job_name="MSP-${V}_${M}x${N}_S-${S}_H-`hostname | cut -c 1-4`"
if [[ -n $make && -e "$make/$job_name.err" ]]; then
  echo "Report file present, skipping job $job_name"
  exit 0
fi

if [[ $quiet != 'yes' ]]; then
  echo "$job_name"
fi
./msp-seq-${V}.exe ${M} ${N} ${S} 1>$job_name.out 2>$job_name.err

