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
add_dependencies(minisat_lib minisat_project)
set_property(TARGET minisat_lib PROPERTY IMPORTED_LOCATION ${MINISAT_INSTALL_LOCATION}/lib/libminisat.a)

# csugar_lib
file(GLOB_RECURSE MAIN ${PROJECT_SOURCE_DIR}/src/common/*.cpp ${PROJECT_SOURCE_DIR}/src/conv/*.cpp ${PROJECT_SOURCE_DIR}/src/csp/*.cpp ${PROJECT_SOURCE_DIR}/src/icsp/*.cpp ${PROJECT_SOURCE_DIR}/src/sat/*.cpp)
add_library(csugar_lib STATIC ${MAIN})
target_include_directories(csugar_lib PUBLIC ${PROJECT_SOURCE_DIR}/include ${MINISAT_INSTALL_LOCATION}/include)
add_dependencies(csugar_lib minisat_lib)
target_link_libraries(csugar_lib minisat_lib)
set_target_properties(csugar_lib PROPERTIES OUTPUT_NAME "csugar")

# csugar executable
add_executable(csugar src/main.cpp)
target_include_directories(csugar PUBLIC ${PROJECT_SOURCE_DIR}/include ${MINISAT_INSTALL_LOCATION}/include)
add_dependencies(csugar csugar_lib)
target_link_libraries(csugar csugar_lib)

# add_subdirectory(test)
