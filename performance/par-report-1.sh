#!/usr/bin/env bash

for p in 4 8 16 32; do
  for c in 1 2 4; do
    for m in 50 1000 4000 8000; do
      for n in 50 1000 4000 8000; do
        if [[ $p -ge 16 || ( $m -le 1000 && $n -le 1000 ) ]]; then
          for s in 2347 4412343 9247 2435 4234; do
            ./submit-par.sh -m $m -n $n -s $s -p $p -c $c -A -s
          done
        fi
      done
    done
  done
done

