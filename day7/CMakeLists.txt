add_executable(aoc_day7)

target_sources(aoc_day7 PRIVATE filesystem.cpp)

configure_file(filesystem_input ${CMAKE_CURRENT_BINARY_DIR}/filesystem_input COPYONLY)
configure_file(filesystem_input_test ${CMAKE_CURRENT_BINARY_DIR}/filesystem_input_test COPYONLY)
