file(GLOB_RECURSE MAIN *.cpp)
add_library(csugar STATIC ${MAIN})
target_include_directories(csugar PUBLIC ${PROJECT_SOURCE_DIR}/include)
