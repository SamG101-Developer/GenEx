# Find GoogleTest, or fetch it where there is no prebuilt package (Windows).
find_package(GTest QUIET)
if (NOT GTest_FOUND)
  include(FetchContent)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.tar.gz
    URL_HASH SHA256=6e3191c1455468b3fc35a417fb565c1c5071aee1b7e7f85e30cf48a98d37d8b5)

  # GoogleTest doesn't use modules, and asks for an older standard than "import std" can be built with.
  block()
    set(CMAKE_CXX_MODULE_STD OFF)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    FetchContent_MakeAvailable(googletest)
  endblock()
endif ()

# Build the test executable.
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(GenExTests ${TEST_SOURCES})
target_link_libraries(GenExTests PRIVATE GTest::gtest_main GenEx)

# Register the tests.
include(GoogleTest)
gtest_discover_tests(GenExTests)
