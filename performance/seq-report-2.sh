#!/usr/bin/env bash

for m in 1000 4000; do
  for n in 1000 4000; do
    for s in 2347 4412343 9247 2435 4234; do
      ./submit-seq.sh -m $m -n $n -s $s -v kadane -a -A -s
    done
  done
done

for m in 1000 4000; do
  for n in 1000 4000; do
    for s in 2347 4412343 9247 2435 4234; do
      if [[ $m -lt 4000 || $n -lt 4000 ]]; then
        ./submit-seq.sh -m $m -n $n -s $s -v takaoka -a -A -s
      fi
    done
  done
done

