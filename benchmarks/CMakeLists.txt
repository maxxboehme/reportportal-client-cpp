find_package(benchmark MODULE REQUIRED)

# This allows for faster build times if multiple benchmark
# executables are being created and just need generic main.
add_library(reportportal-client-cpp_benchmark_main STATIC)
target_sources(reportportal-client-cpp_benchmarks
    PRIVATE
        benchmark_main.cpp)
target_link_libraries(reportportal-client-cpp_benchmark_main
    PUBLIC
        benchmark::benchmark)

add_executable(reportportal-client-cpp_benchmarks)
target_sources(reportportal-client-cpp_benchmarks
    PRIVATE
        benchmarks.cpp)
target_link_libraries(reportportal-client-cpp_benchmarks
    PRIVATE
        benchmark::benchmark
        reportportal-client-cpp_benchmark_main)

