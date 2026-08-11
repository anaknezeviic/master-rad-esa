#ifndef FASTA_HPP
#define FASTA_HPP

#include <string>

struct FastaRecord
{
    std::string header;
    std::string sequence;
};

FastaRecord read_fasta(const std::string& file_path);

#endif