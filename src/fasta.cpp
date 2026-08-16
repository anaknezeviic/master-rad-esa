#include "fasta.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>

FastaRecord read_fasta(const std::string& file_path) {
    std::ifstream input(file_path);

    if (!input) {
        throw std::runtime_error(
            "Could not open FASTA file: " + file_path
        );
    }

    FastaRecord record;

    std::string line;
    bool header_found = false;

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        if (line[0] == '>') {
            if (header_found) {
                throw std::runtime_error(
                    "FASTA file contains more than one record."
                );
            }

            record.header = line.substr(1);
            header_found = true;

            continue;
        }

        if (!header_found) {
            throw std::runtime_error(
                "Invalid FASTA file: sequence appears before header."
            );
        }

        for (char character : line) {

            if (std::isspace(static_cast<unsigned char>(character))) {
                continue;
            }

            const char nucleotide = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));

            if ( nucleotide != 'A' && nucleotide != 'C' && nucleotide != 'G' && nucleotide != 'T' && nucleotide != 'N') {
                throw std::runtime_error(
                    std::string(
                        "Invalid nucleotide in FASTA file: "
                    ) + nucleotide
                );
            }

            record.sequence.push_back(nucleotide);
        }
    }

    if (!header_found) {
        throw std::runtime_error(
            "Invalid FASTA file: missing header."
        );
    }

    if (record.sequence.empty()) {
        throw std::runtime_error(
            "Invalid FASTA file: sequence is empty."
        );
    }

    return record;
}