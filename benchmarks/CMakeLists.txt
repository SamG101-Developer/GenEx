find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(BenchmarkGenEx ${BENCHMARK_SOURCES})

target_link_libraries(
        BenchmarkGenEx PRIVATE
        benchmark::benchmark Threads::Threads GenExLib)
