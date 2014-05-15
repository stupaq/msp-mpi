#!/usr/bin/env bash

COMPARE="`dirname $0`/../compare-seq.sh"

RANDOM=123123
COUNT=10000
MAX_M=50
MAX_N=50

errors=0
for i in `seq $COUNT`; do
  M=$(($RANDOM % $MAX_M + 1))
  N=$(($RANDOM % $MAX_N + 1))
  S=$(($RANDOM + 1))
  echo -ne "kadane\t"
  $COMPARE naive kadane "($M,$N,$S)"
  [[ $? -eq 0 ]] || errors=$(($errors + 1))
  echo -ne "takaoka\t"
  $COMPARE naive takaoka "($M,$N,$S)"
  [[ $? -eq 0 ]] || errors=$(($errors + 1))
done

echo "TOTAL ERRORS: $errors"
exit $errors
