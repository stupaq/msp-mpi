#!/usr/bin/env bash

for m in 1000 2000 4000; do
  for n in 1000 2000 4000; do
    [[ m -eq 4000 && n -eq 4000 ]] && extra='-A -s'
    ./submit-par.sh -m $m -n $n -s 123 $extra
  done
done

./perf-report.sh *.err
cat report.csv
