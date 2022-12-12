add_executable(aoc_day12)

target_sources(aoc_day12 PRIVATE findingsignal.cpp)

configure_file(findingsignal_input ${CMAKE_CURRENT_BINARY_DIR}/findingsignal_input COPYONLY)
configure_file(findingsignal_input_test ${CMAKE_CURRENT_BINARY_DIR}/findingsignal_input_test COPYONLY)
