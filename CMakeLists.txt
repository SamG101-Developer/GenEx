cmake_minimum_required(VERSION 3.31)
project(GenEx)
set(PROJECT_VERSION 0.1.0)

# Set standard and include some common packages.
set(CMAKE_CXX_STANDARD 26)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Include 3rd party libraries and get headers.
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hpp")

# Create the GenEx library, and link 3rd party libraries.
add_library(GenExLib INTERFACE)
target_include_directories(GenExLib INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Library installation configuration.
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.hpp")
install(TARGETS GenExLib EXPORT GenExTargets)
install(EXPORT GenExTargets FILE GenExTargets.cmake NAMESPACE GenEx:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenEx)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/GenExConfigVersion.cmake" VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenExConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/GenExConfig.cmake" INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenEx)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/GenExConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/GenExConfigVersion.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GenEx)

# Library uninstallation configuration.
add_custom_target(uninstall COMMAND xargs rm -vf < "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt")

# Create the tests.
enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)
