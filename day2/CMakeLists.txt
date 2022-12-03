add_executable(aoc_day2)

target_sources(aoc_day2 PRIVATE rockpapersissors.cpp)

configure_file(rockpapersissors_input.txt ${CMAKE_CURRENT_BINARY_DIR}/rockpapersissors_input.txt COPYONLY)
