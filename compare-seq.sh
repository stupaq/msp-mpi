#!/usr/bin/env bash
source "`dirname $0`/commons.sh"

if [[ $# -lt 2 ]]; then
  echo "Must provide two binaries for cross-verification."
  exit 1
fi

BIN1="${1}"
if [[ ! -x "$BIN1" ]]; then
  BIN1="./msp-seq-$BIN1.exe"
fi
BIN2="${2}"
if [[ ! -x "$BIN2" ]]; then
  BIN2="./msp-seq-$BIN2.exe"
fi

shift 2

errors=0
for t in "$@"; do
  [[ $t =~ $TEST_CASE_REGEX ]] || abort "cannot match test case"
  M=${BASH_REMATCH[1]}
  N=${BASH_REMATCH[2]}
  S=${BASH_REMATCH[3]}

  [[ `$BIN1 $M $N $S 2>&1` =~ $RESULT_LINE_REGEX ]] || abort "cannot match output 1 for ($M,$N,$S)"
  [[ $M -eq ${BASH_REMATCH[1]} ]] || abort
  [[ $N -eq ${BASH_REMATCH[2]} ]] || abort
  [[ $S -eq ${BASH_REMATCH[3]} ]] || abort
  sum1=${BASH_REMATCH[8]}
  time1=${BASH_REMATCH[9]}

  # Clean BASH_REMATCH just in case
  [[ 0 =~ 0 ]]

  [[ `$BIN2 $M $N $S 2>&1` =~ $RESULT_LINE_REGEX ]] || abort "cannot match output 2 for ($M,$N,$S)"
  [[ $M -eq ${BASH_REMATCH[1]} ]] || abort
  [[ $N -eq ${BASH_REMATCH[2]} ]] || abort
  [[ $S -eq ${BASH_REMATCH[3]} ]] || abort
  sum2=${BASH_REMATCH[8]}
  time2=${BASH_REMATCH[9]}

  if [[ $sum -ne $ref ]]; then
    echo -e "BAD\tinput: ($M,$N,$S) result: $sum expected: $ref"
    errors=$(($errors + 1))
  else
    echo -e "OK\tinput: ($M,$N,$S) times: $time1 | $time2 (`echo $time1 - $time2 | bc`) "
  fi
done

exit $errors
