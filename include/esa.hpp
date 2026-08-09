#ifndef ESA_HPP
#define ESA_HPP

#include <string>
#include <vector>

struct EnhancedSuffixArray
{
    std::string text;

    std::vector<int> suffix_array;
    std::vector<int> inverse_suffix_array;
    std::vector<int> lcp_array;

    std::string bwt;
};

EnhancedSuffixArray build_esa(const std::string& text);

#endif