#ifndef ESA_HPP
#define ESA_HPP

#include <string>
#include <vector>

struct LCPInterval
{
    int lcp_value;
    int left;
    int right;
};

struct EnhancedSuffixArray
{
    std::string text;

    std::vector<int> suffix_array;
    std::vector<int> inverse_suffix_array;
    std::vector<int> lcp_array;

    std::string bwt;
};

EnhancedSuffixArray build_esa(const std::string& text);

std::vector<LCPInterval> build_lcp_intervals(
    const std::vector<int>& lcp_array
);

#endif