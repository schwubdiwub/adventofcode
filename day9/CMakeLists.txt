add_executable(aoc_day9)

target_sources(aoc_day9 PRIVATE ropephysics.cpp)

configure_file(ropephysics_input ${CMAKE_CURRENT_BINARY_DIR}/ropephysics_input COPYONLY)
configure_file(ropephysics_input_test ${CMAKE_CURRENT_BINARY_DIR}/ropephysics_input_test COPYONLY)
