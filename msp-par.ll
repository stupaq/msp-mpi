# @ job_name = MSP_mm305678_1000x1000
# @ account_no = G52-5
# @ class = kdm
# @ error = MSP_mm305678_1000x1000.err
# @ output = MSP_mm305678_1000x1000.out
# @ environment = COPY_ALL
# @ wall_clock_limit = 00:15:00
# @ notification = error
# @ notify_user = $(user)@icm.edu.pl
# @ job_type = bluegene
# @ bg_size = 8
# @ queue
echo "Started at" `date`
mpirun -exe msp-par.exe -np 32 -mode VN -args "1000 1000 123"
echo "Finished at" `date`
