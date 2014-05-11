#!/usr/bin/env bash

SUBMIT="`dirname $0`/../submit-par.sh"
VERIFY="`dirname $0`/../cross-verify.sh"
ERRS="`dirname $SUBMIT`/*.err"

max_m=4
max_n=4
seed=123

rm -f $ERRS
for m in `seq $max_m`; do
  for n in `seq $max_n`; do
    echo "Submitting M=$m N=$n S=$seed"
    if [[ m -eq $max_m && n -eq $max_n ]]; then
      extra='-A -s'
    fi
    $SUBMIT -m $m -n $n -s $seed -p 4 $extra
  done
done
$VERIFY $ERRS
