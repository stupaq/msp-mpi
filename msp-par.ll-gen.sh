#!/usr/bin/env bash

M=${M:?}
N=${N:?}
S=${S:?}
Q=${Q:-kdm}
P=${P:?}
C=${C:?}

case $C in
  1) mode=SMP ;;
  2) mode=DUAL ;;
  4) mode=VN ;;
esac

job_name="MSP_${M}x${N}_S-${S}_P-${P}_C-${C}"
bg_size=$((($P + $C - 1) / $C))

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
mpirun -exe msp-par.exe -np ${P} -mode ${mode} -args "${M} ${N} ${S}"
echo "Finished at" \`date\`
EOF

