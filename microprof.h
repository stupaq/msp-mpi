/** Copyright (C) Mateusz Machalica, 2014. */

#ifndef MICROPROF_H_
#define MICROPROF_H_

#ifndef MICROPROF_IS_PROFILED
#define MICROPROF_IS_PROFILED true
#endif

#define MICROPROF_STREAM stdout

#ifdef MICROPROF_ENABLE
#define MICROPROF_START(name)                                     \
  double name ## _start = MPI_Wtime();
#define MICROPROF_END(name)                                       \
  double name ## _end = MPI_Wtime();                              \
  if (MICROPROF_IS_PROFILED)                                      \
    fprintf(MICROPROF_STREAM, "PROFILING:\t" #name "\t%.10f\n",   \
        name ## _end - name ## _start)
#define MICROPROF_WARN(cond, warn)                                \
  if (cond) fprintf(MICROPROF_STREAM, "WARNING:\t%s\n", warn)
#define MICROPROF_INFO(...)                                       \
  fprintf(MICROPROF_STREAM, __VA_ARGS__);                         \
  fflush(MICROPROF_STREAM)
#else
#define MICROPROF_START(name)
#define MICROPROF_END(name)
#define MICROPROF_WARN(cond, warn)
#define MICROPROF_INFO(...)
#endif

#define SUPPRESS_UNUSED(x) ((void) (x))

#endif  // MICROPROF_H_
