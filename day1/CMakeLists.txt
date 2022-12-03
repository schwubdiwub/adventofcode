add_executable(aoc_day1)

target_sources(aoc_day1 PRIVATE calories.cpp)

configure_file(calories_input.txt ${CMAKE_CURRENT_BINARY_DIR}/calories_input.txt COPYONLY)
