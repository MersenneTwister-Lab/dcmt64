#pragma once
#ifndef MPICONTROL_HPP
#define MPICONTROL_HPP

#if defined(NO_MPI)
#define MPI_COMM_WORLD 10
#define MPI_UNDEFINED -1
#define MPI_INT 0
#define MPI_DOUBLE 1

#define MPI_Init(a, b) (void)a, (void)b
#define MPI_Comm_rank(a, b) (void)a, (void)b
#define MPI_Comm_size(a, b) (void)a, (void)b
#define MPI_Finalize()
#define MPI_Abort(a, b) (void)a, (void)b
#define MPI_Comm_split(a, b, c, d) (void)a, (void)b, (void)c, (void)d
#define MPI_Bcast(a, b, c, d, e) (void)a, (void)b, (void)c, (void)d, \
        (void)e
#define MPI_Send(a, b, c, d, e, f) (void)a, (void)b, (void)c, (void)d, \
        (void)e, (void)f
#define MPI_Recv(a, b, c, d, e, f, g) (void)a, (void)b, (void)c, (void)d, \
        (void)e, (void)f, (void)g
#define MPI_Allgather(a, b, c, d, e, f, g) (void)a, (void)b, (void)c, (void)d, \
        (void)e, (void)f, (void)g
#define MPI_Comm_free(a) (void)a
#define MPI_Comm int
struct MPI_Status {
    int MPI_SOURCE;
};

#else
#include <mpi.h>
#endif

class MPIControl {
public:
    MPIControl(int *argc, char** argv[]) {
        world_rank = 0;
        world_num_process = 1;
        MPI_Init(argc, argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_num_process);
    }

    ~MPIControl() {
        MPI_Finalize();
    }

    void abort() {
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    int getRank() {
        return world_rank;
    }

    int getNumP() {
        return world_num_process;
    }

private:
    int world_rank;
    int world_num_process;
};

#endif // MPICONTROL_HPP
