#include "repeats.hpp"

#include <bitset>
#include <vector>
#include <algorithm>
#include <string>
#include <set>

namespace {

using LeftContextMask = std::bitset<256>;

bool has_distinct_left_contexts(
    const LeftContextMask& first,
    const LeftContextMask& second
) {
    if (!first.any() || !second.any()) {
        return false;
    }

    return (first | second).count() >= 2;
}

}

std::vector<Repeat> find_supermaximal_repeats(
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options
) {
    const std::vector<LCPIntervalNode> nodes =
        build_lcp_interval_tree(esa.lcp_array);

    return find_supermaximal_repeats(
        esa,
        options,
        nodes
    );
}

std::vector<Repeat> find_supermaximal_repeats(
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options,
    const std::vector<LCPIntervalNode>& nodes
) {
    std::vector<Repeat> repeats;

    for (const LCPIntervalNode& node : nodes) {
        if (node.lcp_value == 0) {
            continue;
        }

        if (node.lcp_value < options.min_length) {
            continue;
        }

        if (!node.children.empty()) {
            continue;
        }

        LeftContextMask left_contexts;
        bool left_contexts_are_distinct = true;

        for (int k = node.left; k <= node.right; ++k) {
            const unsigned char left_character =
                static_cast<unsigned char>(esa.bwt[k]);

            if (left_contexts.test(left_character)) {
                left_contexts_are_distinct = false;
                break;
            }

            left_contexts.set(left_character);
        }

        if (!left_contexts_are_distinct) {
            continue;
        }

        Repeat repeat;
        repeat.length = node.lcp_value;
        repeat.sequence = esa.text.substr(
            esa.suffix_array[node.left],
            node.lcp_value
        );

        repeat.positions.reserve(
            node.right - node.left + 1
        );

        for (int k = node.left; k <= node.right; ++k) {
            repeat.positions.push_back(
                esa.suffix_array[k]
            );
        }

        repeats.push_back(std::move(repeat));
    }

    return repeats;
}

std::vector<MaximalRepeatedPair> find_maximal_repeated_pairs(const EnhancedSuffixArray& esa, const RepeatOptions& options) {
    std::vector<MaximalRepeatedPair> pairs;

    const std::vector<LCPIntervalNode> nodes = build_lcp_interval_tree(esa.lcp_array);

    const int n = static_cast<int>(esa.text.size());

    for (const LCPIntervalNode& node : nodes) {        
        if (node.lcp_value == 0) {
            continue;
        }

        const int repeat_length = node.lcp_value;

        if (repeat_length < options.min_length) {
            continue;
        }

        std::vector<std::vector<int>> groups;
        int current = node.left;

        for (int child_index : node.children) {
            const LCPIntervalNode& child = nodes[child_index];

            while (current < child.left) {
                groups.push_back({esa.suffix_array[current]});
                ++current;
            }

            std::vector<int> child_positions;

            for (int k = child.left; k <= child.right; ++k) {
                child_positions.push_back( esa.suffix_array[k]);
            }

            groups.push_back(child_positions);
            current = child.right + 1;
        }

        while (current <= node.right) {
            groups.push_back({esa.suffix_array[current]});
            ++current;
        }

        for (std::size_t first_group = 0; first_group < groups.size(); ++first_group) {
            for (std::size_t second_group = first_group + 1; second_group < groups.size(); ++second_group) {
                for (int first_position : groups[first_group]) {
                    for (int second_position : groups[second_group]) {

                        const char first_left = (first_position == 0) ? '$' : esa.text[first_position - 1];
                        const char second_left = (second_position == 0) ? '$' : esa.text[second_position - 1];

                        if (first_left == second_left) {
                            continue;
                        }

                        int p1 = first_position;
                        int p2 = second_position;

                        if (p2 < p1) {
                            std::swap(p1, p2);
                        }

                        MaximalRepeatedPair pair;

                        pair.length = repeat_length;
                        pair.sequence = esa.text.substr(p1, repeat_length);
                        pair.first_position = p1;
                        pair.second_position = p2;
                        pairs.push_back(pair);
                    }
                }
            }
        }
    }

    return pairs;
}

std::vector<Repeat> find_maximal_repeats_baseline(const EnhancedSuffixArray& esa, const RepeatOptions& options) {
    const std::vector<MaximalRepeatedPair> pairs = find_maximal_repeated_pairs(esa, options);

    std::set<std::string> maximal_sequences;

    for (const MaximalRepeatedPair& pair : pairs) {
        maximal_sequences.insert(pair.sequence);
    }

    const std::vector<LCPIntervalNode> nodes = build_lcp_interval_tree(esa.lcp_array);

    std::vector<Repeat> repeats;

    for (const LCPIntervalNode& node : nodes) {
        if (node.lcp_value == 0) {
            continue;
        }

        if (node.lcp_value < options.min_length) {
            continue;
        }

        const std::string sequence = esa.text.substr(esa.suffix_array[node.left], node.lcp_value);

        if (maximal_sequences.find(sequence) == maximal_sequences.end()) {
            continue;
        }

        Repeat repeat;
        repeat.sequence = sequence;
        repeat.length = node.lcp_value;

        for (int k = node.left; k <= node.right; ++k) {
            repeat.positions.push_back(esa.suffix_array[k]);
        }

        std::sort(repeat.positions.begin(), repeat.positions.end());
        repeats.push_back(repeat);
    }

    return repeats;
}
std::vector<Repeat> find_maximal_repeats(
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options
) {
    const std::vector<LCPIntervalNode> nodes =
        build_lcp_interval_tree(esa.lcp_array);

    return find_maximal_repeats(
        esa,
        options,
        nodes
    );
}

std::vector<Repeat> find_maximal_repeats(
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options,
    const std::vector<LCPIntervalNode>& nodes
) {
    std::vector<Repeat> repeats;

    if (nodes.empty()) {
        return repeats;
    }

    std::vector<LeftContextMask> node_left_contexts(
        nodes.size()
    );

    for (std::size_t node_index = 0;
         node_index < nodes.size();
         ++node_index) {

        const LCPIntervalNode& node =
            nodes[node_index];

        LeftContextMask accumulated_contexts;
        LeftContextMask complete_node_contexts;

        bool is_maximal_repeat = false;

        int current = node.left;

        for (int child_index : node.children) {
            const LCPIntervalNode& child =
                nodes[child_index];

            while (current < child.left) {
                LeftContextMask group_contexts;

                const unsigned char left_character =
                    static_cast<unsigned char>(
                        esa.bwt[current]
                    );

                group_contexts.set(left_character);

                if (has_distinct_left_contexts(
                        accumulated_contexts,
                        group_contexts
                    )) {
                    is_maximal_repeat = true;
                }

                accumulated_contexts |= group_contexts;
                complete_node_contexts |= group_contexts;

                ++current;
            }

            const LeftContextMask& group_contexts =
                node_left_contexts[child_index];

            if (has_distinct_left_contexts(
                    accumulated_contexts,
                    group_contexts
                )) {
                is_maximal_repeat = true;
            }

            accumulated_contexts |= group_contexts;
            complete_node_contexts |= group_contexts;

            current = child.right + 1;
        }

        while (current <= node.right) {
            LeftContextMask group_contexts;

            const unsigned char left_character =
                static_cast<unsigned char>(
                    esa.bwt[current]
                );

            group_contexts.set(left_character);

            if (has_distinct_left_contexts(
                    accumulated_contexts,
                    group_contexts
                )) {
                is_maximal_repeat = true;
            }

            accumulated_contexts |= group_contexts;
            complete_node_contexts |= group_contexts;

            ++current;
        }

        node_left_contexts[node_index] =
            complete_node_contexts;

        if (node.lcp_value == 0) {
            continue;
        }

        if (node.lcp_value < options.min_length) {
            continue;
        }

        if (!is_maximal_repeat) {
            continue;
        }

        Repeat repeat;

        repeat.length = node.lcp_value;
        repeat.sequence = esa.text.substr(
            esa.suffix_array[node.left],
            node.lcp_value
        );

        repeat.positions.reserve(
            node.right - node.left + 1
        );

        for (int k = node.left;
             k <= node.right;
             ++k) {

            repeat.positions.push_back(
                esa.suffix_array[k]
            );
        }

        repeats.push_back(std::move(repeat));
    }

    return repeats;
}