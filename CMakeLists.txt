cmake_minimum_required(VERSION 3.24)
project(Cerberus)

set(CMAKE_CXX_STANDARD 17)

find_path(libelf.h NAMES LIBELF)

# will be included statically
add_subdirectory(lib/lief)
add_subdirectory(lib/argparse)
# will be included dynamically
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibArchive REQUIRED)

include_directories(lib/argparse/include lib/lief/include lib/gzip-hpp/include ${LIBELF} src)

add_executable(Cerberus src/main.cpp src/utils/logger.h src/utils/logger.cpp src/utils/arg_parser.h src/utils/config.h src/utils/arg_parser.cpp src/global_defs.h src/langs/lang_manager.h src/langs/lang_manager.cpp src/types/value_ordered_map.h src/binaries/bin_identifier.cpp src/user/user_prompt.cpp src/utils/convert.h src/binaries/bin_handler.h src/binaries/handlers/elf_handler.h src/binaries/handlers/elf_handler.cpp src/binaries/bin_extractor.h src/binaries/bin_types.h src/binaries/extractors/lief_extractor.h src/binaries/extractors/lief_extractor.cpp src/binaries/extractors/radare_extractor.h src/binaries/extractors/radare_extractor.cpp src/binaries/handlers/pe_handler.h src/binaries/handlers/pe_handler.cpp src/binaries/bin_handler.cpp src/langs/lib_regex.h src/utils/search.h src/utils/search.cpp src/langs/lang_types.h src/langs/lib_regex.cpp src/utils/convert.cpp src/binaries/lib/lib_manager.h src/binaries/lib/lib_manager.cpp src/binaries/lib/install/lib_installer.h src/binaries/lib/install/rust_lib_installer.h src/binaries/lib/install/go_lib_installer.h src/binaries/lib/install/rust_lib_installer.cpp src/binaries/lib/install/go_lib_installer.cpp src/utils/file_downloader.h src/utils/file_downloader.cpp src/utils/file_operations.h src/utils/file_operations.cpp src/user/dependencies/dependency_manager.h src/command/command_executor.h src/command/command_executor.cpp src/user/local_config.h src/user/local_config.cpp src/user/dependencies/dependency_manager.cpp src/algorithm/part_hash_algorithm.h src/algorithm/part_hash_algorithm.cpp src/algorithm/algorithm.h src/binaries/extractors/libelf_extractor.h src/binaries/extractors/libelf_extractor.cpp)

target_link_libraries(Cerberus PRIVATE argparse PRIVATE LIEF::LIEF PRIVATE CURL::libcurl PRIVATE ZLIB::ZLIB PRIVATE LibArchive::LibArchive PRIVATE uuid PRIVATE elf)
set_target_properties(Cerberus PROPERTIES OUTPUT_NAME cerberus)