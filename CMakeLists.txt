cmake_minimum_required(VERSION 3.1)
project(csugar CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (USE_EMSCRIPTEN)
	set(CMAKE_CXX_COMPILER em++)
	set(CMAKE_CXX_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s MODULARIZE=1 -s SINGLE_FILE=1 -s ENVIRONMENT=web,worker -s FILESYSTEM=0 --memory-init-file 0")
endif()

include(ExternalProject)
set(MINISAT_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/minisat)

# minisat / glucose
if (USE_GLUCOSE)
  ExternalProject_Add(minisat_project
      GIT_REPOSITORY https://github.com/semiexp/glucose
      GIT_TAG 14401c6fa45687e0b68b1be1325696a305bb89c1
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${MINISAT_INSTALL_LOCATION} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  )
  add_library(minisat_lib STATIC IMPORTED)
  add_dependencies(minisat_lib minisat_project)
  set_property(TARGET minisat_lib PROPERTY IMPORTED_LOCATION ${MINISAT_INSTALL_LOCATION}/lib/libglucose.a)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_GLUCOSE")
else()
  ExternalProject_Add(minisat_project
      GIT_REPOSITORY https://github.com/semiexp/minisat
      GIT_TAG custom_constraints
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${MINISAT_INSTALL_LOCATION} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  )
  add_library(minisat_lib STATIC IMPORTED)
  add_dependencies(minisat_lib minisat_project)
  set_property(TARGET minisat_lib PROPERTY IMPORTED_LOCATION ${MINISAT_INSTALL_LOCATION}/lib/libminisat.a)
endif()

# pybind11
if (USE_EMSCRIPTEN)
else()
  set(PYBIND_LOCATION ${CMAKE_BINARY_DIR}/pybind11)
  find_package(Git)
  execute_process(COMMAND
    ${GIT_EXECUTABLE} clone --depth=1 -b v2.13.6 https://github.com/pybind/pybind11.git ${PYBIND_LOCATION}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  add_subdirectory(${PYBIND_LOCATION})
endif()

# csugar_lib
file(GLOB_RECURSE MAIN ${PROJECT_SOURCE_DIR}/src/common/*.cpp ${PROJECT_SOURCE_DIR}/src/conv/*.cpp ${PROJECT_SOURCE_DIR}/src/csp/*.cpp ${PROJECT_SOURCE_DIR}/src/icsp/*.cpp ${PROJECT_SOURCE_DIR}/src/sat/*.cpp ${PROJECT_SOURCE_DIR}/src/integrated/*.cpp)
add_library(csugar_lib STATIC ${MAIN})
target_include_directories(csugar_lib PUBLIC ${PROJECT_SOURCE_DIR}/include ${MINISAT_INSTALL_LOCATION}/include)
add_dependencies(csugar_lib minisat_lib)
target_link_libraries(csugar_lib minisat_lib)
set_target_properties(csugar_lib PROPERTIES OUTPUT_NAME "csugar")

# csugar executable
if (USE_EMSCRIPTEN)
  add_executable(csugar src/main_embind.cpp)
  set_target_properties(csugar PROPERTIES COMPILE_OPTIONS --bind)
  set_target_properties(csugar PROPERTIES LINK_FLAGS --bind)
else()
  add_executable(csugar src/main.cpp)
endif()

target_include_directories(csugar PUBLIC ${PROJECT_SOURCE_DIR}/include ${MINISAT_INSTALL_LOCATION}/include)
add_dependencies(csugar csugar_lib)
target_link_libraries(csugar csugar_lib)

# csugar python bindings
if (USE_EMSCRIPTEN)
else()
  pybind11_add_module(pycsugar ${PROJECT_SOURCE_DIR}/src/pybind.cpp ${PROJECT_SOURCE_DIR}/src/main.cpp)
  target_include_directories(pycsugar PUBLIC ${PROJECT_SOURCE_DIR}/include ${MINISAT_INSTALL_LOCATION}/include)
  add_dependencies(pycsugar csugar_lib)
  target_link_libraries(pycsugar PRIVATE csugar_lib)
endif()
