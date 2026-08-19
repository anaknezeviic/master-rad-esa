# Enhanced Suffix Arrays for Exact Repeats in DNA Sequences

Implementation developed as part of a Master's thesis on the application of Enhanced Suffix Arrays (ESA) for detecting exact repeats in DNA sequences.

The project focuses on the detection of:

- maximal repeats
- supermaximal repeats

The implementation is primarily based on the concepts described in:

> M. I. Abouelhoda, S. Kurtz, E. Ohlebusch,  
> "Replacing suffix trees with enhanced suffix arrays",  
> Journal of Discrete Algorithms, 2004.

The final suffix-array construction additionally uses the SA-IS algorithm described in:

> G. Nong, S. Zhang, W. H. Chan,  
> "Linear Suffix Array Construction by Almost Pure Induced-Sorting",  
> Data Compression Conference, 2009.

---

## 1. Project Overview

The program reads a DNA sequence from a FASTA file, constructs the data structures required for an Enhanced Suffix Array representation, and detects maximal and/or supermaximal exact repeats.

The implementation contains:

- three Suffix Array construction algorithms
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
- configurable benchmark support
- Python scripts for synthetic data generation, benchmark analysis, and visualization

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

The final ESA representation does not explicitly store a terminal suffix `$`.

For the SA-IS construction, the input is internally encoded with a unique lexicographically smallest sentinel. The sentinel suffix is removed before the resulting Suffix Array is returned to the rest of the program.

For the BWT, `$` is used as a special left-context symbol for an occurrence starting at position `0`.

For LCP interval processing, a virtual final LCP value of `0` is used to close remaining intervals.

---

## 3. Implemented Algorithms

### 3.1 Suffix Array

Three Suffix Array construction implementations are provided.

#### Baseline implementation

The baseline implementation uses the prefix-doubling approach together with `std::sort`.

Its purpose is mainly to provide a reference implementation for experimental comparison.

Approximate asymptotic complexity:

```text
O(n log² n)
```

#### Optimized prefix-doubling implementation

The optimized implementation also uses prefix doubling, but replaces the general comparison sort with counting-based sorting of equivalence classes.

Approximate asymptotic complexity:

```text
O(n log n)
```

#### SA-IS implementation

The final Suffix Array construction uses SA-IS (Suffix Array Induced Sorting).

The algorithm:

- classifies suffix positions as L-type or S-type;
- identifies LMS positions;
- performs induced sorting;
- assigns names to LMS substrings;
- recursively solves the reduced problem when necessary;
- induces the complete Suffix Array from the sorted LMS suffixes.

The original sequence is internally mapped to an integer alphabet and a unique sentinel value `0` is appended. After construction, the sentinel suffix is removed so that the external Suffix Array has the same size as the original sequence.

For an integer alphabet, the theoretical complexity is:

```text
O(n)
```

SA-IS is used by the final version of the program.

---

### 3.2 LCP Array

The LCP Array is constructed using the Kasai algorithm.

The implementation first constructs the inverse Suffix Array and then computes the LCP values in linear time.

Approximate complexity:

```text
O(n)
```

---

### 3.3 Supermaximal Repeats

Supermaximal repeats are detected using LCP intervals.

For a candidate LCP interval, the implementation checks:

1. whether the interval corresponds to a local maximum in the LCP Array;
2. whether the BWT characters representing the left contexts of all occurrences are pairwise distinct.

This follows the characterization of supermaximal repeats described by Abouelhoda et al.

---

### 3.4 Maximal Repeated Pairs

A reference implementation for maximal repeated pairs is included.

Occurrences inside an LCP interval are separated into groups representing different right contexts.

Pairs of occurrences from different groups are considered, and their left contexts are compared.

A pair is accepted when the two occurrences cannot be extended with the same character either to the left or to the right.

---

### 3.5 Maximal Repeats

Two implementations are available.

The baseline implementation first generates maximal repeated pairs and then derives distinct maximal repeat strings from them.

The final implementation avoids explicitly materializing all maximal repeated pairs.

Instead, it processes the LCP interval tree bottom-up and stores left-context sets using:

```cpp
std::bitset<256>
```

This allows the implementation to directly determine whether an LCP interval represents a maximal repeat.

The final program outputs distinct maximal repeat strings together with all their occurrence positions.

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

The benchmark-analysis and plotting scripts additionally require:

```text
pandas
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
│   ├── analyze_suffix_array.py
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

The `build/` directory is generated locally and is not required to be stored in the repository.

---

# 6. Building the Project

The following commands should be executed from the root directory of the project.

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
--implementation sais
--implementation final

--benchmark
--benchmark-name NAME
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

Four configurations are available.

## Baseline

```text
baseline prefix-doubling Suffix Array
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
optimized prefix-doubling Suffix Array
+
baseline maximal-repeat detection
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type maximal --implementation optimized-sa
```

---

## SA-IS

```text
SA-IS Suffix Array
+
baseline maximal-repeat detection
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type maximal --implementation sais
```

This configuration is used to compare the Suffix Array construction algorithms while keeping maximal-repeat detection unchanged.

---

## Final implementation

```text
SA-IS Suffix Array
+
optimized maximal-repeat detection
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type maximal --implementation final
```

All four configurations should produce the same maximal-repeat results.

They differ in the algorithms used internally and therefore in execution time.

This separation allows the effects of Suffix Array construction and maximal-repeat optimization to be evaluated independently.

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

`N` is accepted as a regular literal sequence symbol. It is not interpreted as a biological wildcard.

The following are rejected:

- missing FASTA header;
- sequence before the header;
- more than one FASTA record;
- empty sequence;
- symbols outside `A`, `C`, `G`, `T`, `N`.

---

# 11. Repeat Output

When benchmark mode is not enabled, detected repeats are printed to the terminal and written to CSV files.

The CSV format is:

```text
sequence,length,occurrences,positions
```

Example:

```text
ACGT,4,2,"0 4"
```

The `positions` field contains zero-based positions in the original DNA sequence.

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

This value estimates the memory occupied by the main persistent ESA structures:

```text
input text
Suffix Array
Inverse Suffix Array
LCP Array
BWT
```

It does not represent the complete memory consumption of the process.

## Peak process memory

On Windows, peak process memory is measured using the process peak working set.

This includes memory used by the complete running application and is therefore larger than the ESA-only estimate.

---

# 14. Benchmark Mode

Benchmark mode suppresses detailed repeat output and writes performance measurements to CSV.

Enable it with:

```text
--benchmark
```

Example:

```powershell
.\build\Release\master_rad_esa.exe data\raw\synthetic_10k.fasta --min-length 10 --type both --implementation final --benchmark
```

By default, synthetic benchmark data is written to:

```text
data/processed/benchmarks/synthetic_scalability.csv
```

By default, datasets located under:

```text
data/raw/real/
```

are written to:

```text
data/processed/benchmarks/real_datasets.csv
```

A custom benchmark file can be selected without changing the source code by using:

```text
--benchmark-name NAME
```

For example:

```powershell
.\build\Release\master_rad_esa.exe data\raw\synthetic_100k.fasta --min-length 10 --type maximal --implementation sais --benchmark --benchmark-name suffix_array_comparison
```

This writes to:

```text
data/processed/benchmarks/suffix_array_comparison.csv
```

The `.csv` extension and benchmark directory are added automatically.

`--benchmark-name` is intended to be used together with `--benchmark`.

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

When `--type both` is used, one row is written for maximal repeats and another for supermaximal repeats.

`total_time_ms` represents the complete execution of the program and is therefore the same measurement for both rows of the same run.

For direct comparison of maximal and supermaximal detection algorithms, `repeat_detection_time_ms` should be used.

For direct comparison of Suffix Array construction algorithms, `suffix_array_time_ms` should be used.

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

Using the same length and seed produces the same synthetic sequence, allowing the experiments to be reproduced.

For the scalability experiments, synthetic sequences of the following lengths were used:

```text
10,000
50,000
100,000
250,000
500,000
```

---

# 16. Running a Synthetic Benchmark

For example, to benchmark the final implementation on the 10,000-base synthetic dataset:

```powershell
.\build\Release\master_rad_esa.exe data\raw\synthetic_10k.fasta --min-length 10 --type both --implementation final --benchmark
```

The expected repeat counts for this dataset are:

```text
Maximal repeats:       24
Supermaximal repeats:  24
```

For experimental measurements, each configuration is executed multiple times and median values are used in the analysis.

For the Suffix Array comparison, the following configurations are evaluated independently:

```text
baseline
optimized-sa
sais
```

while keeping maximal-repeat detection in its baseline form.

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

For the provided SARS-CoV-2 sequence, the expected counts with minimum repeat length `10` are:

```text
Maximal repeats:       804
Supermaximal repeats:  749
```

---

# 18. Real Datasets Used in the Experimental Evaluation

The experimental evaluation includes several publicly available genomic sequences.

The current dataset collection includes:

```text
SARS-CoV-2 Wuhan-Hu-1
Mycoplasmoides genitalium G37
Bacillus subtilis 168
Escherichia coli K-12 MG1655
```

The real-data benchmark results for the final implementation are stored in:

```text
data/processed/benchmarks/real_datasets.csv
```

The dedicated Suffix Array comparison is stored in:

```text
data/processed/benchmarks/real_suffix_array_comparison.csv
```

---

# 19. Python Environment for Plot Generation

If a Python virtual environment is desired, it can be created from the project root.

On Windows:

```powershell
python -m venv .venv
```

Activate it with:

```powershell
.\.venv\Scripts\Activate.ps1
```

Install the required packages:

```powershell
python -m pip install pandas matplotlib
```

The prompt should then normally show the active environment, for example:

```text
(.venv) PS C:\...\master-rad-esa>
```

---

# 20. Generating Benchmark Plots

The general benchmark plotting script is:

```text
scripts/plot_benchmarks.py
```

Run it from the project root:

```powershell
python scripts\plot_benchmarks.py
```

It reads the existing optimization and scalability benchmark files and generates:

```text
01_suffix_array_optimization.png
02_maximal_repeat_optimization.png
03_total_runtime_comparison.png
04_runtime_scalability.png
05_esa_components.png
06_memory_scalability.png
07_repeat_counts.png
```

The plots are stored in:

```text
data/processed/plots/
```

The script additionally writes the calculated speedup summary to the benchmark results directory.

---

# 21. Suffix Array Benchmark Analysis

The dedicated Suffix Array analysis script is:

```text
scripts/analyze_suffix_array.py
```

Run:

```powershell
python scripts\analyze_suffix_array.py
```

The script reads:

```text
data/processed/benchmarks/suffix_array_comparison.csv
data/processed/benchmarks/real_suffix_array_comparison.csv
```

It:

- verifies that all compared implementations produce consistent repeat counts;
- calculates median values across repeated runs;
- calculates SA-IS speedup relative to the baseline and optimized prefix-doubling implementations;
- writes summary CSV files;
- generates synthetic and real-data comparison plots.

The generated summary files are:

```text
data/processed/benchmarks/suffix_array_synthetic_summary.csv
data/processed/benchmarks/suffix_array_real_summary.csv
```

The generated plots are:

```text
08_suffix_array_scalability.png
09_suffix_array_real_datasets.png
10_suffix_array_synthetic_speedup.png
11_suffix_array_real_speedup.png
```

The synthetic construction-time plot shows the scalability of all three Suffix Array implementations.

The real-data construction-time plot uses a logarithmic y-axis because the datasets span a wide range of sequence lengths and construction times.

The speedup plots compare SA-IS directly against the optimized prefix-doubling implementation. A speedup value greater than `1` means that SA-IS is faster.

---

# 22. Experimental Methodology

The experimental evaluation contains three complementary parts.

## Synthetic scalability experiments

Synthetic data is used for controlled scalability experiments.

The sequence length is gradually increased while keeping the sequence generation method reproducible.

The experiments measure:

- Suffix Array construction time
- ESA construction time
- maximal-repeat detection time
- supermaximal-repeat detection time
- total runtime
- estimated ESA memory
- peak process memory
- number of detected repeats

## Suffix Array construction comparison

Three Suffix Array construction algorithms are compared:

```text
baseline prefix-doubling        O(n log² n)
optimized prefix-doubling       O(n log n)
SA-IS                           O(n)
```

The comparison uses both synthetic and real genomic datasets.

To isolate the effect of Suffix Array construction, the `baseline`, `optimized-sa`, and `sais` configurations all use the same baseline maximal-repeat detection algorithm.

Each configuration is executed five times and the median `suffix_array_time_ms` value is used.

The experiments show that the optimized prefix-doubling implementation can be faster on smaller inputs because of lower constant overhead, while SA-IS becomes increasingly advantageous on larger inputs.

## Real genomic datasets

Publicly available DNA sequences are used to evaluate the practical behavior of the implementation on real biological data.

For performance evaluation, the Release build is used.

Repeated measurements are summarized using median values to reduce the effect of runtime noise.

---

# 23. Validation

The implementation was validated using small sequences for which the expected Suffix Arrays and repeats can be manually determined.

The three Suffix Array implementations were directly compared on:

```text
BANANA
MISSISSIPPI
AAAAA
A
empty input
ACGTACGTGATTACANN
```

For all tested inputs:

```text
baseline Suffix Array
=
optimized prefix-doubling Suffix Array
=
SA-IS Suffix Array
```

The complete program was additionally tested using:

```text
data/raw/tests/test.fasta
```

For the provided test sequence with minimum repeat length `2`, all four implementation configurations produce the same maximal repeats:

```text
ACGT
AC
TAC
```

The Suffix Array benchmark analysis also verifies that repeat counts remain consistent between the compared implementations.

This provides a regression check that the performance optimizations do not change the detected maximal-repeat set.

---

# 24. Notes

The project focuses on exact direct repeats.

It does not currently search for:

- approximate repeats
- reverse-complement repeats
- repeats with mismatches or indels

The FASTA parser supports one DNA record at a time and the alphabet:

```text
A C G T N
```

`N` is accepted as a literal symbol and has no special wildcard semantics.

---

# 25. References

M. I. Abouelhoda, S. Kurtz, E. Ohlebusch,  
"Replacing suffix trees with enhanced suffix arrays",  
Journal of Discrete Algorithms, Volume 2, Issue 1, 2004, pp. 53-86.

G. Nong, S. Zhang, W. H. Chan,  
"Linear Suffix Array Construction by Almost Pure Induced-Sorting",  
Data Compression Conference, 2009.

The first paper provides the theoretical basis for the Enhanced Suffix Array structures and the LCP-interval-based repeat analysis used in this project.

The second paper provides the basis for the linear-time SA-IS Suffix Array construction used by the final implementation.
