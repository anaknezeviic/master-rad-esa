#include "suffix_array.hpp"
#include "lcp.hpp"

#include <iostream>
#include <string>
#include <vector>

int main()
{
    const std::string text = "BANANA";

    const std::vector<int> suffix_array =
        build_suffix_array(text);

    const std::vector<int> lcp_array =
        build_lcp_array(text, suffix_array);

    std::cout << "Text: " << text << "\n\n";

    std::cout << "SA\tLCP\tSuffix\n";

    for (std::size_t i = 0; i < suffix_array.size(); ++i) {
        std::cout
            << suffix_array[i]
            << '\t'
            << lcp_array[i]
            << '\t'
            << text.substr(suffix_array[i])
            << '\n';
    }

    return 0;
}