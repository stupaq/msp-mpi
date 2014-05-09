#!/usr/bin/env bash

RESULT_LINE_REGEX='Input: \(([0-9]*),([0-9]*),([0-9]*)\) Solution: \|\(([0-9]*),([0-9]*)\),\(([0-9]*),([0-9]*)\)\|=([.,0-9]*) Time: ([.,0-9]*)'

function abort() {
  echo "ERROR: ${1:-aborting due to unexpected error}"
  exit 1
}

