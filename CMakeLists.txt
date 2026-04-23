cmake_minimum_required(VERSION 3.10)
project(FileSearchSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/document.cpp
    src/main.cpp
    src/read_input_functions.cpp
    src/search_server.cpp
    src/string_processing.cpp
    src/txtfile_scanner.cpp
)

add_executable(file_search ${SOURCES})

target_include_directories(file_search PRIVATE src)

target_link_options(file_search PRIVATE -static)

if (MINGW)
    target_link_libraries(file_search PRIVATE stdc++fs)
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(file_search PRIVATE stdc++fs)
endif()