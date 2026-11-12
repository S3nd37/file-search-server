cmake_minimum_required(VERSION 3.10)
project(FileSearchSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/document.cpp
    src/main.cpp
    src/search_server.cpp
    src/string_processing.cpp
    src/txtfile_scanner.cpp
    src/file_reader.cpp
)

add_executable(file_search ${SOURCES})

target_include_directories(file_search PRIVATE src)

target_link_options(file_search PRIVATE -static)