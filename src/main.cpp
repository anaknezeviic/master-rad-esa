#include "esa.hpp"

#include <iostream>
#include <string>
#include <vector>

int main()
{
    const std::string text = "MISSISSIPPI";

    const EnhancedSuffixArray esa =
        build_esa(text);

    const std::vector<LCPInterval> intervals =
        build_lcp_intervals(esa.lcp_array);

    std::cout << "Text: " << text << "\n\n";

    std::cout << "SA:\n";
    for (int value : esa.suffix_array) {
        std::cout << value << ' ';
    }

    std::cout << "\n\nLCP:\n";
    for (int value : esa.lcp_array) {
        std::cout << value << ' ';
    }

    std::cout << "\n\nLCP intervals:\n";

    for (const LCPInterval& interval : intervals) {

        const std::string repeat =
            text.substr(
                esa.suffix_array[interval.left],
                interval.lcp_value
            );

        std::cout
            << interval.lcp_value
            << "-["
            << interval.left
            << ".."
            << interval.right
            << "]"
            << " -> "
            << repeat
            << '\n';
    }

    return 0;
}