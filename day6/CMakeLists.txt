add_executable(aoc_day6)

target_sources(aoc_day6 PRIVATE communication.cpp)

configure_file(communication_input ${CMAKE_CURRENT_BINARY_DIR}/communication_input COPYONLY)
