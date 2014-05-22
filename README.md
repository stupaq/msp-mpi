Implementation of Maximum Subarray Problem algorithms in MPI
============================================================

Introduction
------------
Throughout the code and instrumentation tools we agree on the following conventions

* let M be the number of rows of input matrix
* let N be the number of columns of input matrix
* let S be the seed passed to matrix input library
* let P be the number of processes spawned for MPI computation
* let C be the mode of operation of MPI subsystem, where 1 = SMP, 2 = DUAL, 4 =
  VN, these are the only allowed values of C

Usage notes
-----------
There are many more scripts than I wish to cover here, the most important ones
being `./submit-seq.sh` and `./submit-par.sh` which submit execution of
specified sequential and parallel algorithm respectively on ICM's Notos
supercomputer BlueGene/P.
The scripts take parameters described in the introduction together with
appropriate values, as arguments.
Additionally `./submit-seq.sh` takes `-v <name>` obligatory parameter, which
determines which version of sequential algorithm should be used.

Notes on attached performance evaluation results
------------------------------------------------
Performance evaluation results are stored in flat textual form under `reports/`
directory, naming convention follows the one described in the introduction,
note that you can easily query reports by the file names using `ls` or `cat`
and bash globbing syntax.
Reports prefixed with `MSP_` were generated for the fastest parallel
implementation, these prefixed with `MSP-<name>` were collected for sequential
implementation of `<name>` version of the algorithm.
I strongly suggest you to rely on `reports/export-as-csv.sh` script and look
for CSV files with performance evaluation results under `exported/` directory.

Extras
------
We have implemented sequential version of Takaoka's algorithm using $O(n^2
\log{n})$ expected time APSP algorithm due Moffat and Takaoka.
See comments in `msp-seq-takaoka.c`, `minsum.h` and `ranking.h` for explanation
of the algorithm.

Copyright (c) 2014 Mateusz Machalica