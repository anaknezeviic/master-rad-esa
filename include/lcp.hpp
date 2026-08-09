#ifndef LCP_HPP
#define LCP_HPP

#include <string>
#include <vector>

std::vector<int> build_lcp_array(
    const std::string& text,
    const std::vector<int>& suffix_array
);

#endif