CMAKE_MINIMUM_REQUIRED(VERSION 3.12)


PROJECT(H5Wrapper)

message("COMPILER ID " ${CMAKE_CXX_COMPILER_ID})
#INCLUDE(cmake/StandardProjectSettings.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_17)


add_library(project_warnings INTERFACE)
include(CompilerWarnings.cmake)
set_project_warnings(project_warnings)

#Configure MPI
FIND_PACKAGE(MPI REQUIRED)
MESSAGE("STATUS MPI FOUND: ${MPI_CXX_FOUND}")

#Configure HDF5
SET(HDF5_PREFER_PARALLEL true)
FIND_PACKAGE(HDF5 REQUIRED)
MESSAGE(STATUS "Found HDF5: ${HDF5_FOUND}")
MESSAGE(STATUS "HDF5 is parallel: ${HDF5_IS_PARALLEL}")
MESSAGE(STATUS "Prefer parallel: ${HDF5_PREFER_PARALLEL}")


#option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable Test Builds" ON)

include_directories(include)

#Set library and binary paths
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



if(ENABLE_TESTING)
  enable_testing()
  message(
    "Building Tests."
  )
  add_subdirectory(test)
endif()

