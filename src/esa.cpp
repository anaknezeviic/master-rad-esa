#include "esa.hpp"

#include "lcp.hpp"
#include "suffix_array.hpp"

#include <stack>

EnhancedSuffixArray build_esa(const std::string& text)
{
    EnhancedSuffixArray esa;

    esa.text = text;

    esa.suffix_array = build_suffix_array(text);

    const int n = static_cast<int>(text.size());

    esa.inverse_suffix_array.resize(n);

    for (int i = 0; i < n; ++i) {
        esa.inverse_suffix_array[esa.suffix_array[i]] = i;
    }

    esa.lcp_array =
        build_lcp_array(text, esa.suffix_array);

    esa.bwt.resize(n);

    for (int i = 0; i < n; ++i) {

        const int suffix_position =
            esa.suffix_array[i];

        if (suffix_position == 0) {
            esa.bwt[i] = '$';
        } else {
            esa.bwt[i] = text[suffix_position - 1];
        }
    }

    return esa;
}

std::vector<LCPInterval> build_lcp_intervals(
    const std::vector<int>& lcp_array
)
{
    std::vector<LCPInterval> intervals;

    if (lcp_array.empty()) {
        return intervals;
    }

    struct StackEntry
    {
        int lcp_value;
        int left;
    };

    std::stack<StackEntry> stack;

    stack.push({0, 0});

    const int n = static_cast<int>(lcp_array.size());

    for (int i = 1; i <= n; ++i) {

        const int current_lcp =
            (i < n) ? lcp_array[i] : 0;

        int left = i - 1;

        while (
            !stack.empty() &&
            current_lcp < stack.top().lcp_value
        ) {
            const StackEntry interval_start =
                stack.top();

            stack.pop();

            intervals.push_back({
                interval_start.lcp_value,
                interval_start.left,
                i - 1
            });

            left = interval_start.left;
        }

        if (
            !stack.empty() &&
            current_lcp > stack.top().lcp_value
        ) {
            stack.push({
                current_lcp,
                left
            });
        }
    }

    return intervals;
}

std::vector<LCPIntervalNode> build_lcp_interval_tree(
    const std::vector<int>& lcp_array
)
{
    std::vector<LCPIntervalNode> nodes;

    if (lcp_array.empty()) {
        return nodes;
    }

    struct StackEntry
    {
        int lcp_value;
        int left;
        std::vector<int> children;
    };

    std::vector<StackEntry> stack;

    stack.push_back({
        0,
        0,
        {}
    });

    const int n =
        static_cast<int>(lcp_array.size());

    int last_interval_index = -1;

    for (int i = 1; i <= n; ++i) {

        const int current_lcp =
            (i < n)
            ? lcp_array[i]
            : 0;

        int left = i - 1;

        while (
            stack.size() > 1 &&
            current_lcp < stack.back().lcp_value
        )
        {
            StackEntry completed =
                stack.back();

            stack.pop_back();

            LCPIntervalNode node;

            node.lcp_value =
                completed.lcp_value;

            node.left =
                completed.left;

            node.right =
                i - 1;

            node.children =
                completed.children;

            nodes.push_back(node);

            last_interval_index =
                static_cast<int>(nodes.size()) - 1;

            left = completed.left;

            if (
                current_lcp <=
                stack.back().lcp_value
            )
            {
                stack.back().children.push_back(
                    last_interval_index
                );

                last_interval_index = -1;
            }
        }

        if (
            current_lcp >
            stack.back().lcp_value
        )
        {
            StackEntry new_interval;

            new_interval.lcp_value =
                current_lcp;

            new_interval.left =
                left;

            if (last_interval_index != -1) {
                new_interval.children.push_back(
                    last_interval_index
                );

                last_interval_index = -1;
            }

            stack.push_back(
                new_interval
            );
        }
    }

    LCPIntervalNode root;

    root.lcp_value = 0;
    root.left = 0;
    root.right = n - 1;
    root.children = stack.front().children;

    nodes.push_back(root);

    return nodes;
}