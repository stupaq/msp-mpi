#!/usr/bin/env bash

extra=""
while getopts ":m:n:s:v:A:Q" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    s) export S=$OPTARG ;;
    v) export V=$OPTARG ;;
    A) extra="$extra $OPTARG" ;;
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

if [[ -z $quiet ]]; then
  ./msp-seq.ll-gen.sh
fi

./msp-seq.ll-gen.sh | llsubmit $extra -

