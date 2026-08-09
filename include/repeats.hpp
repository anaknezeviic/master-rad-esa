#ifndef REPEATS_HPP
#define REPEATS_HPP

#include "esa.hpp"

#include <string>
#include <vector>

struct Repeat
{
    std::string sequence;
    int length;
    std::vector<int> positions;
};

std::vector<Repeat> find_supermaximal_repeats(
    const EnhancedSuffixArray& esa
);

#endif