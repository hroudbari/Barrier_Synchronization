CC       = g++              # default is CC = cc
CPPFLAGS = -g -Wall -fopenmp   # default is CFLAGS = [blank]
LDFLAGS  =                     # default is LDFLAGS = [blank]
OMP_THREADS = 8

omp_MCS_barrier: omp_MCS_barrier.cpp  omp_MCS_barrier.h
		 $(CC) $(CPPFLAGS) omp_MCS_barrier.cpp -o omp_MCS_barrier
		./omp_MCS_barrier $(OMP_THREADS)

omp_sense-reverse_barrier: omp_sense-reverse_barrier.cpp
		 $(CC) $(CPPFLAGS) omp_sense-reverse_barrier.cpp -o omp_sense-reverse_barrier
		./omp_sense-reverse_barrier $(OMP_THREADS)

clean:
	 rm *.csv omp_sense-reverse_barrier omp_MCS_barrier

