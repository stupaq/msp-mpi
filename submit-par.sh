#!/usr/bin/env bash

extra=""
while getopts ":m:n:p:c:s:A:" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    p) export P=$OPTARG ;;
    c) export C=$OPTARG ;;
    s) export S=$OPTARG ;;
    A) extra="$extra $OPTARG" ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1 ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1 ;;
  esac
done

./msp-par.ll-gen.sh | llsubmit $extra -

