#!/usr/bin/env bash
set -e

export S=${S:-123}

while getopts ":m:n:s:v:q:aA:QM:" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    s) export S=$OPTARG ;;
    v) export V=$OPTARG ;;
    q) export Q=$OPTARG ;;
    a) export A='yes' ;;
    A) extra="$extra $OPTARG" ;;
    Q) quiet='yes' ;;
    M) make="$OPTARG" ;;
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

job_name="MSP-${V}_${M}x${N}_S-${S}_seq_fast-${A:-no}"
if [[ -n $make && -e "$make/$job_name.err" ]]; then
  echo "Report file present, skipping job $job_name"
  exit 0
fi

if [[ -z $quiet ]]; then
  ./msp-seq.ll-gen.sh
fi

./msp-seq.ll-gen.sh | llsubmit $extra -

