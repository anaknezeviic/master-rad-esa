#include "output.hpp"

#include <fstream>
#include <stdexcept>

void write_repeats_csv(const std::string& file_path, const std::vector<Repeat>& repeats) {
    
    std::ofstream output(file_path);

    if (!output) {
        throw std::runtime_error(
            "Could not open output file: " + file_path
        );
    }

    output << "sequence,length,occurrences,positions\n";

    for (const Repeat& repeat : repeats) {

        output
            << repeat.sequence
            << ','
            << repeat.length
            << ','
            << repeat.positions.size()
            << ",\"";

        for (std::size_t i = 0; i < repeat.positions.size(); ++i) {
            if (i > 0) {
                output << ' ';
            }
            output << repeat.positions[i];
        }

        output << "\"\n";
    }
}