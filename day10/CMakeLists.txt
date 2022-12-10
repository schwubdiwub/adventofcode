add_executable(aoc_day10)

target_sources(aoc_day10 PRIVATE devicerepair.cpp)

configure_file(devicerepair_input ${CMAKE_CURRENT_BINARY_DIR}/devicerepair_input COPYONLY)
configure_file(devicerepair_input_test ${CMAKE_CURRENT_BINARY_DIR}/devicerepair_input_test COPYONLY)
