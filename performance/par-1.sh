#!/usr/bin/env bash

SUBMIT="`dirname $0`/../submit-par.sh"
REPORT="`dirname $0`/../perf-report.sh"
ERRS="`dirname $SUBMIT`/*.err"

seed=123

rm -f $ERRS
for m in 1000 2000 4000; do
  for n in 1000 2000 4000; do
    echo "Submitting M=$m N=$n S=$seed"
    if [[ m -eq 4000 && n -eq 4000 ]]; then
      extra='-A -s'
    fi
    $SUBMIT -m $m -n $n -s $seed $extra
  done
done
$REPORT $ERRS
cat report.csv
