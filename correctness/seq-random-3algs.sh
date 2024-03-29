#!/usr/bin/env bash

# Takes ~20 min on students with default values
SEED=${SEED:-123123}
COUNT=${COUNT:-10000}
MAX_M=${MAX_M:-50}
MAX_N=${MAX_N:-50}

RANDOM=$SEED
errors=0
for i in `seq $COUNT`; do
  M=$(($RANDOM % $MAX_M + 1))
  N=$(($RANDOM % $MAX_N + 1))
  S=$(($RANDOM + 1))
  echo -ne "kadane\t"
  ./compare-seq.sh naive kadane "($M,$N,$S)"
  [[ $? -eq 0 ]] || errors=$(($errors + 1))
  echo -ne "takaoka\t"
  ./compare-seq.sh kadane takaoka "($M,$N,$S)"
  [[ $? -eq 0 ]] || errors=$(($errors + 1))
done

echo "TOTAL ERRORS: $errors FOR RNG SEED: $SEED"
exit $errors
