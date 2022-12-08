add_executable(aoc_day8)

target_sources(aoc_day8 PRIVATE treehouse.cpp)

configure_file(treehouse_input ${CMAKE_CURRENT_BINARY_DIR}/treehouse_input COPYONLY)
configure_file(treehouse_input_test ${CMAKE_CURRENT_BINARY_DIR}/treehouse_input_test COPYONLY)
