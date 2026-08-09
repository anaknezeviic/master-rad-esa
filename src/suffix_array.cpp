#include "suffix_array.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

std::vector<int> build_suffix_array(const std::string& text)
{
    const int n = static_cast<int>(text.size());

    if (n == 0) {
        return {};
    }

    std::vector<int> suffix_array(n);
    std::vector<int> equivalence_class(n);
    std::vector<int> new_equivalence_class(n);

    // Initially:
    // suffix_array = [0, 1, 2, ..., n - 1]
    std::iota(suffix_array.begin(), suffix_array.end(), 0);

    // Initial equivalence_class is determined by the character at each position.
    for (int i = 0; i < n; ++i) {
        equivalence_class[i] = static_cast<unsigned char>(text[i]);
    }

    for (int k = 1; k < n; k *= 2) {

        auto compare_suffixes = [&](int i, int j) {
            if (equivalence_class[i] != equivalence_class[j]) {
                return equivalence_class[i] < equivalence_class[j];
            }

            const int equivalence_class_i_second =
                (i + k < n) ? equivalence_class[i + k] : -1;

            const int equivalence_class_j_second =
                (j + k < n) ? equivalence_class[j + k] : -1;

            return equivalence_class_i_second < equivalence_class_j_second;
        };

        std::sort(
            suffix_array.begin(),
            suffix_array.end(),
            compare_suffixes
        );

        new_equivalence_class[suffix_array[0]] = 0;

        for (int i = 1; i < n; ++i) {

            const int previous = suffix_array[i - 1];
            const int current = suffix_array[i];

            new_equivalence_class[current] =
                new_equivalence_class[previous]
                + (compare_suffixes(previous, current) ? 1 : 0);
        }

        equivalence_class = new_equivalence_class;

        // All suffixes have unique equivalence_classs.
        if (equivalence_class[suffix_array[n - 1]] == n - 1) {
            break;
        }
    }

    return suffix_array;
}