find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

add_executable(
        BenchmarkGenEx benchmark.cpp)

target_link_libraries(
        BenchmarkGenEx PRIVATE
        benchmark::benchmark Threads::Threads GenExLib)
