#!/usr/bin/env bash

COMPARE="`dirname $0`/../compare-seq.sh"

RANDOM=123123
COUNT=1000
MAX_M=50
MAX_N=50

errors=0
for i in `seq $COUNT`; do
  echo -ne "kadane\t"
  $COMPARE naive kadane "($(($RANDOM % $MAX_M + 1)),$(($RANDOM % $MAX_N + 1)),$RANDOM)"
  echo -ne "takaoka\t"
  $COMPARE naive takaoka "($(($RANDOM % $MAX_M + 1)),$(($RANDOM % $MAX_N + 1)),$RANDOM)"
  if [[ $? -ne 0 ]]; then
    errors=$(($errors + 1))
  fi
done
