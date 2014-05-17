#!/usr/bin/env bash

./submit-par.sh -m 43 -n 27 -s 21398 -p 4 -Q -A -s
./corr-report.sh *.err
