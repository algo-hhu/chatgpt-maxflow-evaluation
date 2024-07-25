cmake_minimum_required(VERSION 3.13)

project(sppr)

# Find TBB
find_package(TBB REQUIRED)

add_library(sppr STATIC "maxFlow.cpp")
target_include_directories(sppr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link TBB
target_link_libraries(sppr PUBLIC TBB::tbb)

# Define this to indicate TBB availability
target_compile_definitions(sppr PRIVATE SPPR_HAS_TBB)

# Set OpenMP flags
if (WIN32)
    target_compile_options(sppr PRIVATE /openmp)
else()
    target_compile_options(sppr PRIVATE -fopenmp)
    target_link_options(sppr PRIVATE -lpthread -fopenmp)
endif()

