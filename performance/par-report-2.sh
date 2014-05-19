#!/usr/bin/env bash

for p in 32 16 8 4; do
  for c in 1 2 4; do
    for m in 1 2 5000000 10000000 20000000; do
      for n in 1 2 5000000 10000000 20000000; do
        if [[ ( $m -lt 1000 && $n -gt 1000 ) || ( $m -gt 1000 && $n -lt 1000 ) ]]; then
          for s in 2347 4412343 9247 2435 4234; do
            ./submit-par.sh -m $m -n $n -s $s -p $p -c $c -A -s -M reports/
          done
        fi
      done
    done
  done
done

