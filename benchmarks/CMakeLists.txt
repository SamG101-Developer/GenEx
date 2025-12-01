# Find and configure Google Benchmark.
find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

# Build the benchmark executable.
file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(GenExBenchmark ${BENCHMARK_SOURCES})
target_link_libraries(GenExBenchmark PRIVATE benchmark::benchmark Threads::Threads GenEx)
