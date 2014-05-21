#!/usr/bin/env bash

src_dir="`dirname $0`"
dst_dir="`dirname $0`/../exported"

REPORT_FILE="$dst_dir/seq-kadane-notos.csv" \
  ./perf-report.sh $src_dir/MSP-kadane_*_seq_fast-*.err
REPORT_FILE="$dst_dir/seq-takaoka-notos.csv" \
  ./perf-report.sh $src_dir/MSP-takaoka_*_seq_fast-*.err
REPORT_FILE="$dst_dir/seq-kadane-labs.csv" \
  ./perf-report.sh $src_dir/MSP-kadane_*_H-*.err
REPORT_FILE="$dst_dir/seq-takaoka-labs.csv" \
  ./perf-report.sh $src_dir/MSP-takaoka_*_H-*.err

for c in 1 2 4; do
  for w in 1 2 4 8 16; do
    p=$(($w * $c))
    f="$src_dir/MSP_*_P-${p}_C-${c}.err"
    ls $f &>/dev/null
    if [[ $? -eq 0 ]]; then
      REPORT_FILE="$dst_dir/par-hybrid-P$p-C$c.csv" \
        ./perf-report.sh $f
    else
      echo "Skipping for P=$p C=$c"
    fi
  done
done

find "$dst_dir" -size 0 -print0 | xargs -0 rm -f

