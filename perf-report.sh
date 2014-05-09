#!/usr/bin/env bash
source "`dirname $0`/commons.sh"
REPORT_FILE=report.csv

if [[ $# -lt 1 ]]; then
  echo "Missing input files."
  exit 1
fi

for f in "$@"; do
  [[ `cat "$f"` =~ $RESULT_LINE_REGEX ]] || abort "cannot match output"
  M=${BASH_REMATCH[1]}
  N=${BASH_REMATCH[2]}
  S=${BASH_REMATCH[3]}
  secs=${BASH_REMATCH[9]}
  echo "$M,$N,$secs,$S"
done > $REPORT_FILE

