cmake_minimum_required(VERSION 3.31)
project(GenEx)

set(CMAKE_CXX_STANDARD 26)
find_package(Boost REQUIRED)
add_subdirectory(modules/genex)
include_directories(${CMAKE_CURRENT_LIST_DIR}/headers)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE GenEx::All)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)

add_library(_GenEx INTERFACE)
target_link_libraries(_GenEx INTERFACE GenEx::All)
add_library(GenEx::GenEx ALIAS _GenEx)
