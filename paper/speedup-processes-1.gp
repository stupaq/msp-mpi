#!/usr/bin/env gnuplot -p

set palette color
set linestyle 1 linetype rgb "red"
set linestyle 2 linetype rgb "blue" linewidth 1

set terminal pdf enhanced font 'Helvetica,14' size 17cm,15cm
set output 'speedup-processes-1.pdf'

set size 1,1
set origin 0,0
set multiplot layout 2,2
set key left top

id(x)=x

set title "M = 4000, N = 4000, mode = VN"
set xlabel "Number of processors"
set ylabel "Speed-up with respect to our sequential code"
set xrange [0:70]
plot id(x) linestyle 2 title "perfect speed-up", "< grep -e '^4000 4000' speedup-processes-1.dat" using 3:4 with linespoints linestyle 1 title "measured speed-up"

set title "M = 1, N = 30000000, mode = SMP"
set xlabel "Number of processors"
set ylabel "Speed-up with respect to our sequential code"
set xrange [0:18]
plot id(x) linestyle 2 title "perfect speed-up", "< grep -e '^1 30000000' speedup-processes-1.dat" using 3:4 with linespoints linestyle 1 title "measured speed-up"

set title "M = 1000, N = 1000, mode = VN"
set xlabel "Number of processors"
set ylabel "Speed-up with respect to our sequential code"
set xrange [0:70]
plot id(x) linestyle 2 title "perfect speed-up", "< grep -e '^1000 1000' speedup-processes-1.dat" using 3:4 with linespoints linestyle 1 title "measured speed-up"

set title "M = 1, N = 10000000, mode = SMP"
set xlabel "Number of processors"
set ylabel "Speed-up with respect to our sequential code"
set xrange [0:18]
plot id(x) linestyle 2 title "perfect speed-up", "< grep -e '^1 10000000' speedup-processes-1.dat" using 3:4 with linespoints linestyle 1 title "measured speed-up"

unset multiplot

# EOF
