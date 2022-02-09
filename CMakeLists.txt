cmake_minimum_required(VERSION 3.19)
project(sfvArchiving)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(include)

add_executable(${PROJECT_NAME}
        src/main.cpp
         ${PROJECT_SOURCE_DIR}/include/crc/Crc32.cpp
        src/sfv/sfv_common.cpp
        src/sfv/sfv_common_crc.cpp
        )