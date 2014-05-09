#!/usr/bin/env bash

M=${M:-1000}
N=${N:-1000}
P=${P:-32}
C=${C:-4}
S=${S:-123}

case $C in
  1) mode=SMP ;;
  2) mode=DUAL ;;
  4) mode=VN ;;
esac

cat <<EOF
# @ job_name = MSP_`whoami`_${M}x${N}
# @ account_no = G52-5
# @ class = kdm
# @ error = MSP_`whoami`_${M}x${N}.err
# @ output = MSP_`whoami`_${M}x${N}.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = `whoami`@students.mimuw.edu.pl
# @ job_type = bluegene
# @ bg_size = `echo $(( ($P + $C - 1) / $C ))`
# @ queue
echo "Started at" \`date\`
mpirun -np 32 -mode ${mode} msp-par.exe ${M} ${N} ${S}
echo "Finished at" \`date\`
EOF
