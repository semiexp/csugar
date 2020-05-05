file(GLOB_RECURSE TEST *.cpp)
add_executable(test ${TEST})
target_include_directories(csugar PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test)
target_link_libraries(test csugar)
