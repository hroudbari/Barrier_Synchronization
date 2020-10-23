CC       = mpic++              # default is CC = cc
CPPFLAGS = -g -Wall         # default is CFLAGS = [blank]
LDFLAGS  =                  # default is LDFLAGS = [blank]
MPI_NODES = 8

mpi_MCS_barrier: mpi_MCS_barrier.cpp  mpi_MCS_barrier.h
		 $(CC) $(CPPFLAGS) mpi_MCS_barrier.cpp -o mpi_MCS_barrier
		 mpirun -np ${MPI_NODES} ./mpi_MCS_barrier

mpi_dissemination_barrier: mpi_dissemination_barrier.cpp  mpi_dissemination_barrier.h
		 $(CC) $(CPPFLAGS) mpi_dissemination_barrier.cpp -o mpi_dissemination_barrier
		 mpirun -np ${MPI_NODES} ./mpi_dissemination_barrier

clean:
	 rm *.csv mpi_MCS_barrier mpi_dissemination_barrier

