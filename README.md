# Enhanced Suffix Arrays for Exact Repeats in DNA Sequences

Implementation developed as part of a Master's thesis on the application of Enhanced Suffix Arrays (ESA) for detecting exact repeats in DNA sequences.

The project focuses on the detection of:

- maximal repeats
- supermaximal repeats

The implementation is primarily based on the ESA concepts described by Abouelhoda, Kurtz, and Ohlebusch:

> M. I. Abouelhoda, S. Kurtz, E. Ohlebusch,  
> "Replacing suffix trees with enhanced suffix arrays",  
> Journal of Discrete Algorithms, 2004.

The final Suffix Array construction uses the SA-IS algorithm described by Nong, Zhang, and Chan:

> G. Nong, S. Zhang, W. H. Chan,  
> "Linear Suffix Array Construction by Almost Pure Induced-Sorting",  
> Data Compression Conference, 2009.

The optimized repeat-detection design is additionally motivated by linear-time ESA-based processing of context-sensitive repeats:

> E. Ohlebusch, T. Beller,  
> "Alphabet-independent algorithms for finding context-sensitive repeats in linear time",  
> Journal of Discrete Algorithms, 2015.

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
- maximal repeated-pair reference implementation
- baseline maximal-repeat detection
- optimized maximal-repeat detection
- optimized supermaximal-repeat detection
- FASTA parsing
- CSV output
- runtime measurements
- memory measurements
- configurable benchmark support
- automated CTest regression tests
- synthetic and real-genome benchmark analysis
- Python scripts for data generation, analysis, and visualization

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

The external ESA representation does not explicitly store a terminal suffix `$`.

For SA-IS, the input is internally mapped to an integer alphabet and a unique lexicographically smallest sentinel value `0` is appended. The sentinel suffix is removed before the Suffix Array is returned to the rest of the application.

For the BWT, `$` is used as a special left-context symbol for an occurrence starting at position `0`.

For LCP interval processing, a virtual final LCP value of `0` closes the remaining intervals.

---

## 3. Implemented Algorithms

### 3.1 Suffix Array

Three Suffix Array construction implementations are provided.

#### Baseline prefix-doubling

The baseline implementation uses prefix doubling together with `std::sort`.

Its purpose is to provide a simple reference implementation for experimental comparison.

Approximate asymptotic complexity:

```text
O(n log² n)
```

#### Optimized prefix-doubling

The optimized prefix-doubling implementation replaces the general comparison sort with counting-based sorting of equivalence classes.

Approximate asymptotic complexity:

```text
O(n log n)
```

#### SA-IS

The final Suffix Array construction uses SA-IS (Suffix Array Induced Sorting).

The implementation:

- classifies positions as L-type or S-type;
- identifies LMS positions;
- performs induced sorting;
- assigns names to LMS substrings;
- recursively solves the reduced problem when necessary;
- induces the complete Suffix Array from the sorted LMS suffixes.

For an integer alphabet, the theoretical construction complexity is:

```text
O(n)
```

SA-IS is used by both the `sais` and `final` configurations.

---

### 3.2 Inverse Suffix Array and LCP Array

The Inverse Suffix Array is constructed once during ESA construction.

The LCP Array is then constructed using the Kasai algorithm and reuses the already available Inverse Suffix Array instead of rebuilding it internally.

Approximate complexity:

```text
Inverse Suffix Array: O(n)
Kasai LCP Array:      O(n)
```

This avoids one redundant linear pass and one redundant temporary allocation.

---

### 3.3 Burrows-Wheeler Transform

The BWT is constructed directly from the Suffix Array.

For a suffix beginning at position `p`:

- if `p > 0`, its BWT character is `text[p - 1]`;
- if `p == 0`, its BWT character is `$`.

Approximate complexity:

```text
O(n)
```

The BWT is used to represent left contexts of repeat occurrences.

---

### 3.4 LCP Intervals and LCP Interval Tree

LCP intervals are derived from the LCP Array using a stack-based traversal.

The LCP interval tree is also constructed with a stack. Each interval is inserted only after its internal children have already been completed.

Therefore, nodes are stored in bottom-up order:

```text
child index < parent index
```

The repeat-detection implementation relies on this ordering when propagating context information from children to parents.

Approximate complexity:

```text
Time:  O(n)
Space: O(n)
```

---

### 3.5 Supermaximal Repeats

The optimized supermaximal-repeat algorithm processes the LCP interval tree.

A candidate must satisfy two conditions:

1. the corresponding LCP interval is a leaf of the LCP interval tree, which rules out a repeated common right extension;
2. all BWT characters in the interval are pairwise distinct, which rules out a repeated common left extension.

Left-context distinctness is checked using:

```cpp
std::bitset<256>
```

instead of a tree-based set.

For the fixed byte alphabet used by the implementation, context-mask operations have constant cost with respect to the sequence length.

If `z_s` denotes the total number of reported occurrence positions for supermaximal repeats, the final implementation is analyzed as:

```text
Classification: O(n)
Reporting:      O(z_s)
Total:          O(n + z_s)
```

---

### 3.6 Maximal Repeated Pairs

A reference implementation for maximal repeated pairs is retained for comparison and validation.

Occurrences belonging to different right-context groups are explicitly paired and their left contexts are compared.

Because the implementation materializes occurrence pairs, its running time is output-sensitive and can become quadratic in unfavorable cases.

This implementation is intentionally retained as part of the baseline maximal-repeat approach.

---

### 3.7 Baseline Maximal Repeats

The baseline maximal-repeat implementation:

1. explicitly generates maximal repeated pairs;
2. collects distinct repeated strings from those pairs;
3. reconstructs their full occurrence lists from the LCP interval tree.

This implementation serves as the reference algorithm in repeat-detection benchmarks.

---

### 3.8 Optimized Maximal Repeats

The final maximal-repeat implementation avoids explicitly materializing maximal repeated pairs.

It processes the LCP interval tree bottom-up and propagates left-context information through:

```cpp
std::bitset<256>
```

For each LCP node:

- internal children represent groups with a common longer right extension;
- uncovered Suffix Array positions represent direct leaf groups;
- BWT-derived context masks represent possible left extensions;
- context masks are combined bottom-up;
- a repeat is classified as maximal when distinct left contexts occur across distinct right-context groups.

Child intervals are already stored in Suffix Array order, so no per-node child sorting is required.

Occurrence positions are reported directly in Suffix Array order. Their numeric order is not part of the repeat definition.

If `z` denotes the total number of reported occurrence positions for maximal repeats:

```text
Classification: O(n)
Reporting:      O(z)
Total:          O(n + z)
```

Auxiliary structural memory is linear in `n`; output storage is additionally proportional to the reported results.

The implementation follows the same general ESA-based design principle as linear-time context-sensitive repeat algorithms: process LCP structure bottom-up, use left-context information from the BWT, and avoid explicit enumeration of all repeated pairs. It is not intended as a line-by-line reproduction of the Ohlebusch-Beller pseudocode.

---

### 3.9 Shared Repeat Structure

When the final implementation is executed with:

```text
--type both
```

the LCP interval tree is constructed once and reused by both maximal and supermaximal repeat detection.

This avoids rebuilding the same repeat-processing structure twice.

---

## 4. Complexity Summary

For the final implementation:

| Component | Time | Auxiliary / persistent space |
|---|---:|---:|
| SA-IS | `O(n)` | `O(n)` |
| Inverse Suffix Array | `O(n)` | `O(n)` |
| Kasai LCP Array | `O(n)` | `O(n)` |
| BWT | `O(n)` | `O(n)` |
| LCP interval tree | `O(n)` | `O(n)` |
| Maximal-repeat classification | `O(n)` | `O(n)` |
| Maximal-repeat reporting | `O(z)` | output-dependent |
| Maximal detection + reporting | `O(n + z)` | `O(n)` + output |
| Supermaximal classification | `O(n)` | `O(n)` |
| Supermaximal reporting | `O(z_s)` | output-dependent |
| Supermaximal detection + reporting | `O(n + z_s)` | `O(n)` + output |

Where:

```text
n   = input sequence length
z   = total reported occurrence positions for maximal repeats
z_s = total reported occurrence positions for supermaximal repeats
```

---

## 5. Requirements

### C++ application

Required:

- C++17 compatible compiler
- CMake 3.20 or newer
- CTest, included with CMake

The project was developed and tested on Windows using Visual Studio / MSVC.

### Python scripts

Python is required only for:

- synthetic DNA generation
- benchmark processing
- plot generation

Required Python version:

```text
Python 3
```

Required packages:

```text
pandas
matplotlib
```

---

## 6. Project Structure

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
├── tests/
│   ├── test_fasta.cpp
│   ├── test_repeats.cpp
│   └── test_suffix_array.cpp
│
├── scripts/
│   ├── analyze_repeat_algorithms.py
│   ├── analyze_suffix_array.py
│   ├── generate_dna.py
│   └── plot_benchmarks.py
│
├── data/
│   ├── raw/
│   │   ├── real/
│   │   ├── tests/
│   │   │   ├── test.fasta
│   │   │   ├── test_invalid_char.fasta
│   │   │   ├── test_multiple_records.fasta
│   │   │   └── test_no_header.fasta
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

# 7. Building the Project

Run the following commands from the project root.

## 7.1 Configure CMake

```powershell
cmake -S . -B build
```

## 7.2 Build the Release Version

```powershell
cmake --build build --config Release
```

The executable is created at:

```text
build\Release\master_rad_esa.exe
```

For performance measurements, use the `Release` configuration.

## 7.3 Run Automated Tests

```powershell
ctest --test-dir build -C Release --output-on-failure
```

The current CTest suite contains:

```text
suffix_array_tests
repeat_tests
fasta_tests
```

A successful run reports:

```text
100% tests passed, 0 tests failed
```

Complete build-and-test workflow:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

---

# 8. Running the Program

General syntax:

```powershell
.\build\Release\master_rad_esa.exe <fasta_file> [options]
```

Available options:

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

Defaults:

```text
minimum repeat length = 1
repeat type           = both
implementation        = final
```

---

# 9. Implementation Configurations

Four configurations are provided.

| Configuration | Suffix Array | Maximal-repeat algorithm |
|---|---|---|
| `baseline` | baseline prefix-doubling | baseline |
| `optimized-sa` | optimized prefix-doubling | baseline |
| `sais` | SA-IS | baseline |
| `final` | SA-IS | optimized |

This separation supports two controlled comparisons.

### Suffix Array comparison

```text
baseline
optimized-sa
sais
```

All three use the same baseline maximal-repeat algorithm.

### Repeat-algorithm comparison

```text
sais
final
```

Both use the same SA-IS Suffix Array.

Therefore, differences in `repeat_detection_time_ms` isolate the effect of the maximal-repeat algorithm.

---

# 10. Quick Functional Test

A small test sequence is included in:

```text
data/raw/tests/test.fasta
```

Run:

```powershell
.\build\Release\master_rad_esa.exe data\raw\tests\test.fasta --min-length 2 --type both --implementation final
```

The sequence is:

```text
ACGTACGTGATTACANN
```

Expected maximal repeats:

```text
ACGT    length 4    positions {0, 4}
AC      length 2    positions {0, 4, 12}
TAC     length 3    positions {3, 11}
```

Expected supermaximal repeats:

```text
ACGT    length 4    positions {0, 4}
TAC     length 3    positions {3, 11}
```

Expected counts:

```text
Maximal repeats:       3
Supermaximal repeats:  2
```

The final implementation reports occurrence positions in Suffix Array order, so the printed numeric order may differ, for example:

```text
AC  -> 12 0 4
TAC -> 11 3
```

The set of positions is the relevant result.

Output files:

```text
data/processed/repeats/tests/test_maximal.csv
data/processed/repeats/tests/test_supermaximal.csv
```

---

# 11. FASTA Input Format

The program expects exactly one FASTA record.

Example:

```text
>example_sequence
ACGTACGTGATTACANN
```

Supported nucleotide symbols:

```text
A C G T N
```

Behavior:

- lowercase symbols are converted to uppercase;
- whitespace inside sequence lines is ignored;
- `N` is treated as a literal sequence symbol, not a wildcard.

Rejected input includes:

- missing FASTA header;
- sequence before the header;
- more than one FASTA record;
- empty sequence;
- symbols outside `A`, `C`, `G`, `T`, `N`.

---

# 12. Repeat Output

Outside benchmark mode, repeats are printed to the terminal and written to CSV.

CSV schema:

```text
sequence,length,occurrences,positions
```

Example:

```text
ACGT,4,2,"0 4"
```

Positions are zero-based positions in the original sequence.

Depending on the input location, output is written to:

```text
data/processed/repeats/tests/
data/processed/repeats/synthetic/
data/processed/repeats/real/
```

---

# 13. Performance and Memory Output

Each run reports timing information for:

```text
FASTA parsing
ESA construction
Suffix Array
Inverse Suffix Array
LCP Array
BWT
Maximal repeat detection
Supermaximal repeat detection
Total runtime
```

It also reports:

```text
ESA estimated memory
Peak process memory
```

### ESA estimated memory

This estimates the persistent main ESA structures:

```text
input text
Suffix Array
Inverse Suffix Array
LCP Array
BWT
```

### Peak process memory

On Windows, peak process memory is measured using the process peak working set and includes memory used by the complete running process.

---

# 14. Benchmark Mode

Enable benchmark mode with:

```text
--benchmark
```

Example:

```powershell
.\build\Release\master_rad_esa.exe data\raw\synthetic_10k.fasta --min-length 10 --type maximal --implementation final --benchmark
```

A custom output name can be selected with:

```text
--benchmark-name NAME
```

Example:

```powershell
.\build\Release\master_rad_esa.exe data\raw\synthetic_100k.fasta --min-length 10 --type maximal --implementation sais --benchmark --benchmark-name suffix_array_comparison
```

This writes to:

```text
data/processed/benchmarks/suffix_array_comparison.csv
```

Benchmark CSV columns:

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

For Suffix Array comparisons use:

```text
suffix_array_time_ms
```

For repeat-algorithm comparisons use:

```text
repeat_detection_time_ms
```

---

# 15. Synthetic DNA Generation

Synthetic DNA sequences are generated with:

```text
scripts/generate_dna.py
```

Example:

```powershell
python scripts\generate_dna.py --length 10000 --seed 42 --output data\raw\synthetic_10k.fasta
```

The generated sequence uses a uniform:

```text
A C G T
```

alphabet.

The experiments use seed:

```text
42
```

and synthetic sequence lengths:

```text
10,000
50,000
100,000
250,000
500,000
```

---

# 16. Real Genomic Datasets

The experimental evaluation includes:

```text
SARS-CoV-2 Wuhan-Hu-1          29,903 bp
Mycoplasma genitalium G37      580,076 bp
Bacillus subtilis 168          4,215,606 bp
Escherichia coli K-12 MG1655   4,641,652 bp
```

Real FASTA files are stored under:

```text
data/raw/real/
```

The repeat benchmark uses:

```text
SARS-CoV-2:             minimum length 10
M. genitalium:          minimum length 20
B. subtilis:            minimum length 20
E. coli K-12:           minimum length 20
```

---

# 17. Suffix Array Benchmark Analysis

The dedicated analysis script is:

```text
scripts/analyze_suffix_array.py
```

Run:

```powershell
python scripts\analyze_suffix_array.py
```

Input benchmark files:

```text
data/processed/benchmarks/suffix_array_comparison.csv
data/processed/benchmarks/real_suffix_array_comparison.csv
```

Generated summaries:

```text
data/processed/benchmarks/suffix_array_synthetic_summary.csv
data/processed/benchmarks/suffix_array_real_summary.csv
```

Generated plots:

```text
05_suffix_array_scalability.png
06_suffix_array_real_datasets.png
07_suffix_array_synthetic_speedup.png
08_suffix_array_real_speedup.png
```

The analysis validates that compared configurations return consistent repeat counts and uses median construction times across repeated runs.

The experiments show that optimized prefix-doubling can be faster on smaller synthetic inputs, while SA-IS becomes increasingly advantageous on larger inputs and large real genomes.

---

# 18. Maximal-Repeat Algorithm Benchmark

The maximal-repeat benchmark compares:

```text
sais  = SA-IS + baseline maximal-repeat detection
final = SA-IS + optimized maximal-repeat detection
```

This keeps Suffix Array construction fixed and isolates the repeat-detection algorithm.

Each configuration is executed five times and the median:

```text
repeat_detection_time_ms
```

is used.

## 18.1 Synthetic Results

| Dataset | Length | Baseline median | Optimized median | Speedup | Time reduction |
|---|---:|---:|---:|---:|---:|
| synthetic_10k | 10,000 | 7.946 ms | 4.434 ms | 1.79x | 44.2% |
| synthetic_50k | 50,000 | 55.705 ms | 22.275 ms | 2.50x | 60.0% |
| synthetic_100k | 100,000 | 81.793 ms | 44.775 ms | 1.83x | 45.3% |
| synthetic_250k | 250,000 | 252.355 ms | 111.200 ms | 2.27x | 55.9% |
| synthetic_500k | 500,000 | 581.552 ms | 226.104 ms | 2.57x | 61.1% |

Repeat counts are identical between the baseline and optimized implementations for every synthetic dataset.

Observed repeat counts:

```text
10k      24
50k      883
100k     3,383
250k     20,389
500k     74,795
```

The largest observed synthetic speedup is:

```text
2.57x
```

on the 500,000-base dataset.

## 18.2 Real-Genome Results

| Dataset | Length | Baseline median | Optimized median | Speedup | Time reduction |
|---|---:|---:|---:|---:|---:|
| SARS-CoV-2 | 29,903 | 26.683 ms | 15.004 ms | 1.78x | 43.8% |
| M. genitalium | 580,076 | 483.716 ms | 263.899 ms | 1.83x | 45.4% |
| B. subtilis | 4,215,606 | 3403.660 ms | 1917.430 ms | 1.78x | 43.7% |
| E. coli K-12 | 4,641,652 | 3500.230 ms | 1925.060 ms | 1.82x | 45.0% |

Repeat counts are again identical:

```text
SARS-CoV-2       804
M. genitalium    596
B. subtilis      622
E. coli K-12     2,048
```

On the real genomic datasets, the optimized implementation consistently reduces maximal-repeat detection time by approximately:

```text
44% - 45%
```

and produces a speedup of approximately:

```text
1.78x - 1.83x
```

## 18.3 Memory Trade-off

The optimized algorithm stores a left-context mask for LCP interval nodes and can therefore use more peak process memory on large real datasets.

Median peak-memory overhead in the current real-data benchmark is approximately:

```text
SARS-CoV-2        +5.5%
M. genitalium     +8.2%
B. subtilis       +2.9%
E. coli K-12     +10.7%
```

Peak process memory can vary between runs, especially for smaller datasets, so runtime conclusions are based primarily on repeated median timing measurements.

---

# 19. Repeat Benchmark Analysis Script

The dedicated repeat-analysis script is:

```text
scripts/analyze_repeat_algorithms.py
```

Run:

```powershell
python scripts\analyze_repeat_algorithms.py
```

It reads:

```text
data/processed/benchmarks/repeat_algorithm_comparison.csv
data/processed/benchmarks/real_repeat_algorithm_comparison.csv
```

The script:

- validates repeat-count stability across repeated runs;
- verifies that `sais` and `final` produce identical repeat counts;
- calculates median repeat-detection times;
- calculates optimized-algorithm speedup;
- calculates percentage runtime reduction;
- summarizes peak-memory differences;
- writes synthetic and real summary CSV files;
- generates repeat-algorithm runtime and speedup plots.

Generated summaries:

```text
data/processed/benchmarks/repeat_algorithm_synthetic_summary.csv
data/processed/benchmarks/repeat_algorithm_real_summary.csv
```

Generated plots:

```text
09_repeat_algorithm_scalability.png
10_repeat_algorithm_synthetic_speedup.png
11_repeat_algorithm_real_datasets.png
12_repeat_algorithm_real_speedup.png
```

A speedup greater than `1.0x` means that the optimized repeat algorithm is faster than the baseline.

---

# 20. General Benchmark Plots

The existing general plotting script is:

```text
scripts/plot_benchmarks.py
```

Run:

```powershell
python scripts\plot_benchmarks.py
```

It generates:

```text
01_runtime_scalability.png
02_esa_components.png
03_memory_scalability.png
04_repeat_counts.png
```

Plots are stored in:

```text
data/processed/plots/
```

---

# 21. Experimental Methodology

The experimental evaluation contains complementary controlled comparisons.

## 21.1 Synthetic scalability

Synthetic DNA generated with a fixed random seed provides reproducible inputs whose length can be increased systematically.

Measurements include:

- Suffix Array construction time
- ESA construction time
- maximal-repeat detection time
- supermaximal-repeat detection time
- total runtime
- estimated ESA memory
- peak process memory
- repeat count

## 21.2 Suffix Array comparison

Compared configurations:

```text
baseline
optimized-sa
sais
```

All use the same baseline maximal-repeat algorithm.

This isolates Suffix Array construction.

Each configuration is executed five times and median `suffix_array_time_ms` is used.

## 21.3 Maximal-repeat comparison

Compared configurations:

```text
sais
final
```

Both use SA-IS.

This isolates maximal-repeat detection.

Each configuration is executed five times and median `repeat_detection_time_ms` is used.

## 21.4 Real genomic evaluation

Public genomic sequences are used to evaluate practical behavior on biologically realistic data and substantially larger inputs.

Release builds are used for performance experiments.

Median values are preferred over single-run timings to reduce the effect of runtime noise and cache/cold-start effects.

---

# 22. Validation and Regression Tests

Correctness checks are automated through CTest.

## 22.1 Suffix Array Tests

`tests/test_suffix_array.cpp` validates all three Suffix Array implementations on known examples:

```text
empty string
A
BANANA
MISSISSIPPI
AAAAA
ACGTACGTGATTACANN
```

For example:

```text
BANANA
-> 5 3 1 0 4 2
```

and:

```text
MISSISSIPPI
-> 10 7 4 1 0 9 8 6 3 5 2
```

For each case:

```text
baseline
=
optimized prefix-doubling
=
SA-IS
=
expected Suffix Array
```

## 22.2 Repeat Regression Tests

`tests/test_repeats.cpp` validates repeat detection on multiple known examples.

For:

```text
NLAREPLNOREPTFCGIREPTLSIG
```

with minimum length `3`, the expected results are:

```text
Maximal:
REP     positions {3, 9, 17}
REPT    positions {9, 17}

Supermaximal:
REPT    positions {9, 17}
```

For the DNA test:

```text
ACGTACGTGATTACANN
```

with minimum length `2`, expected results are:

```text
Maximal:
ACGT    {0, 4}
AC      {0, 4, 12}
TAC     {3, 11}

Supermaximal:
ACGT    {0, 4}
TAC     {3, 11}
```

Occurrence positions are compared as sets, so Suffix Array output order does not affect correctness.

The test suite additionally verifies the structural invariant required by the bottom-up maximal-repeat algorithm:

```text
each internal child is stored before its parent
```

and that each child interval is contained within its parent interval.

## 22.3 FASTA Parser Tests

`tests/test_fasta.cpp` validates:

```text
data/raw/tests/test.fasta
data/raw/tests/test_invalid_char.fasta
data/raw/tests/test_multiple_records.fasta
data/raw/tests/test_no_header.fasta
```

The tests verify successful parsing of valid input and rejection of:

- invalid symbols;
- multiple records;
- missing headers.

## 22.4 Benchmark Correctness Checks

The analysis scripts independently verify that repeat counts remain equal between implementations being compared.

This gives an additional regression check that performance optimizations do not change the detected maximal-repeat set.

---

# 23. Notes and Current Scope

The project focuses on exact direct repeats.

It does not currently search for:

- approximate repeats;
- reverse-complement repeats;
- repeats with mismatches;
- repeats with insertions or deletions.

The FASTA parser currently supports one DNA record at a time and the alphabet:

```text
A C G T N
```

`N` is a literal symbol and has no wildcard semantics.

The internal Suffix Array and repeat algorithms operate on byte-valued strings, which also allows direct algorithmic regression tests on non-DNA strings without passing through the FASTA parser.

---

# 24. References

M. I. Abouelhoda, S. Kurtz, E. Ohlebusch,  
"Replacing suffix trees with enhanced suffix arrays",  
Journal of Discrete Algorithms, Volume 2, Issue 1, 2004, pp. 53-86.

G. Nong, S. Zhang, W. H. Chan,  
"Linear Suffix Array Construction by Almost Pure Induced-Sorting",  
Data Compression Conference, 2009.

E. Ohlebusch, T. Beller,  
"Alphabet-independent algorithms for finding context-sensitive repeats in linear time",  
Journal of Discrete Algorithms, Volume 34, 2015, pp. 23-36.

The first paper provides the theoretical basis for Enhanced Suffix Arrays, LCP intervals, and ESA-based repeat processing used throughout the project.

The second paper provides the basis for the linear-time SA-IS Suffix Array construction.

The third paper motivates the linear-time, context-based treatment of repeats and the avoidance of explicit quadratic pair enumeration. The implementation in this repository follows these general design principles while using its own fixed-byte-alphabet context-mask representation.
