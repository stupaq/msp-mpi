#!/usr/bin/env bash

while getopts ":m:n:p:c:s:" opt; do
  case $opt in
    m) export M=$OPTARG ;;
    n) export N=$OPTARG ;;
    p) export P=$OPTARG ;;
    c) export C=$OPTARG ;;
    s) export S=$OPTARG ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1 ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1 ;;
  esac
done

llsubmit <(./msp-par.ll-gen.sh)

