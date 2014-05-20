#!/usr/bin/env bash

for s in 2347 2435 4234 4412343 9247; do
  ./run-seq.sh -m 1000 -n 1000 -s $s -v kadane -M reports/ &
  ./run-seq.sh -m 1000 -n 4000 -s $s -v kadane -M reports/ &
  ./run-seq.sh -m 1000 -n 8000 -s $s -v kadane -M reports/ &
  ./run-seq.sh -m 4000 -n 1000 -s $s -v kadane -M reports/ &
  wait
  ./run-seq.sh -m 4000 -n 4000 -s $s -v kadane -M reports/ &
  ./run-seq.sh -m 4000 -n 8000 -s $s -v kadane -M reports/ &
  ./run-seq.sh -m 8000 -n 1000 -s $s -v kadane -M reports/ &
  ./run-seq.sh -m 8000 -n 4000 -s $s -v kadane -M reports/ &
  wait
  ./run-seq.sh -m 8000 -n 8000 -s $s -v kadane -M reports/ &
  ./run-seq.sh -m 1000 -n 1000 -s $s -v takaoka -M reports/ &
  ./run-seq.sh -m 1000 -n 4000 -s $s -v takaoka -M reports/ &
  ./run-seq.sh -m 4000 -n 1000 -s $s -v takaoka -M reports/ &
  wait
  ./run-seq.sh -m 4000 -n 4000 -s $s -v takaoka -M reports/ &
  wait
done

