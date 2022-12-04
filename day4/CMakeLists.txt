add_executable(aoc_day4)

target_sources(aoc_day4 PRIVATE campcleaning.cpp)

configure_file(campcleaning_input.txt ${CMAKE_CURRENT_BINARY_DIR}/campcleaning_input.txt COPYONLY)
