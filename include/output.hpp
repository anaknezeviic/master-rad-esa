#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "repeats.hpp"

#include <string>
#include <vector>

void write_repeats_csv(
    const std::string& file_path,
    const std::vector<Repeat>& repeats
);

#endif