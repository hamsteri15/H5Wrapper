#pragma once

#include <mpi.h>

namespace H5Wrapper{


static inline bool is_parallel() {

    int MPI_is_initialized;
    MPI_Initialized(&MPI_is_initialized);

    if (bool(MPI_is_initialized)){
        return true;
    }

    return false;

}




}