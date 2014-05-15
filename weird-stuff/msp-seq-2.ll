# @ job_name = MSP-kadane_mm305678_1000x1000
# @ account_no = G52-5
# @ class = kdm
# @ error = MSP-kadane_mm305678_1000x1000.err
# @ output = MSP-kadane_mm305678_1000x1000.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = mm305678@students.mimuw.edu.pl
# @ job_type = bluegene
# @ bg_size = 1
# @ queue
echo "Started at" `date`
./msp-seq-kadane.exe 1000 1000 123
echo "Finished at" `date`
