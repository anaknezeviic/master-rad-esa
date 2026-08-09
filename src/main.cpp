#include "esa.hpp"
#include "repeats.hpp"

#include <iostream>
#include <string>
#include <vector>

int main()
{
    const std::string text =
        "BANANA";

    const EnhancedSuffixArray esa =
        build_esa(text);

    const std::vector<MaximalRepeatedPair> pairs =
        find_maximal_repeated_pairs(esa);

    std::cout << "Text: "
              << text
              << "\n\n";

    std::cout << "Maximal repeated pairs:\n";

    for (const MaximalRepeatedPair& pair : pairs) {

        std::cout
            << pair.sequence
            << " (length = "
            << pair.length
            << ") positions: "
            << pair.first_position
            << ", "
            << pair.second_position
            << '\n';
    }

    return 0;
}