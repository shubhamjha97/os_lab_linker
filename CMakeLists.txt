set (CMAKE_CXX_STANDARD 11)
project(linker)
add_executable(main main.cpp Tokenizer.cpp Parser.cpp)
set_target_properties(main PROPERTIES OUTPUT_NAME "linker")
