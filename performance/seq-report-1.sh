#!/usr/bin/env bash

for m in 1000 4000 8000; do
  for n in 1000 4000 8000; do
    for s in 2347 4412343 9247 2435 4234; do
      ./run-seq.sh -m $m -n $n -s $s -v kadane
    done
  done
done

for m in 1000 4000; do
  for n in 1000 4000; do
    for s in 2347 4412343 9247 2435 4234; do
      if [[ $m -lt 4000 || $n -lt 4000 ]]; then
        ./run-seq.sh -m $m -n $n -s $s -v takaoka
      fi
    done
  done
done

