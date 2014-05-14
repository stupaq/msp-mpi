#!/usr/bin/env bash

SUBMIT="`dirname $0`/../submit-par.sh"
REPORT="`dirname $0`/../corr-report.sh"
ERRS="`dirname $SUBMIT`/*.err"

m=43
n=27
seed=123

rm -f $ERRS
echo "Submitting M=$m N=$n S=$seed"
$SUBMIT -m $m -n $n -s $seed -p 4 -A -s
$REPORT $ERRS
