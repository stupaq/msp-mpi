#!/usr/bin/env bash

for p in 4 8 16 32; do
  for m in 1 3 10000000; do
    for n in 1 3 1000000; do
      if [[ $m -lt 10000 || $n -lt 10000 ]]; then
        for s in 2347 4412343 9247 2435 4234; do
          ./submit-par.sh -m $m -n $n -s $s -p $p -A -s
        done
      fi
    done
  done
done

