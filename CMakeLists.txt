cmake_minimum_required(VERSION 3.24)
project(Cerberus)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/argparse)
include_directories(lib/argparse/include src)

add_executable(Cerberus src/main.cpp src/utils/logger.h src/utils/logger.cpp src/utils/arg_parser.h src/utils/config.h src/utils/arg_parser.cpp src/global_defs.h src/langs/lang_manager.h src/langs/lang_manager.cpp)
target_link_libraries(Cerberus PRIVATE argparse)
set_target_properties(Cerberus PROPERTIES OUTPUT_NAME cerberus)