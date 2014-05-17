#!/usr/bin/env bash

for m in 50 100 200 400; do
  for n in 50 100 200 400; do
    for s in 2347 4412343 9247 2435 4234; do
      ./submit-seq.sh -m $m -n $n -s $s -v kadane -A -s
    done
  done
done

for m in 50 100 200 400; do
  for n in 50 100 200 400; do
    for s in 2347 4412343 9247 2435 4234; do
      ./submit-seq.sh -m $m -n $n -s $s -v takaoka -A -s
    done
  done
done

