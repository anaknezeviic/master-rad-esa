#ifndef REPEATS_HPP
#define REPEATS_HPP

#include "esa.hpp"

#include <string>
#include <vector>

struct RepeatOptions
{
    int min_length = 1;
};

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
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options
);

std::vector<MaximalRepeatedPair> find_maximal_repeated_pairs(
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options
);

std::vector<Repeat> find_maximal_repeats(
    const EnhancedSuffixArray& esa,
    const RepeatOptions& options
);

#endif