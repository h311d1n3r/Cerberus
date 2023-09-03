cmake_minimum_required(VERSION 3.24)
project(Cerberus)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/argparse)
include_directories(lib/argparse/include src)

add_executable(Cerberus src/main.cpp src/utils/logger.h src/utils/logger.cpp src/utils/arg_parser.h src/utils/config.h src/utils/arg_parser.cpp src/global_defs.h src/langs/lang_manager.h src/langs/lang_manager.cpp src/types/value_ordered_map.h src/binaries/bin_identifier.cpp src/user/user_prompt.cpp src/utils/convert.h src/binaries/bin_handler.h src/binaries/handlers/elf_handler.h src/binaries/handlers/elf_handler.cpp src/binaries/bin_extractor.h src/binaries/bin_types.h src/binaries/extractors/lief_extractor.h src/binaries/extractors/lief_extractor.cpp src/binaries/extractors/radare_extractor.h src/binaries/extractors/radare_extractor.cpp src/binaries/handlers/pe_handler.h src/binaries/handlers/pe_handler.cpp)
target_link_libraries(Cerberus PRIVATE argparse)
set_target_properties(Cerberus PROPERTIES OUTPUT_NAME cerberus)