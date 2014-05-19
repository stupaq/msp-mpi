#!/usr/bin/env bash

M=${M:?}
N=${N:?}
S=${S:?}
Q=${Q:-kdm}
V=${V:?}

job_name="MSP-${V}_${M}x${N}_S-${S}_seq_fast-${A:-no}"
bg_size=1

cat <<EOF
# @ job_name = ${job_name}
# @ account_no = G52-5
# @ class = ${Q}
# @ error = ${job_name}.err
# @ output = ${job_name}.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = \$(user)@icm.edu.pl
# @ job_type = bluegene
# @ bg_size = ${bg_size}
# @ queue
echo "Started at" \`date\`
EOF

if [[ $A = 'yes' ]]; then
  echo "./msp-seq-${V}.exe ${M} ${N} ${S}"
else
  echo "mpirun -exe msp-seq-${V}.exe -np 1 -mode SMP -args \"${M} ${N} ${S}\""
fi

cat <<EOF
echo "Finished at" \`date\`
EOF
