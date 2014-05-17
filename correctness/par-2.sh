#!/usr/bin/env bash

SEED=${SEED:-123123}
COUNT=${COUNT:-1000}
MAX_M=${MAX_M:-100}
MAX_N=${MAX_N:-100}
MIN_P=${MIN_P:-1}

RANDOM=$SEED
for i in `seq $COUNT`; do
  m=$(($RANDOM % $MAX_M + 1))
  n=$(($RANDOM % $MAX_N + 1))
  s=$(($RANDOM + 1))
  p=$(($RANDOM % 32 + MIN_P))
  if [[ $(($i % 32)) -eq 0 || $i -eq $COUNT ]]; then
    extra='-A -s'
  else
    extra=
  fi
  ./submit-par.sh -m $m -n $n -s $s -p $p $extra
done

./corr-report.sh *.err
