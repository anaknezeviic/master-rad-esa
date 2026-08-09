#include "esa.hpp"

#include <iostream>
#include <string>
#include <vector>

int main()
{
    const std::string text =
        "MISSISSIPPI";

    const EnhancedSuffixArray esa =
        build_esa(text);

    const std::vector<LCPIntervalNode> nodes =
        build_lcp_interval_tree(
            esa.lcp_array
        );

    for (std::size_t i = 0;
         i < nodes.size();
         ++i)
    {
        const LCPIntervalNode& node =
            nodes[i];

        std::cout
            << "Node "
            << i
            << ": "
            << node.lcp_value
            << "-["
            << node.left
            << ".."
            << node.right
            << "]";

        if (node.lcp_value > 0) {

            const std::string repeat =
                text.substr(
                    esa.suffix_array[node.left],
                    node.lcp_value
                );

            std::cout
                << " -> "
                << repeat;
        }
        else {
            std::cout << " -> ROOT";
        }

        std::cout << "\n  children: ";

        if (node.children.empty()) {
            std::cout << "none";
        }
        else {
            for (int child : node.children) {
                std::cout << child << ' ';
            }
        }

        std::cout << "\n\n";
    }

    return 0;
}