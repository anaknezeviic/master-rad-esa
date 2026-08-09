#include "suffix_array.hpp"
#include "lcp.hpp"

#include <iostream>
#include <string>
#include <vector>

void run_test(
    const std::string& text,
    const std::vector<int>& expected_sa,
    const std::vector<int>& expected_lcp
)
{
    const std::vector<int> suffix_array =
        build_suffix_array(text);

    const std::vector<int> lcp_array =
        build_lcp_array(text, suffix_array);

    std::cout << "Text: " << text << '\n';

    std::cout << "SA:  ";
    for (int value : suffix_array) {
        std::cout << value << ' ';
    }

    std::cout << "\nLCP: ";
    for (int value : lcp_array) {
        std::cout << value << ' ';
    }

    std::cout << "\n";

    const bool sa_correct = suffix_array == expected_sa;
    const bool lcp_correct = lcp_array == expected_lcp;

    if (sa_correct && lcp_correct) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAIL\n";

        if (!sa_correct) {
            std::cout << "  Suffix Array is incorrect.\n";
        }

        if (!lcp_correct) {
            std::cout << "  LCP Array is incorrect.\n";
        }
    }

    std::cout << "--------------------------\n";
}

int main()
{
    run_test(
        "BANANA",
        {5, 3, 1, 0, 4, 2},
        {0, 1, 3, 0, 0, 2}
    );

    run_test(
        "AAAAA",
        {4, 3, 2, 1, 0},
        {0, 1, 2, 3, 4}
    );

    run_test(
        "ACGT",
        {0, 1, 2, 3},
        {0, 0, 0, 0}
    );

    run_test(
        "MISSISSIPPI",
        {10, 7, 4, 1, 0, 9, 8, 6, 3, 5, 2},
        {0, 1, 1, 4, 0, 0, 1, 0, 2, 1, 3}
    );

    return 0;
}