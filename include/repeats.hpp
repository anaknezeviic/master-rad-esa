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

struct MaximalRepeatedPair
{
    std::string sequence;
    int length;

    int first_position;
    int second_position;
};

std::vector<Repeat> find_supermaximal_repeats(
    const EnhancedSuffixArray& esa
);

std::vector<MaximalRepeatedPair> find_maximal_repeated_pairs(
    const EnhancedSuffixArray& esa
);

#endif