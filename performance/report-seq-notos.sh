#!/usr/bin/env bash

for s in 2347 2435 4234 4412343 9247; do
  for m in 100 200 400; do
    for n in 100 200 400; do
      ./submit-seq.sh -m $m -n $n -s $s -v kadane -A -s -M reports/
      ./submit-seq.sh -m $m -n $n -s $s -v takaoka -A -s -M reports/
    done
  done
done

