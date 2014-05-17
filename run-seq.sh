#!/usr/bin/env bash

M=${M:-1000}
N=${N:-1000}
S=${S:-123}
V=

while getopts ":m:n:s:v:" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    s) export S=$OPTARG ;;
    v) export V=$OPTARG ;;
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

job_name="MSP-${V}_${M}x${N}_S-${S}_seq_run"
./msp-seq-${V}.exe ${M} ${N} ${S} 1>$job_name.out 2>$job_name.err

