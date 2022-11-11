

# Build catch mpi main
add_library(catch_mpi_main STATIC catch_mpi_main.cpp)

target_include_directories(catch_mpi_main PUBLIC ${MPI_CXX_INCLUDE_DIRS})

#This is required to link against mpi for MPI_Init in main
target_link_libraries(catch_mpi_main PUBLIC ${MPI_CXX_LIBRARIES})

SET(TestSources
    test_h5_wrapper.cpp;
)

add_executable(TestH5Wrapper.bin ${TestSources})
target_include_directories(TestH5Wrapper.bin PUBLIC ${HDF5_INCLUDE_DIRS})
target_link_libraries(TestH5Wrapper.bin PUBLIC project_options catch_mpi_main ${HDF5_LIBRARIES})
target_compile_options(TestH5Wrapper.bin PRIVATE -DDEBUG)

#serial execution of mpi code
add_test( NAME H5WrapperMpiTest0 
          COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestH5Wrapper.bin)


add_test( NAME H5WrapperMpiTest1 
          COMMAND mpirun -np 1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestH5Wrapper.bin)

add_test( NAME H5WrapperMpiTest2 
          COMMAND mpirun -np 2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestH5Wrapper.bin
          )

add_test( NAME H5WrapperMpiTest4 
          COMMAND mpirun -np 4 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestH5Wrapper.bin
)   

