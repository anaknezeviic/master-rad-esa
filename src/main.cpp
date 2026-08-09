#include "esa.hpp"

#include <iostream>
#include <string>

int main()
{
    const std::string text = "BANANA";

    const EnhancedSuffixArray esa =
        build_esa(text);

    std::cout << "Text: " << esa.text << "\n\n";

    std::cout << "SA:\n";
    for (int value : esa.suffix_array) {
        std::cout << value << ' ';
    }

    std::cout << "\n\nInverse SA:\n";
    for (int value : esa.inverse_suffix_array) {
        std::cout << value << ' ';
    }

    std::cout << "\n\nLCP:\n";
    for (int value : esa.lcp_array) {
        std::cout << value << ' ';
    }

    std::cout << "\n\nBWT:\n";
    std::cout << esa.bwt << '\n';

    return 0;
}