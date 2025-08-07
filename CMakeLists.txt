cmake_minimum_required(VERSION 3.31)
project(GenEx)

set(CMAKE_CXX_STANDARD 26)
find_package(Boost REQUIRED)
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hpp")

add_library(GenExLib INTERFACE)
target_include_directories(GenExLib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(GenExLib INTERFACE Boost::boost)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE GenExLib)
