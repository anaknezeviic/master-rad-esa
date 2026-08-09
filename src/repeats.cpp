#include "repeats.hpp"

#include <set>
#include <vector>

std::vector<Repeat> find_supermaximal_repeats(
    const EnhancedSuffixArray& esa
)
{
    std::vector<Repeat> repeats;

    const std::vector<LCPInterval> intervals =
        build_lcp_intervals(esa.lcp_array);

    for (const LCPInterval& interval : intervals) {

        const int lcp_value = interval.lcp_value;

        // Condition 1:
        // The interval must be a local maximum in the LCP table.
        bool is_local_maximum = true;

        for (int k = interval.left + 1;
             k <= interval.right;
             ++k)
        {
            if (esa.lcp_array[k] != lcp_value) {
                is_local_maximum = false;
                break;
            }
        }

        if (!is_local_maximum) {
            continue;
        }

        // Condition 2:
        // BWT characters in the interval must be pairwise distinct.
        std::set<char> preceding_characters;

        bool bwt_characters_are_distinct = true;

        for (int k = interval.left;
             k <= interval.right;
             ++k)
        {
            const char character = esa.bwt[k];

            if (preceding_characters.count(character) > 0) {
                bwt_characters_are_distinct = false;
                break;
            }

            preceding_characters.insert(character);
        }

        if (!bwt_characters_are_distinct) {
            continue;
        }

        Repeat repeat;

        repeat.length = lcp_value;

        repeat.sequence =
            esa.text.substr(
                esa.suffix_array[interval.left],
                lcp_value
            );

        for (int k = interval.left;
             k <= interval.right;
             ++k)
        {
            repeat.positions.push_back(
                esa.suffix_array[k]
            );
        }

        repeats.push_back(repeat);
    }

    return repeats;
}