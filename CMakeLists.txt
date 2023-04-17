cmake_minimum_required(VERSION 3.12)

project(khct_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
   add_compile_options(/W4)
else()
   add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(khct INTERFACE)
target_include_directories(khct INTERFACE ${CMAKE_SOURCE_DIR}/khct)

add_executable(string_test tests/string.cpp)
target_link_libraries(string_test PUBLIC khct)

add_executable(map_test tests/map.cpp)
target_link_libraries(map_test PUBLIC khct)

add_executable(json_test tests/json.cpp)
target_link_libraries(json_test PUBLIC khct)

add_executable(common_tests tests/common.cpp)
target_link_libraries(common_tests PUBLIC khct)
