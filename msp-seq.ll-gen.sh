#!/usr/bin/env bash

M=${M:-1000}
N=${N:-1000}
S=${S:-123}
V=${V:-naive}

cat <<EOF
# @ job_name = MSP-${V}_`whoami`_${M}x${N}
# @ account_no = G52-5
# @ class = kdm
# @ error = MSP-${V}_`whoami`_${M}x${N}.err
# @ output = MSP-${V}_`whoami`_${M}x${N}.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = `whoami`@students.mimuw.edu.pl
# @ job_type = bluegene
# @ bg_size = 1
# @ queue
echo "Started at" \`date\`
./msp-seq-${V}.exe ${M} ${N} ${S}
echo "Finished at" \`date\`
EOF
