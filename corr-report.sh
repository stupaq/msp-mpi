#!/usr/bin/env bash
source "`dirname $0`/commons.sh"

REFERENCE_EXE=${REFERENCE_EXE:-./msp-seq-naive.exe}

if [[ $# -lt 1 ]]; then
  echo "Missing input files."
  exit 1
fi

errors=0
for f in "$@"; do
  [[ `cat "$f"` =~ $RESULT_LINE_REGEX ]] || abort "cannot match output"
  M=${BASH_REMATCH[1]}
  N=${BASH_REMATCH[2]}
  S=${BASH_REMATCH[3]}
  sum=${BASH_REMATCH[8]}

  # Clean BASH_REMATCH just in case
  [[ 0 =~ 0 ]]

  [[ `$REFERENCE_EXE $M $N $S 2>&1` =~ $RESULT_LINE_REGEX ]] || abort "cannot match reference"
  [[ $M -eq ${BASH_REMATCH[1]} ]] || abort
  [[ $N -eq ${BASH_REMATCH[2]} ]] || abort
  [[ $S -eq ${BASH_REMATCH[3]} ]] || abort
  ref=${BASH_REMATCH[8]}

  if [[ $sum -ne $ref ]]; then
    echo -e "BAD\tinput: ($M,$N,$S) result: $sum expected: $ref"
    errors=$(($errors + 1))
  else
    echo -e "OK\tinput: ($M,$N,$S)"
  fi
done

exit $errors
