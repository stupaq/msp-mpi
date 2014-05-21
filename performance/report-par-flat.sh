#!/usr/bin/env bash

for s in 2347 2435 4234 4412343 9247; do
  for w in 64 32 16 8 4 2 1; do
    for c in 4 2 1; do
      if [[ $(($w * $c)) -le 64 ]]; then
        for m in 10000000 20000000 30000000; do
          ./submit-par.sh -m $m -n 1 -s $s -p $(($w * $c)) -c $c -A -s -M reports/
        done

        for n in 10000000 20000000 30000000; do
          ./submit-par.sh -m 1 -n $n -s $s -p $(($w * $c)) -c $c -A -s -M reports/
        done
      fi
    done
  done
done

