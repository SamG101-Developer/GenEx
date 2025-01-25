cmake_minimum_required(VERSION 3.31)
project(GenEx)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(modules/genex)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE GenEx::All)
