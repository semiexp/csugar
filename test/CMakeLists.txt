file(GLOB_RECURSE TEST *.cpp)
add_executable(test ${TEST})
target_link_libraries(test csugar)
