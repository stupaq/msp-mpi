#!/usr/bin/env bash

for s in 2347 2435 4234 4412343 9247; do
  for w in 64 32 16 8 4 2 1; do
    for c in 4 2 1; do
      if [[ $(($w * $c)) -le 64 ]]; then
        for m in 250 400 1000 4000; do
          for n in 250 400 1000 4000; do
            ./submit-par.sh -m $m -n $n -s $s -p $(($w * $c)) -c $c -A -s -M reports/
          done
        done
      fi
    done
  done
done

