#include "suffix_array.hpp"

#include <iostream>
#include <string>
#include <vector>


bool check_suffix_array(
    const std::string& text,
    const std::vector<int>& expected
) {
    const auto baseline =
        build_suffix_array_baseline(text);

    const auto optimized =
        build_suffix_array_optimized(text);

    const auto sais =
        build_suffix_array_sais(text);

    if (baseline != expected) {
        std::cerr
            << "Baseline Suffix Array failed for: "
            << text
            << '\n';

        return false;
    }

    if (optimized != expected) {
        std::cerr
            << "Optimized Suffix Array failed for: "
            << text
            << '\n';

        return false;
    }

    if (sais != expected) {
        std::cerr
            << "SA-IS failed for: "
            << text
            << '\n';

        return false;
    }

    if (baseline != optimized ||
        optimized != sais) {

        std::cerr
            << "Suffix Array implementations differ for: "
            << text
            << '\n';

        return false;
    }

    return true;
}


int main() {
    bool success = true;

    success &= check_suffix_array(
        "",
        {}
    );

    success &= check_suffix_array(
        "A",
        {0}
    );

    success &= check_suffix_array(
        "BANANA",
        {5, 3, 1, 0, 4, 2}
    );

    success &= check_suffix_array(
        "MISSISSIPPI",
        {10, 7, 4, 1, 0, 9, 8, 6, 3, 5, 2}
    );

    success &= check_suffix_array(
        "AAAAA",
        {4, 3, 2, 1, 0}
    );

    success &= check_suffix_array(
        "ACGTACGTGATTACANN",
        {
            12, 0, 4, 14, 9, 13, 1, 5, 8,
            2, 6, 16, 15, 11, 3, 7, 10
        }
    );

    if (!success) {
        return 1;
    }

    std::cout
        << "All Suffix Array tests passed.\n";

    return 0;
}