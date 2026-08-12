#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <cstddef>
#include <string>

struct BenchmarkResult
{
    std::string dataset;
    std::size_t sequence_length = 0;
    int min_length = 1;
    std::string repeat_type;

    double fasta_time_ms = 0.0;
    double suffix_array_time_ms = 0.0;
    double inverse_suffix_array_time_ms = 0.0;
    double lcp_time_ms = 0.0;
    double bwt_time_ms = 0.0;
    double esa_time_ms = 0.0;

    double repeat_detection_time_ms = 0.0;
    double total_time_ms = 0.0;

    std::size_t esa_memory_bytes = 0;
    std::size_t peak_memory_bytes = 0;

    std::size_t repeat_count = 0;
};

void append_benchmark_csv(
    const std::string& file_path,
    const BenchmarkResult& result
);

#endif