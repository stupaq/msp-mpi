#!/usr/bin/env bash

M=${M:-1000}
N=${N:-1000}
S=${S:-123}
Q=${Q:-kdm}
V=${V:-naive}

cat <<EOF
# @ job_name = MSP-${V}_`whoami`_${M}x${N}
# @ account_no = G52-5
# @ class = ${Q}
# @ error = MSP-${V}_`whoami`_${M}x${N}.err
# @ output = MSP-${V}_`whoami`_${M}x${N}.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = \$(user)@icm.edu.pl
# @ job_type = bluegene
# @ bg_size = 1
# @ queue
echo "Started at" \`date\`
mpirun -exe msp-seq-${V}.exe -np 1 -mode SMP -args "${M} ${N} ${S}"
echo "Finished at" \`date\`
EOF
