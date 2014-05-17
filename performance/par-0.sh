#!/usr/bin/env bash

./submit-par.sh -m 4000 -n 4000 -s 123 -Q -A -s
./perf-report.sh *.err
cat report.csv
