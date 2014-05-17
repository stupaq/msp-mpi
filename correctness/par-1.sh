#!/usr/bin/env bash

for m in `seq 5`; do
  for n in `seq 5`; do
    [[ m -eq 5 && n -eq 5 ]] && extra='-A -s'
    ./submit-par.sh -m $m -n $n -s 17326 -p 4 -Q $extra
  done
done

./corr-report.sh *.err
