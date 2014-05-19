#!/usr/bin/env bash
set -e

export S=${S:-123}
export P=${P:-32}
export C=${C:-4}

while getopts ":m:n:p:c:s:q:A:QM:" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    p) export P=$OPTARG ;;
    c) export C=$OPTARG ;;
    s) export S=$OPTARG ;;
    q) export Q=$OPTARG ;;
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

job_name="MSP_${M}x${N}_S-${S}_P-${P}_C-${C}"
if [[ -n $make && -e "$make/$job_name.err" ]]; then
  echo "Report file present, skipping job $job_name"
  exit 0
fi

if [[ -z $quiet ]]; then
  ./msp-par.ll-gen.sh
fi

./msp-par.ll-gen.sh | llsubmit $extra -

