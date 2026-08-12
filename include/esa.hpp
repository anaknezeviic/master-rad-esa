#ifndef ESA_HPP
#define ESA_HPP

#include <string>
#include <vector>

struct ESAConstructionMetrics
{
    double suffix_array_time_ms = 0.0;
    double inverse_suffix_array_time_ms = 0.0;
    double lcp_time_ms = 0.0;
    double bwt_time_ms = 0.0;
};

struct LCPInterval
{
    int lcp_value;
    int left;
    int right;
};

struct LCPIntervalNode
{
    int lcp_value;
    int left;
    int right;

    std::vector<int> children;
};

struct EnhancedSuffixArray
{
    std::string text;

    std::vector<int> suffix_array;
    std::vector<int> inverse_suffix_array;
    std::vector<int> lcp_array;

    std::string bwt;
};

EnhancedSuffixArray build_esa(
    const std::string& text,
    ESAConstructionMetrics* metrics = nullptr
);

std::vector<LCPInterval> build_lcp_intervals(
    const std::vector<int>& lcp_array
);

std::vector<LCPIntervalNode> build_lcp_interval_tree(
    const std::vector<int>& lcp_array
);

#endif