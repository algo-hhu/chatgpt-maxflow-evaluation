cmake_minimum_required(VERSION 3.13)

# Set up the project
project("maxflow_algos" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Options for the project
option(maxflow_algos_build_bench "Build benchmark program" ON)
option(maxflow_algos_gridcut_available "GridCut implementation is available" OFF)
option(maxflow_algos_vtune_instrumentation "Enable Intel VTune instrumentation" OFF)
option(maxflow_algos_link_pard "Link P-ARD implementation" OFF)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build: Debug, Release, RelWithDebInfo, MinSizeRel.")
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

# Find TBB package
find_package(TBB CONFIG REQUIRED)
if(TBB_FOUND)
    message(STATUS "TBB library found.")
    include_directories(${TBB_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "TBB library not found.")  # Corrected error message format
endif()

# VTune instrumentation
if(maxflow_algos_vtune_instrumentation AND NOT DEFINED ENV{VTUNE_PROFILER_2020_DIR})
    message(FATAL_ERROR "Environment variable VTUNE_PROFILER_2020_DIR not found")
endif()

# Subdirectories
add_subdirectory(reimpls)
add_subdirectory(bk)
add_subdirectory(snappy)
add_subdirectory(hi_pr)
add_subdirectory(sppr)
if(maxflow_algos_link_pard)
    add_subdirectory(prd)
endif()
if(maxflow_algos_gridcut_available)
    add_subdirectory(grid_cut)
endif()

# Define executables and link libraries
add_executable(demo "demo.cpp" "graph_io.cpp")
add_executable(bench_io "bench_io.cpp" "graph_io.cpp")

if(maxflow_algos_build_bench)
    add_executable(bench "bench.cpp" "graph_io.cpp")
    target_compile_options(bench PRIVATE $<$<BOOL:${WIN32}>:/debug /Z7 /bigobj> $<$<NOT:$<BOOL:${WIN32}>>:-g -O3 -fopenmp -fpermissive>)
    target_link_options(bench PRIVATE $<$<BOOL:${WIN32}>:/profile> $<$<NOT:$<BOOL:${WIN32}>>:-g -O3 -lpthread -fopenmp>)
    target_link_libraries(bench PRIVATE TBB::tbb snappy reimpls bk hi_pr sppr)
    if(maxflow_algos_gridcut_available)
        target_compile_definitions(bench PRIVATE GRIDCUT_IS_AVAILABLE)
        target_link_libraries(bench PRIVATE grid_cut)
    endif()
    if(maxflow_algos_link_pard)
        target_compile_definitions(bench PRIVATE PARD_IS_AVAILABLE)
        target_link_libraries(bench PRIVATE d_maxflow_prd)
    endif()
endif()

foreach(EXE demo bench_io)
    target_compile_options(${EXE} PRIVATE $<$<BOOL:${WIN32}>:/debug /Z7 /openmp> $<$<NOT:$<BOOL:${WIN32}>>:-g -O3 -fopenmp -fpermissive>)
    target_link_options(${EXE} PRIVATE $<$<BOOL:${WIN32}>:/profile> $<$<NOT:$<BOOL:${WIN32}>>:-g -O3 -lpthread -fopenmp>)
    target_link_libraries(${EXE} PRIVATE TBB::tbb snappy hi_pr reimpls bk sppr)
    if(maxflow_algos_vtune_instrumentation)
        target_include_directories(${EXE} PRIVATE "${VTUNE_PROFILER_2020_DIR}/include")
        target_link_directories(${EXE} PRIVATE "${VTUNE_PROFILER_2020_DIR}/lib64")
        target_link_libraries(${EXE} PRIVATE libittnotify)
        target_compile_definitions(${EXE} PRIVATE REIMPLS_ENABLE_VTUNE)
    else()
        target_compile_definitions(${EXE} PRIVATE INTEL_NO_ITTNOTIFY_API)
    endif()
    if(maxflow_algos_gridcut_available)
        target_compile_definitions(${EXE} PRIVATE GRIDCUT_IS_AVAILABLE)
        target_link_libraries(${EXE} PRIVATE grid_cut)
    endif()
endforeach()

