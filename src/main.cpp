#include "esa.hpp"
#include "fasta.hpp"
#include "repeats.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr
            << "Usage: master_rad_esa <fasta_file>\n";

        return 1;
    }

    const std::string file_path = argv[1];

    try {
        //Read DNA sequence from FASTA file
        const FastaRecord record =
            read_fasta(file_path);

        std::cout
            << "FASTA header: "
            << record.header
            << '\n';

        std::cout
            << "Sequence length: "
            << record.sequence.size()
            << "\n\n";

        //Configure repeat analysis
        RepeatOptions options;
        options.min_length = 2;

        //Build Enhanced Suffix Array structures
        const EnhancedSuffixArray esa =
            build_esa(record.sequence);

        //Find maximal repeats
        const std::vector<Repeat> maximal_repeats =
            find_maximal_repeats(
                esa,
                options
            );

        //Find supermaximal repeats
        const std::vector<Repeat> supermaximal_repeats =
            find_supermaximal_repeats(
                esa,
                options
            );

        //Print maximal repeats
        std::cout
            << "Maximal repeats "
            << "(minimum length = "
            << options.min_length
            << "):\n";

        if (maximal_repeats.empty()) {
            std::cout << "None\n";
        }

        for (const Repeat& repeat : maximal_repeats) {

            std::cout
                << repeat.sequence
                << " (length = "
                << repeat.length
                << ") positions: ";

            for (int position : repeat.positions) {
                std::cout << position << ' ';
            }

            std::cout << '\n';
        }

        std::cout << "\n";

        //Print supermaximal repeats
        std::cout
            << "Supermaximal repeats "
            << "(minimum length = "
            << options.min_length
            << "):\n";

        if (supermaximal_repeats.empty()) {
            std::cout << "None\n";
        }

        for (const Repeat& repeat : supermaximal_repeats) {

            std::cout
                << repeat.sequence
                << " (length = "
                << repeat.length
                << ") positions: ";

            for (int position : repeat.positions) {
                std::cout << position << ' ';
            }

            std::cout << '\n';
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