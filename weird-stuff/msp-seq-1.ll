# @ job_name = MSP-kadane_mm305678_1000x1000
# @ account_no = G52-5
# @ class = kdm
# @ error = MSP-kadane_mm305678_1000x1000.err
# @ output = MSP-kadane_mm305678_1000x1000.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = $(user)@icm.edu.pl
# @ job_type = bluegene
# @ bg_size = 1
# @ queue
echo "Started at" `date`
mpirun -exe msp-seq-kadane.exe -mode SMP -np 1 -args "1000 1000 123"
echo "Finished at" `date`
