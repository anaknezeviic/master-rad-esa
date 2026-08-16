#include "lcp.hpp"

#include <vector>

std::vector<int> build_lcp_array(const std::string& text, const std::vector<int>& suffix_array){
    const int n = static_cast<int>(text.size());

    if (n == 0) {
        return {};
    }

    std::vector<int> inverse_suffix_array(n);
    std::vector<int> lcp(n, 0);

    for (int i = 0; i < n; ++i) {
        inverse_suffix_array[suffix_array[i]] = i;
    }

    int common_prefix_length = 0;

    for (int i = 0; i < n; ++i) {

        const int suffix_rank = inverse_suffix_array[i];

        if (suffix_rank == 0) {
            common_prefix_length = 0;
            continue;
        }

        const int previous_suffix_position = suffix_array[suffix_rank - 1];

        while (i + common_prefix_length < n && previous_suffix_position + common_prefix_length < n &&
                text[i + common_prefix_length] == text[previous_suffix_position + common_prefix_length]) {
            ++common_prefix_length;
        }

        lcp[suffix_rank] = common_prefix_length;

        if (common_prefix_length > 0) {
            --common_prefix_length;
        }
    }

    return lcp;
}