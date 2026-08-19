#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP

#include <string>
#include <vector>

std::vector<int> build_suffix_array_baseline(
    const std::string& text
);

std::vector<int> build_suffix_array_optimized(
    const std::string& text
);

std::vector<int> build_suffix_array_sais(
    const std::string& text
);

std::vector<int> build_suffix_array(
    const std::string& text
);


#endif