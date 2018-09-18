cmake_minimum_required(VERSION 2.8.9)

project(Work-Stealing)
 
#Bring the headers into the project
#include_directories(include)

set (CMAKE_C_STANDARD 99)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

if (MPI)
find_package(MPI QUIET)
  if (NOT MPI_FOUND)
    message (FATAL_ERROR "MPI must exist to compile Work-stealing.")
  else ()
    message (STATUS "MPI found & enabled.")
  endif ()


    add_executable(ws_test_task ws_test_task.c)
    target_include_directories (ws_test_task PUBLIC ${MPI_C_INCLUDE_PATH} )
    target_compile_options (ws_test_task PUBLIC ${MPI_C_COMPILE_FLAGS} )
    target_link_libraries (ws_test_task ${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS} m)
    install (TARGETS ws_test_task DESTINATION bin)

endif()