#include "benchmark.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

void append_benchmark_csv(const std::string& file_path,const BenchmarkResult& result) {
    const std::filesystem::path path(file_path);

    if (path.has_parent_path() && !path.parent_path().empty()) {
        std::filesystem::create_directories(path.parent_path());
    }

    const bool file_exists = std::filesystem::exists(path);

    std::ofstream output(file_path, std::ios::app);

    if (!output) {
        throw std::runtime_error(
            "Could not open benchmark file: " +
            file_path
        );
    }

    if (!file_exists) {
        output
            << "dataset,"
            << "implementation,"
            << "sequence_length,"
            << "min_length,"
            << "repeat_type,"
            << "fasta_time_ms,"
            << "suffix_array_time_ms,"
            << "inverse_suffix_array_time_ms,"
            << "lcp_time_ms,"
            << "bwt_time_ms,"
            << "esa_time_ms,"
            << "repeat_detection_time_ms,"
            << "total_time_ms,"
            << "esa_memory_bytes,"
            << "peak_memory_bytes,"
            << "repeat_count\n";
    }

    output
        << result.dataset << ','
        << result.implementation << ','
        << result.sequence_length << ','
        << result.min_length << ','
        << result.repeat_type << ','
        << result.fasta_time_ms << ','
        << result.suffix_array_time_ms << ','
        << result.inverse_suffix_array_time_ms << ','
        << result.lcp_time_ms << ','
        << result.bwt_time_ms << ','
        << result.esa_time_ms << ','
        << result.repeat_detection_time_ms << ','
        << result.total_time_ms << ','
        << result.esa_memory_bytes << ','
        << result.peak_memory_bytes << ','
        << result.repeat_count
        << '\n';
}