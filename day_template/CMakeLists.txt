add_executable(aoc_dayX)

target_sources(aoc_dayX PRIVATE template_cpp.cpp)

configure_file(template_input ${CMAKE_CURRENT_BINARY_DIR}/template_input COPYONLY)
configure_file(template_input_test ${CMAKE_CURRENT_BINARY_DIR}/template_input_test COPYONLY)
