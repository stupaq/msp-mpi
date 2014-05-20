#!/usr/bin/env bash

src_dir="`dirname $0`"
dst_dir="`dirname $0`/../exported"

REPORT_FILE="$dst_dir/seq-bluegene-kadane.csv" \
  ./perf-report.sh $src_dir/MSP-kadane_*_seq_fast-no.err
REPORT_FILE="$dst_dir/seq-bluegene-takaoka.csv" \
  ./perf-report.sh $src_dir/MSP-takaoka_*_seq_fast-no.err
REPORT_FILE="$dst_dir/seq-khaki-kadane.csv" \
  ./perf-report.sh $src_dir/MSP-kadane_*_seq_run.err
REPORT_FILE="$dst_dir/seq-khaki-takaoka.csv" \
  ./perf-report.sh $src_dir/MSP-takaoka_*_seq_run.err

for c in 1 2 4; do
  for p in 4 8 16 24 32; do
    filter="$src_dir/MSP_*_P-${p}_C-${c}.err"
    ls $filter &>/dev/null
    if [[ $? -eq 0 ]]; then
      echo "Collecting for P=$p C=$c"
      REPORT_FILE="$dst_dir/par-kadane-P$p-C$c.csv" \
        ./perf-report.sh $filter
    else
      echo "Skipping for P=$p C=$c"
    fi
  done
done

find "$dst_dir" -size 0 -print0 | xargs -0 rm -f

