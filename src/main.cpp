#include "esa.hpp"
#include "fasta.hpp"
#include "repeats.hpp"

#include <exception>
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

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr
            << "Usage:\n"
            << "  master_rad_esa <fasta_file> "
            << "[--min-length N] "
            << "[--type maximal|supermaximal|both]\n";

        return 1;
    }

    const std::string file_path = argv[1];

    RepeatOptions options;

    try {
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
            else {
                throw std::runtime_error(
                    "Unknown argument: " + argument
                );
            }
        }

        const FastaRecord record =
            read_fasta(file_path);

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
            << "\n\n";

        const EnhancedSuffixArray esa =
            build_esa(record.sequence);

        if (
            options.type == RepeatType::Maximal ||
            options.type == RepeatType::Both
        ) {
            const std::vector<Repeat> maximal_repeats =
                find_maximal_repeats(
                    esa,
                    options
                );

            std::cout
                << "Maximal repeats:\n";

            if (maximal_repeats.empty()) {
                std::cout << "None\n";
            }
            else {
                for (const Repeat& repeat :
                     maximal_repeats)
                {
                    std::cout
                        << repeat.sequence
                        << " (length = "
                        << repeat.length
                        << ") positions: ";

                    for (int position :
                         repeat.positions)
                    {
                        std::cout
                            << position
                            << ' ';
                    }

                    std::cout << '\n';
                }
            }

            std::cout << '\n';
        }

        if (
            options.type == RepeatType::Supermaximal ||
            options.type == RepeatType::Both
        ) {
            const std::vector<Repeat> supermaximal_repeats =
                find_supermaximal_repeats(
                    esa,
                    options
                );

            std::cout
                << "Supermaximal repeats:\n";

            if (supermaximal_repeats.empty()) {
                std::cout << "None\n";
            }
            else {
                for (const Repeat& repeat :
                     supermaximal_repeats)
                {
                    std::cout
                        << repeat.sequence
                        << " (length = "
                        << repeat.length
                        << ") positions: ";

                    for (int position :
                         repeat.positions)
                    {
                        std::cout
                            << position
                            << ' ';
                    }

                    std::cout << '\n';
                }
            }
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