# Enhanced Suffix Arrays for Exact Repeats in DNA Sequences

Implementation developed as part of a Master's thesis on the application of
Enhanced Suffix Arrays (ESA) for detecting exact repeats in DNA sequences.

The project focuses on the detection of:

- maximal repeats
- supermaximal repeats

The implementation is primarily based on the concepts described in:

> M. I. Abouelhoda, S. Kurtz, E. Ohlebusch,  
> "Replacing suffix trees with enhanced suffix arrays",  
> Journal of Discrete Algorithms, 2004.

---

## 1. Project Overview

The program reads a DNA sequence from a FASTA file, constructs the data
structures required for an Enhanced Suffix Array representation, and detects
maximal and/or supermaximal exact repeats.

The implementation contains:

- Suffix Array construction
- Inverse Suffix Array construction
- LCP Array construction
- Burrows-Wheeler Transform (BWT)
- LCP interval construction
- LCP interval tree construction
- maximal repeated pair detection
- maximal repeat detection
- supermaximal repeat detection
- FASTA parsing
- CSV output
- runtime measurements
- memory measurements
- benchmark support
- Python scripts for synthetic data generation and benchmark visualization

The final implementation is written in C++17.

---

## 2. Main Data Structures

The `EnhancedSuffixArray` structure contains:

```text
text
suffix_array
inverse_suffix_array
lcp_array
bwt
```

The implementation does not explicitly append the terminal suffix `$` to the
input sequence.

Instead, sequence boundaries are handled implicitly.

For the BWT, `$` is used as a special left-context symbol for an occurrence
starting at position `0`.

For LCP interval processing, a virtual final LCP value of `0` is used to close
remaining intervals.

---

## 3. Implemented Algorithms

### 3.1 Suffix Array

Two Suffix Array construction implementations are provided.

#### Baseline implementation

The baseline implementation uses the prefix-doubling approach together with
`std::sort`.

Its purpose is mainly to provide a reference implementation for the
experimental comparison.

Approximate asymptotic complexity:

```text
O(n log² n)
```

#### Optimized implementation

The optimized implementation also uses prefix doubling, but replaces the
general comparison sort with counting-based sorting of equivalence classes.

Approximate asymptotic complexity:

```text
O(n log n)
```

The optimized implementation is used by the final version of the program.

---

### 3.2 LCP Array

The LCP Array is constructed using the Kasai algorithm.

The implementation first constructs the inverse Suffix Array and then computes
the LCP values in linear time.

Approximate complexity:

```text
O(n)
```

---

### 3.3 Supermaximal Repeats

Supermaximal repeats are detected using LCP intervals.

For a candidate LCP interval, the implementation checks:

1. whether the interval corresponds to a local maximum in the LCP Array;
2. whether the BWT characters representing the left contexts of all
   occurrences are pairwise distinct.

This follows the characterization of supermaximal repeats described by
Abouelhoda et al.

---

### 3.4 Maximal Repeated Pairs

A reference implementation for maximal repeated pairs is included.

Occurrences inside an LCP interval are separated into groups representing
different right contexts.

Pairs of occurrences from different groups are considered, and their left
contexts are compared.

A pair is accepted when the two occurrences cannot be extended with the same
character either to the left or to the right.

---

### 3.5 Maximal Repeats

Two implementations are available.

The baseline implementation first generates maximal repeated pairs and then
derives distinct maximal repeat strings from them.

The final implementation avoids explicitly materializing all maximal repeated
pairs.

Instead, it processes the LCP interval tree bottom-up and stores left-context
sets using:

```cpp
std::bitset<256>
```

This allows the implementation to directly determine whether an LCP interval
represents a maximal repeat.

The final program outputs distinct maximal repeat strings together with all
their occurrence positions.

---

## 4. Requirements

### C++ application

The following are required:

- C++17 compatible compiler
- CMake 3.20 or newer

The project was developed and tested on Windows using Visual Studio / MSVC.

### Python scripts

Python is only required for:

- synthetic DNA generation
- benchmark processing
- plot generation

Required Python version:

```text
Python 3
```

The plotting script additionally requires:

```text
matplotlib
```

---

## 5. Project Structure

```text
master-rad-esa/
│
├── CMakeLists.txt
├── README.md
│
├── include/
│   ├── benchmark.hpp
│   ├── esa.hpp
│   ├── fasta.hpp
│   ├── lcp.hpp
│   ├── memory.hpp
│   ├── output.hpp
│   ├── repeats.hpp
│   └── suffix_array.hpp
│
├── src/
│   ├── benchmark.cpp
│   ├── esa.cpp
│   ├── fasta.cpp
│   ├── lcp.cpp
│   ├── main.cpp
│   ├── memory.cpp
│   ├── output.cpp
│   ├── repeats.cpp
│   └── suffix_array.cpp
│
├── scripts/
│   ├── generate_dna.py
│   └── plot_benchmarks.py
│
├── data/
│   ├── raw/
│   │   ├── real/
│   │   ├── tests/
│   │   └── synthetic_*.fasta
│   │
│   └── processed/
│       ├── benchmarks/
│       ├── plots/
│       └── repeats/
│           ├── real/
│           ├── synthetic/
│           └── tests/
│
└── build/
```

The `build/` directory is generated locally and is not required to be stored
in the repository.

---

# 6. Building the Project

The following commands should be executed from the root directory of the
project.

For example:

```text
C:\Users\<username>\...\master-rad-esa
```

## 6.1 Configure CMake

The first time the project is built, run:

```powershell
cmake -S . -B build
```

This creates the CMake build directory.

---

## 6.2 Build the Release Version

Run:

```powershell
cmake --build build --config Release
```

After a successful build, the executable should be available at:

```text
build\Release\master_rad_esa.exe
```

On Windows PowerShell it can therefore be executed as:

```powershell
.\build\Release\master_rad_esa.exe
```

For performance measurements, the `Release` configuration should be used.

---

# 7. Running the Program

The general syntax is:

```powershell
.\build\Release\master_rad_esa.exe <fasta_file> [options]
```

Available options are:

```text
--min-length N

--type maximal
--type supermaximal
--type both

--implementation baseline
--implementation optimized-sa
--implementation final

--benchmark
```

If no repeat type is specified, the default is:

```text
both
```

If no minimum length is specified, the default is:

```text
1
```

If no implementation is specified, the default is:

```text
final
```

---

# 8. Quick Functional Test

A small test sequence is included in:

```text
data/raw/tests/test.fasta
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type both --implementation final
```

The test sequence contains 17 nucleotides.

The expected maximal repeats are:

```text
ACGT    length 4    positions 0 4
AC      length 2    positions 0 4 12
TAC     length 3    positions 3 11
```

The expected supermaximal repeats are:

```text
ACGT    length 4    positions 0 4
TAC     length 3    positions 3 11
```

Therefore, the expected number of detected repeats is:

```text
Maximal repeats:       3
Supermaximal repeats:  2
```

The program also creates:

```text
data/processed/repeats/tests/test_maximal.csv
data/processed/repeats/tests/test_supermaximal.csv
```

---

# 9. Comparing Implementations

Three configurations are available.

## Baseline

```text
baseline Suffix Array
+
baseline maximal-repeat detection
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type maximal --implementation baseline
```

---

## Optimized Suffix Array

```text
optimized Suffix Array
+
baseline maximal-repeat detection
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type maximal --implementation optimized-sa
```

---

## Final implementation

```text
optimized Suffix Array
+
optimized maximal-repeat detection
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type maximal --implementation final
```

All three configurations should produce the same maximal-repeat results.

They differ in the algorithms used internally and therefore in execution time.

This separation is used for the optimization experiments.

---

# 10. FASTA Input Format

The program expects a FASTA file containing exactly one DNA record.

Example:

```text
>example_sequence
ACGTACGTGATTACANN
```

Supported nucleotide symbols are:

```text
A
C
G
T
N
```

Lowercase characters are automatically converted to uppercase.

Whitespace inside sequence lines is ignored.

`N` is treated as a regular sequence symbol.

The following are rejected:

- missing FASTA header;
- sequence before the header;
- more than one FASTA record;
- empty sequence;
- symbols outside `A`, `C`, `G`, `T`, `N`.

---

# 11. Repeat Output

When benchmark mode is not enabled, detected repeats are printed to the
terminal and written to CSV files.

The CSV format is:

```text
sequence,length,occurrences,positions
```

Example:

```text
ACGT,4,2,"0 4"
```

The `positions` field contains zero-based positions in the original DNA
sequence.

Depending on the input file, output is automatically placed in one of:

```text
data/processed/repeats/tests/
data/processed/repeats/synthetic/
data/processed/repeats/real/
```

For example:

```text
data/raw/real/ecoli_k12.fasta
```

produces:

```text
data/processed/repeats/real/ecoli_k12_maximal.csv
data/processed/repeats/real/ecoli_k12_supermaximal.csv
```

---

# 12. Performance Output

After each run, the program prints a performance summary.

Example fields include:

```text
FASTA parsing
ESA construction
Suffix Array
Inverse Suffix Array
LCP Array
BWT
ESA estimated memory
Peak process memory
Maximal repeat detection
Supermaximal repeat detection
Total runtime
```

All execution times are reported in milliseconds.

---

# 13. Memory Measurements

Two different memory measurements are reported.

## ESA estimated memory

This value estimates the memory occupied by the main persistent ESA
structures:

```text
input text
Suffix Array
Inverse Suffix Array
LCP Array
BWT
```

It does not represent the complete memory consumption of the process.

## Peak process memory

On Windows, peak process memory is measured using the process peak working
set.

This includes memory used by the complete running application and is therefore
larger than the ESA-only estimate.

---

# 14. Benchmark Mode

Benchmark mode suppresses detailed repeat output and writes performance
measurements to CSV.

Enable it with:

```text
--benchmark
```

Example:

```powershell
.\build\Release\master_rad_esa.exe data\raw\synthetic_10k.fasta --min-length 10 --type both --implementation final --benchmark
```

For synthetic datasets, benchmark data is written to:

```text
data/processed/benchmarks/synthetic_scalability.csv
```

For datasets located under:

```text
data/raw/real/
```

benchmark data is written to:

```text
data/processed/benchmarks/real_datasets.csv
```

The benchmark CSV contains the following columns:

```text
dataset
implementation
sequence_length
min_length
repeat_type
fasta_time_ms
suffix_array_time_ms
inverse_suffix_array_time_ms
lcp_time_ms
bwt_time_ms
esa_time_ms
repeat_detection_time_ms
total_time_ms
esa_memory_bytes
peak_memory_bytes
repeat_count
```

When `--type both` is used, one row is written for maximal repeats and another
for supermaximal repeats.

`total_time_ms` represents the complete execution of the program and is
therefore the same measurement for both rows of the same run.

For direct comparison of maximal and supermaximal detection algorithms,
`repeat_detection_time_ms` should be used.

---

# 15. Synthetic DNA Generation

Synthetic DNA sequences can be generated using:

```text
scripts/generate_dna.py
```

Example:

```powershell
python scripts\generate_dna.py --length 10000 --seed 42 --output data\raw\synthetic_10k.fasta
```

The generated DNA sequence contains uniformly selected characters from:

```text
A C G T
```

The default random seed is:

```text
42
```

Using the same length and seed produces the same synthetic sequence, allowing
the experiments to be reproduced.

For the scalability experiments, synthetic sequences of the following lengths
were used:

```text
10,000
50,000
100,000
250,000
500,000
```

---

# 16. Running a Synthetic Benchmark

For example, to benchmark the final implementation on the 10,000-base
synthetic dataset:

```powershell
.\build\Release\master_rad_esa.exe data\raw\synthetic_10k.fasta --min-length 10 --type both --implementation final --benchmark
```

The expected repeat counts for this dataset are:

```text
Maximal repeats:       24
Supermaximal repeats:  24
```

For experimental measurements, each configuration can be executed multiple
times and median values can then be used in the analysis.

---

# 17. Running a Real DNA Dataset

Real genomic sequences are stored in:

```text
data/raw/real/
```

For example, SARS-CoV-2 can be processed with:

```powershell
.\build\Release\master_rad_esa.exe data\raw\real\sars_cov_2.fasta --min-length 10 --type both --implementation final
```

To run it in benchmark mode:

```powershell
.\build\Release\master_rad_esa.exe data\raw\real\sars_cov_2.fasta --min-length 10 --type both --implementation final --benchmark
```

For the provided SARS-CoV-2 sequence, the expected counts with minimum repeat
length `10` are:

```text
Maximal repeats:       804
Supermaximal repeats:  749
```

---

# 18. Real Datasets Used in the Experimental Evaluation

The experimental evaluation includes several publicly available genomic
sequences.

The current dataset collection includes:

```text
SARS-CoV-2 Wuhan-Hu-1
Mycoplasmoides genitalium G37
Bacillus subtilis 168
Escherichia coli K-12 MG1655
```

The real-data benchmark results are stored in:

```text
data/processed/benchmarks/real_datasets.csv
```

---

# 19. Python Environment for Plot Generation

If a Python virtual environment is desired, it can be created from the project
root.

On Windows:

```powershell
python -m venv .venv
```

Activate it with:

```powershell
.\.venv\Scripts\Activate.ps1
```

Install Matplotlib:

```powershell
python -m pip install matplotlib
```

The prompt should then normally show the active environment, for example:

```text
(.venv) PS C:\...\master-rad-esa>
```

---

# 20. Generating Benchmark Plots

The plotting script is:

```text
scripts/plot_benchmarks.py
```

Run it from the project root:

```powershell
python scripts\plot_benchmarks.py
```

The script reads:

```text
data/processed/benchmarks/optimization_comparison.csv
data/processed/benchmarks/synthetic_scalability.csv
```

and calculates median values across repeated benchmark runs.

The generated plots are stored in:

```text
data/processed/plots/
```

The generated plots are:

```text
01_suffix_array_optimization.png
02_maximal_repeat_optimization.png
03_total_runtime_comparison.png
04_runtime_scalability.png
05_esa_components.png
06_memory_scalability.png
07_repeat_counts.png
```

The script additionally writes the calculated speedup summary to the benchmark
results directory.

---

# 21. Experimental Methodology

The experimental evaluation consists of two main parts.

## Synthetic datasets

Synthetic data is used for controlled scalability experiments.

The sequence length is gradually increased while keeping the sequence
generation method reproducible.

The experiments measure:

- Suffix Array construction time
- ESA construction time
- maximal-repeat detection time
- supermaximal-repeat detection time
- total runtime
- estimated ESA memory
- peak process memory
- number of detected repeats

## Real genomic datasets

Publicly available DNA sequences are used to evaluate the practical behavior
of the implementation on real biological data.

The experiments measure the same runtime and memory properties as the
synthetic experiments.

For performance evaluation, the Release build is used.

Repeated measurements are summarized using median values to reduce the effect
of runtime noise.

---

# 22. Validation

The implementation was validated using small sequences for which the expected
repeats can be manually determined.

Additionally, the baseline and optimized implementations were compared on the
same inputs.

For the provided test sequence with minimum repeat length `2`, all three
implementation configurations produce the same maximal repeats:

```text
ACGT
AC
TAC
```

This provides a regression check that the performance optimizations do not
change the detected maximal-repeat set.

---

# 23. Notes

The project focuses on exact direct repeats.

It does not currently search for:

- approximate repeats
- reverse-complement repeats
- repeats with mismatches or indels

The FASTA parser supports one DNA record at a time and the alphabet:

```text
A C G T N
```

---

# 24. Reference

M. I. Abouelhoda, S. Kurtz, E. Ohlebusch,  
"Replacing suffix trees with enhanced suffix arrays",  
Journal of Discrete Algorithms, Volume 2, Issue 1, 2004, pp. 53-86.

The paper provides the theoretical basis for the Enhanced Suffix Array
structures and the LCP-interval-based repeat analysis used in this project.