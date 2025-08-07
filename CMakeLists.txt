cmake_minimum_required(VERSION 3.31)
project(GenEx)

# Set standard and include some common packages.
set(CMAKE_CXX_STANDARD 26)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Include 3rd party libraries and get headers.
find_package(Boost REQUIRED)
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hpp")

# Create the GenEx library, and link 3rd party libraries.
add_library(GenExLib INTERFACE)
target_link_libraries(GenExLib INTERFACE Boost::boost)
target_include_directories(GenExLib INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Library installation configuration.
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS GenExLib EXPORT GenExTargets)
install(EXPORT GenExTargets FILE GenExTargets.cmake NAMESPACE GenEx:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenEx)

# Create the test executable.
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE GenExLib)
