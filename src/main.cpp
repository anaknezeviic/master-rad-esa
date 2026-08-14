#include "benchmark.hpp"
#include "esa.hpp"
#include "fasta.hpp"
#include "memory.hpp"
#include "output.hpp"
#include "repeats.hpp"

#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

RepeatType parse_repeat_type(const std::string& value)
{
    if (value == "maximal") {
        return RepeatType::Maximal;
    }

    if (value == "supermaximal") {
        return RepeatType::Supermaximal;
    }

    if (value == "both") {
        return RepeatType::Both;
    }

    throw std::runtime_error(
        "Invalid repeat type: " + value +
        ". Expected maximal, supermaximal, or both."
    );
}

using Clock = std::chrono::steady_clock;

int main(int argc, char* argv[])
{
    const auto total_start = Clock::now();

    if (argc < 2) {
        std::cerr
            << "Usage:\n"
            << "  master_rad_esa <fasta_file> "
            << "[--min-length N] "
            << "[--type maximal|supermaximal|both] "
            << "[--benchmark]\n";

        return 1;
    }

    const std::string file_path = argv[1];

    const std::filesystem::path input_path(file_path);

    const std::string input_name =
        input_path.stem().string();

    const std::string maximal_output_path =
        "data/processed/" +
        input_name +
        "_maximal.csv";

    const std::string supermaximal_output_path =
        "data/processed/" +
        input_name +
        "_supermaximal.csv";

    const std::string benchmark_output_path =
        "data/processed/benchmark.csv";

    RepeatOptions options;

    bool benchmark_mode = false;

    try {
        // Parse command-line arguments.
        for (int i = 2; i < argc; ++i) {

            const std::string argument = argv[i];

            if (argument == "--min-length") {

                if (i + 1 >= argc) {
                    throw std::runtime_error(
                        "Missing value after --min-length."
                    );
                }

                options.min_length =
                    std::stoi(argv[++i]);

                if (options.min_length < 1) {
                    throw std::runtime_error(
                        "--min-length must be at least 1."
                    );
                }
            }
            else if (argument == "--type") {

                if (i + 1 >= argc) {
                    throw std::runtime_error(
                        "Missing value after --type."
                    );
                }

                options.type =
                    parse_repeat_type(argv[++i]);
            }
            else if (argument == "--benchmark") {

                benchmark_mode = true;
            }
            else {
                throw std::runtime_error(
                    "Unknown argument: " + argument
                );
            }
        }

        // FASTA parsing.
        const auto fasta_start = Clock::now();

        const FastaRecord record =
            read_fasta(file_path);

        const auto fasta_end = Clock::now();

        const double fasta_time_ms =
            std::chrono::duration<double, std::milli>(
                fasta_end - fasta_start
            ).count();

        std::cout
            << "FASTA header: "
            << record.header
            << '\n';

        std::cout
            << "Sequence length: "
            << record.sequence.size()
            << '\n';

        std::cout
            << "Minimum repeat length: "
            << options.min_length
            << '\n';

        if (benchmark_mode) {
            std::cout
                << "Mode: benchmark\n";
        }

        std::cout << '\n';

        // ESA construction.
        const auto esa_start = Clock::now();

        ESAConstructionMetrics esa_metrics;

        const EnhancedSuffixArray esa =
            build_esa(
                record.sequence,
                &esa_metrics
            );

        const auto esa_end = Clock::now();

        const double esa_time_ms =
            std::chrono::duration<double, std::milli>(
                esa_end - esa_start
            ).count();

        double maximal_time_ms = 0.0;
        double supermaximal_time_ms = 0.0;

        std::size_t maximal_repeat_count = 0;
        std::size_t supermaximal_repeat_count = 0;

        // Maximal repeats.
        if (
            options.type == RepeatType::Maximal ||
            options.type == RepeatType::Both
        ) {
            const auto maximal_start =
                Clock::now();

            const std::vector<Repeat> maximal_repeats =
                find_maximal_repeats(
                    esa,
                    options
                );

            const auto maximal_end =
                Clock::now();

            maximal_time_ms =
                std::chrono::duration<double, std::milli>(
                    maximal_end - maximal_start
                ).count();

            maximal_repeat_count =
                maximal_repeats.size();

            // Detailed output is skipped in benchmark mode.
            if (!benchmark_mode) {

                std::cout
                    << "Maximal repeats:\n";

                if (maximal_repeats.empty()) {
                    std::cout
                        << "None\n";
                }
                else {
                    for (
                        const Repeat& repeat :
                        maximal_repeats
                    ) {
                        std::cout
                            << repeat.sequence
                            << " (length = "
                            << repeat.length
                            << ") positions: ";

                        for (
                            int position :
                            repeat.positions
                        ) {
                            std::cout
                                << position
                                << ' ';
                        }

                        std::cout << '\n';
                    }
                }

                write_repeats_csv(
                    maximal_output_path,
                    maximal_repeats
                );

                std::cout
                    << "\nMaximal repeats written to: "
                    << maximal_output_path
                    << "\n\n";
            }
        }

        // Supermaximal repeats.
        if (
            options.type == RepeatType::Supermaximal ||
            options.type == RepeatType::Both
        ) {
            const auto supermaximal_start =
                Clock::now();

            const std::vector<Repeat>
                supermaximal_repeats =
                    find_supermaximal_repeats(
                        esa,
                        options
                    );

            const auto supermaximal_end =
                Clock::now();

            supermaximal_time_ms =
                std::chrono::duration<double, std::milli>(
                    supermaximal_end -
                    supermaximal_start
                ).count();

            supermaximal_repeat_count =
                supermaximal_repeats.size();

            // Detailed output is skipped in benchmark mode.
            if (!benchmark_mode) {

                std::cout
                    << "Supermaximal repeats:\n";

                if (supermaximal_repeats.empty()) {
                    std::cout
                        << "None\n";
                }
                else {
                    for (
                        const Repeat& repeat :
                        supermaximal_repeats
                    ) {
                        std::cout
                            << repeat.sequence
                            << " (length = "
                            << repeat.length
                            << ") positions: ";

                        for (
                            int position :
                            repeat.positions
                        ) {
                            std::cout
                                << position
                                << ' ';
                        }

                        std::cout << '\n';
                    }
                }

                write_repeats_csv(
                    supermaximal_output_path,
                    supermaximal_repeats
                );

                std::cout
                    << "\nSupermaximal repeats written to: "
                    << supermaximal_output_path
                    << '\n';
            }
        }

        // Peak process memory.
        const std::size_t peak_memory_bytes =
            get_peak_memory_bytes();

        // Total runtime.
        const auto total_end =
            Clock::now();

        const double total_time_ms =
            std::chrono::duration<double, std::milli>(
                total_end - total_start
            ).count();

        // ESA memory estimate.
        const double esa_memory_kb =
            static_cast<double>(
                esa_metrics.estimated_memory_bytes
            ) / 1024.0;

        const double esa_memory_mb =
            esa_memory_kb / 1024.0;

        const double peak_memory_mb =
            static_cast<double>(
                peak_memory_bytes
            ) / (1024.0 * 1024.0);

        // Save benchmark results only in benchmark mode.
        if (benchmark_mode) {

            if (
                options.type == RepeatType::Maximal ||
                options.type == RepeatType::Both
            ) {
                BenchmarkResult benchmark;

                benchmark.dataset =
                    input_name;

                benchmark.implementation = 
                    "optimized_sa_maximal";

                benchmark.sequence_length =
                    record.sequence.size();

                benchmark.min_length =
                    options.min_length;

                benchmark.repeat_type =
                    "maximal";

                benchmark.fasta_time_ms =
                    fasta_time_ms;

                benchmark.suffix_array_time_ms =
                    esa_metrics.suffix_array_time_ms;

                benchmark.inverse_suffix_array_time_ms =
                    esa_metrics.inverse_suffix_array_time_ms;

                benchmark.lcp_time_ms =
                    esa_metrics.lcp_time_ms;

                benchmark.bwt_time_ms =
                    esa_metrics.bwt_time_ms;

                benchmark.esa_time_ms =
                    esa_time_ms;

                benchmark.repeat_detection_time_ms =
                    maximal_time_ms;

                benchmark.total_time_ms =
                    total_time_ms;

                benchmark.esa_memory_bytes =
                    esa_metrics.estimated_memory_bytes;

                benchmark.peak_memory_bytes =
                    peak_memory_bytes;

                benchmark.repeat_count =
                    maximal_repeat_count;

                append_benchmark_csv(
                    benchmark_output_path,
                    benchmark
                );
            }

            if (
                options.type ==
                    RepeatType::Supermaximal ||
                options.type ==
                    RepeatType::Both
            ) {
                BenchmarkResult benchmark;

                benchmark.dataset =
                    input_name;

                benchmark.implementation =
                    "optimized_sa";

                benchmark.sequence_length =
                    record.sequence.size();

                benchmark.min_length =
                    options.min_length;

                benchmark.repeat_type =
                    "supermaximal";

                benchmark.fasta_time_ms =
                    fasta_time_ms;

                benchmark.suffix_array_time_ms =
                    esa_metrics.suffix_array_time_ms;

                benchmark.inverse_suffix_array_time_ms =
                    esa_metrics.inverse_suffix_array_time_ms;

                benchmark.lcp_time_ms =
                    esa_metrics.lcp_time_ms;

                benchmark.bwt_time_ms =
                    esa_metrics.bwt_time_ms;

                benchmark.esa_time_ms =
                    esa_time_ms;

                benchmark.repeat_detection_time_ms =
                    supermaximal_time_ms;

                benchmark.total_time_ms =
                    total_time_ms;

                benchmark.esa_memory_bytes =
                    esa_metrics.estimated_memory_bytes;

                benchmark.peak_memory_bytes =
                    peak_memory_bytes;

                benchmark.repeat_count =
                    supermaximal_repeat_count;

                append_benchmark_csv(
                    benchmark_output_path,
                    benchmark
                );
            }
        }

        // Performance summary.
        std::cout
            << "\nPerformance:\n"

            << "FASTA parsing: "
            << fasta_time_ms
            << " ms\n"

            << "ESA construction: "
            << esa_time_ms
            << " ms\n"

            << "  Suffix Array: "
            << esa_metrics.suffix_array_time_ms
            << " ms\n"

            << "  Inverse Suffix Array: "
            << esa_metrics.inverse_suffix_array_time_ms
            << " ms\n"

            << "  LCP Array: "
            << esa_metrics.lcp_time_ms
            << " ms\n"

            << "  BWT: "
            << esa_metrics.bwt_time_ms
            << " ms\n";

        std::cout
            << "ESA estimated memory: ";

        if (esa_memory_mb >= 1.0) {
            std::cout
                << esa_memory_mb
                << " MB\n";
        }
        else {
            std::cout
                << esa_memory_kb
                << " KB\n";
        }

        std::cout
            << "Peak process memory: "
            << peak_memory_mb
            << " MB\n";

        if (
            options.type == RepeatType::Maximal ||
            options.type == RepeatType::Both
        ) {
            std::cout
                << "Maximal repeat detection: "
                << maximal_time_ms
                << " ms\n"

                << "Maximal repeats found: "
                << maximal_repeat_count
                << '\n';
        }

        if (
            options.type ==
                RepeatType::Supermaximal ||
            options.type ==
                RepeatType::Both
        ) {
            std::cout
                << "Supermaximal repeat detection: "
                << supermaximal_time_ms
                << " ms\n"

                << "Supermaximal repeats found: "
                << supermaximal_repeat_count
                << '\n';
        }

        std::cout
            << "Total runtime: "
            << total_time_ms
            << " ms\n";

        if (benchmark_mode) {
            std::cout
                << "Benchmark written to: "
                << benchmark_output_path
                << '\n';
        }
    }
    catch (const std::exception& error) {

        std::cerr
            << "Error: "
            << error.what()
            << '\n';

        return 1;
    }

    return 0;
}