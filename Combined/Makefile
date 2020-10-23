CC       = mpic++              # default is CC = cc
CPPFLAGS = -g -Wall -fopenmp   # default is CFLAGS = [blank]
LDFLAGS  =                     # default is LDFLAGS = [blank]
MPI_NODES = 8
OMP_THREADS=8

combined_barrier: combined_barrier.cpp  combined_barrier.h
		 $(CC) $(CPPFLAGS) combined_barrier.cpp -o combined_barrier
		 mpirun -np ${MPI_NODES} ./combined_barrier ${OMP_THREADS}

clean:
	 rm *.csv combined_barrier combined_barrier
