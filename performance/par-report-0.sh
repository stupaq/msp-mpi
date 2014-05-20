#!/usr/bin/env bash

for p in 32 24 16 8 4; do
  for m in 250 1000 4000 8000; do
    for n in 250 1000 4000 8000; do
      if [[ $p -ge 16 || ( $m -le 4000 && $n -le 4000 ) ]]; then
        for s in 2347 4412343 9247 2435 4234; do
          ./submit-par.sh -m $m -n $n -s $s -p $p -A -s -M reports/
        done
      fi
    done
  done
done

