#!/usr/bin/env bash

for s in 2347 2435 4234 4412343 9247; do
  for m in 1000 4000 8000; do
    for n in 1000 4000 8000; do
      ./run-seq.sh -m $m -n $n -s $s -v kadane -M reports/
    done
  done

  for m in 1000 4000; do
    for n in 1000 4000; do
      ./run-seq.sh -m $m -n $n -s $s -v takaoka -M reports/
    done
  done
done

