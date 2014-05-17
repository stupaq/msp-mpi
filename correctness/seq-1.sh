#!/usr/bin/env bash

SEED=$(date +%s)
COUNT=${COUNT:-1000}
MAX_M=${MAX_M:-100}
MAX_N=${MAX_N:-100}

source "`dirname $0`/seq-0.sh"
