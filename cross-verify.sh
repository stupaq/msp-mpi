#!/usr/bin/env bash
set -e

function abort() {
  echo "ERROR: ${1:-aborting due to unexpected error}"
  exit 1
}

REGEX='Input: \(([0-9]*),([0-9]*),([0-9]*)\) Solution: \|\(([0-9]*),([0-9]*)\),\(([0-9]*),([0-9]*)\)\|=([.,0-9]*) Time: ([.,0-9]*)'

if [[ $# -lt 1 ]]; then
  echo "Missing output file."
  exit 1
fi

[[ `cat "$1"` =~ $REGEX ]] || abort "cannot match output"
M=${BASH_REMATCH[1]}
N=${BASH_REMATCH[2]}
S=${BASH_REMATCH[3]}
sum=${BASH_REMATCH[8]}

# Clean BASH_REMATCH just in case
[[ 0 =~ 0 ]]

[[ `./msp-seq-naive.exe $M $N $S 2>&1` =~ $REGEX ]] || abort "cannot match reference"
[[ $M -eq ${BASH_REMATCH[1]} ]] || abort
[[ $N -eq ${BASH_REMATCH[2]} ]] || abort
[[ $S -eq ${BASH_REMATCH[3]} ]] || abort
ref=${BASH_REMATCH[8]}

if [[ $sum -ne $ref ]]; then
  echo "ERROR: input: ($M,$N,$S) result: $sum expected: $ref"
  exit 1
fi

