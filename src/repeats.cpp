#include "repeats.hpp"

#include <vector>
#include <algorithm>
#include <string>
#include <set>

std::vector<Repeat> find_supermaximal_repeats(
    const EnhancedSuffixArray& esa,
     const RepeatOptions& options
)
{
    std::vector<Repeat> repeats;

    const std::vector<LCPInterval> intervals =
        build_lcp_intervals(esa.lcp_array);

    for (const LCPInterval& interval : intervals) {

        const int lcp_value = interval.lcp_value;

        if (lcp_value < options.min_length) {
            continue;
        }
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

std::vector<MaximalRepeatedPair> find_maximal_repeated_pairs(
    const EnhancedSuffixArray& esa,
     const RepeatOptions& options
)
{
    std::vector<MaximalRepeatedPair> pairs;

    const std::vector<LCPIntervalNode> nodes =
        build_lcp_interval_tree(esa.lcp_array);

    const int n =
        static_cast<int>(esa.text.size());

    for (const LCPIntervalNode& node : nodes) {

        
        if (node.lcp_value == 0) {
            continue;
        }

        const int repeat_length =
            node.lcp_value;

        if (repeat_length < options.min_length) {
            continue;
        }

        std::vector<std::vector<int>> groups;

        int current = node.left;

        std::vector<int> sorted_children =
            node.children;

        std::sort(
            sorted_children.begin(),
            sorted_children.end(),
            [&](int first, int second)
            {
                return nodes[first].left <
                       nodes[second].left;
            }
        );

        for (int child_index : sorted_children) {

            const LCPIntervalNode& child =
                nodes[child_index];

           
            while (current < child.left) {

                groups.push_back({
                    esa.suffix_array[current]
                });

                ++current;
            }

           
            std::vector<int> child_positions;

            for (int k = child.left;
                 k <= child.right;
                 ++k)
            {
                child_positions.push_back(
                    esa.suffix_array[k]
                );
            }

            groups.push_back(child_positions);

            current = child.right + 1;
        }

       
        while (current <= node.right) {

            groups.push_back({
                esa.suffix_array[current]
            });

            ++current;
        }

        for (std::size_t first_group = 0;
             first_group < groups.size();
             ++first_group)
        {
            for (std::size_t second_group =
                     first_group + 1;
                 second_group < groups.size();
                 ++second_group)
            {
                for (int first_position :
                     groups[first_group])
                {
                    for (int second_position :
                         groups[second_group])
                    {
                        const char first_left =
                            (first_position == 0)
                            ? '$'
                            : esa.text[
                                first_position - 1
                              ];

                        const char second_left =
                            (second_position == 0)
                            ? '$'
                            : esa.text[
                                second_position - 1
                              ];

                        if (
                            first_left ==
                            second_left
                        ) {
                            continue;
                        }

                        int p1 = first_position;
                        int p2 = second_position;

                        if (p2 < p1) {
                            std::swap(p1, p2);
                        }

                        MaximalRepeatedPair pair;

                        pair.length =
                            repeat_length;

                        pair.sequence =
                            esa.text.substr(
                                p1,
                                repeat_length
                            );

                        pair.first_position =
                            p1;

                        pair.second_position =
                            p2;

                        pairs.push_back(pair);
                    }
                }
            }
        }
    }

    return pairs;
}

std::vector<Repeat> find_maximal_repeats(
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options
)
{
    const std::vector<MaximalRepeatedPair> pairs =
        find_maximal_repeated_pairs(
            esa,
            options
        );

    //A substring is a maximal repeat if there exists
    //at least one maximal repeated pair for it.
    std::set<std::string> maximal_sequences;

    for (const MaximalRepeatedPair& pair : pairs) {
        maximal_sequences.insert(pair.sequence);
    }

    //LCP intervals contain all occurrences of their
    //corresponding repeated substring.
    const std::vector<LCPIntervalNode> nodes =
        build_lcp_interval_tree(
            esa.lcp_array
        );

    std::vector<Repeat> repeats;

    for (const LCPIntervalNode& node : nodes) {

        if (node.lcp_value == 0) {
            continue;
        }

        if (node.lcp_value < options.min_length) {
            continue;
        }

        const std::string sequence =
            esa.text.substr(
                esa.suffix_array[node.left],
                node.lcp_value
            );

        //If no maximal repeated pair exists for this
        //sequence, it is not a maximal repeat
        if (
            maximal_sequences.find(sequence) ==
            maximal_sequences.end()
        ) {
            continue;
        }

        Repeat repeat;

        repeat.sequence = sequence;
        repeat.length = node.lcp_value;

        //Every suffix-array position in this LCP interval
        //corresponds to one occurrence of the repeat
        for (int k = node.left;
             k <= node.right;
             ++k)
        {
            repeat.positions.push_back(
                esa.suffix_array[k]
            );
        }

        std::sort(
            repeat.positions.begin(),
            repeat.positions.end()
        );

        repeats.push_back(repeat);
    }

    return repeats;
}