add_executable(aoc_day3)

target_sources(aoc_day3 PRIVATE rucksacks.cpp)

configure_file(rucksacks_input.txt ${CMAKE_CURRENT_BINARY_DIR}/rucksacks_input.txt COPYONLY)
