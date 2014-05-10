#!/usr/bin/env bash

SUBMIT="`dirname $0`/../submit.sh"
REPORT="`dirname $0`/../perf-report.sh"
ERRS="`dirname $SUBMIT`/*.err"

n=4000
m=4000
seed=123

rm -f $ERRS
echo "Submitting M=$m N=$n S=$seed"
$SUBMIT -m $m -n $n -s $seed -A -s
$REPORT $ERRS
cat report.csv
