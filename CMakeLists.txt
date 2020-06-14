cmake_minimum_required(VERSION 3.1)
project(csugar CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(ExternalProject)
set(MINISAT_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/minisat)

ExternalProject_Add(minisat_project
	GIT_REPOSITORY https://github.com/semiexp/minisat
	GIT_TAG custom_constraints
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${MINISAT_INSTALL_LOCATION} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
)
add_library(minisat_lib STATIC IMPORTED)
set_property(TARGET minisat_lib PROPERTY IMPORTED_LOCATION ${MINISAT_INSTALL_LOCATION}/lib/libminisat.a)

add_subdirectory(src)
add_subdirectory(test)
