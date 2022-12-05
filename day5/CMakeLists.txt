add_executable(aoc_day5)

target_sources(aoc_day5 PRIVATE cratestacking.cpp)

configure_file(cratestacking_input ${CMAKE_CURRENT_BINARY_DIR}/cratestacking_input COPYONLY)
