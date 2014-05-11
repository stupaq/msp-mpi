# @ job_name = MSP_mm305678_4000x4000
# @ account_no = G52-5
# @ class = kdm
# @ error = MSP_mm305678_4000x4000.err
# @ output = MSP_mm305678_4000x4000.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = mm305678@students.mimuw.edu.pl
# @ job_type = bluegene
# @ bg_size = 1
# @ queue
echo "Started at" `date`
./msp-seq-kadane.exe 4000 4000 123
echo "Finished at" `date`
