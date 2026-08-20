#include "fasta.hpp"

#include <iostream>
#include <stdexcept>
#include <string>


bool expect_success(
    const std::string& path,
    const std::string& expected_header,
    const std::string& expected_sequence
) {
    try {
        const FastaRecord record = read_fasta(path);

        if (record.header != expected_header) {
            std::cerr
                << "Unexpected header for "
                << path
                << '\n';

            return false;
        }

        if (record.sequence != expected_sequence) {
            std::cerr
                << "Unexpected sequence for "
                << path
                << '\n';

            return false;
        }

        return true;
    }
    catch (const std::exception& error) {
        std::cerr
            << "Expected success for "
            << path
            << ", but got: "
            << error.what()
            << '\n';

        return false;
    }
}


bool expect_failure(
    const std::string& path
) {
    try {
        read_fasta(path);

        std::cerr
            << "Expected failure for "
            << path
            << ", but parsing succeeded.\n";

        return false;
    }
    catch (const std::exception&) {
        return true;
    }
}


int main() {
    bool success = true;

    success &= expect_success(
        "data/raw/tests/test.fasta",
        "test_sequence",
        "ACGTACGTGATTACANN"
    );

    success &= expect_failure(
        "data/raw/tests/test_invalid_char.fasta"
    );

    success &= expect_failure(
        "data/raw/tests/test_multiple_records.fasta"
    );

    success &= expect_failure(
        "data/raw/tests/test_no_header.fasta"
    );

    if (!success) {
        return 1;
    }

    std::cout
        << "All FASTA parser tests passed.\n";

    return 0;
}