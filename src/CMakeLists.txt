file(GLOB_RECURSE MAIN *.cpp)
add_library(csugar STATIC ${MAIN})

ExternalProject_Get_Property(minisat_project install_dir)

target_include_directories(csugar PUBLIC ${PROJECT_SOURCE_DIR}/include ${MINISAT_INSTALL_LOCATION}/include)
# add_dependencies(csugar minisat_lib)
target_link_libraries(csugar)
