#include "esa.hpp"
#include "repeats.hpp"

#include <iostream>
#include <string>
#include <vector>

int main()
{
    const std::string text = "ACGTCGACGTAG";

    const EnhancedSuffixArray esa =
        build_esa(text);

    const std::vector<Repeat> repeats =
        find_supermaximal_repeats(esa);

    std::cout << "Text: " << text << "\n\n";

    std::cout << "Supermaximal repeats:\n";

    for (const Repeat& repeat : repeats) {

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

    return 0;
}