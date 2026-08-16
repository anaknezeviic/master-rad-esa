from pathlib import Path
import csv
from statistics import median

import matplotlib.pyplot as plt


COMPARISON_FILE = Path("data/processed/benchmarks/optimization_comparison.csv")
RELEASE_FILE = Path("data/processed/benchmarks/synthetic_scalability.csv")
OUTPUT_DIR = Path("data/processed/plots")
SPEEDUP_OUTPUT_FILE = Path("data/processed/benchmarks/release_speedup_summary.csv")

SEQUENCE_LENGTHS = [
    10_000,
    50_000,
    100_000,
    250_000,
    500_000,
]

def read_csv(file_path):
    rows = []

    with open(file_path, newline="", encoding="utf-8") as file:

        reader = csv.DictReader(file)

        for row in reader:
            row["sequence_length"] = int(row["sequence_length"])
            row["min_length"] = int(row["min_length"])
            row["fasta_time_ms"] = float(row["fasta_time_ms"])
            row["suffix_array_time_ms"] = float(row["suffix_array_time_ms"])
            row["inverse_suffix_array_time_ms"] = float(row["inverse_suffix_array_time_ms"])
            row["lcp_time_ms"] = float(row["lcp_time_ms"])
            row["bwt_time_ms"] = float( row["bwt_time_ms"])
            row["esa_time_ms"] = float(row["esa_time_ms"])
            row["repeat_detection_time_ms"] = float(row["repeat_detection_time_ms"])
            row["total_time_ms"] = float(row["total_time_ms"])
            row["esa_memory_bytes"] = int(row["esa_memory_bytes"])
            row["peak_memory_bytes"] = int(row["peak_memory_bytes"])
            row["repeat_count"] = int(row["repeat_count"])

            rows.append(row)

    return rows


def filter_rows(rows, sequence_length, repeat_type=None, implementation=None):
    result = []

    for row in rows:
        if (row["sequence_length"] != sequence_length):
            continue
        if (repeat_type is not None and row["repeat_type"] != repeat_type):
            continue
        if implementation is not None:
            if (row.get("implementation") != implementation):
                continue

        result.append(row)

    return result


def median_value(rows, field):
    if not rows:
        raise RuntimeError(f"No rows available for field '{field}'.")

    return median(
        row[field]
        for row in rows
    )


def format_lengths():
    return [f"{length // 1000}k" for length in SEQUENCE_LENGTHS]

def save_plot(file_name):
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    output_path = (OUTPUT_DIR / file_name)

    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches="tight")
    plt.close()

    print(f"Written: {output_path}")


def plot_suffix_array_optimization(comparison_rows):
    baseline_times = []
    optimized_times = []

    for length in SEQUENCE_LENGTHS:

        baseline = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="baseline"
        )

        optimized = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="optimized_sa"
        )

        baseline_times.append(
            median_value(
                baseline,
                "suffix_array_time_ms"
            )
        )

        optimized_times.append(
            median_value(
                optimized,
                "suffix_array_time_ms"
            )
        )

    plt.figure(figsize=(9, 6))

    plt.plot(
        SEQUENCE_LENGTHS,
        baseline_times,
        marker="o",
        label="Baseline suffix array"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        optimized_times,
        marker="o",
        label="Optimized suffix array"
    )

    plt.xlabel("Sequence length")

    plt.ylabel("Median construction time (ms)")

    plt.title(
        "Suffix Array Construction: "
        "Baseline vs Optimized"
    )

    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.xticks(SEQUENCE_LENGTHS, format_lengths())

    save_plot("01_suffix_array_optimization.png")


def plot_maximal_repeat_optimization(comparison_rows):
    baseline_times = []
    optimized_sa_times = []
    final_times = []

    for length in SEQUENCE_LENGTHS:

        baseline = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="baseline"
        )

        optimized_sa = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="optimized_sa"
        )

        final = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="final"
        )

        baseline_times.append(
            median_value(
                baseline,
                "repeat_detection_time_ms"
            )
        )

        optimized_sa_times.append(
            median_value(
                optimized_sa,
                "repeat_detection_time_ms"
            )
        )

        final_times.append(
            median_value(
                final,
                "repeat_detection_time_ms"
            )
        )

    plt.figure(figsize=(9, 6))

    plt.plot(
        SEQUENCE_LENGTHS,
        baseline_times,
        marker="o",
        label="Baseline"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        optimized_sa_times,
        marker="o",
        label="Optimized SA"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        final_times,
        marker="o",
        label="Final optimized implementation"
    )

    plt.xlabel("Sequence length")
    plt.ylabel("Median detection time (ms)")
    plt.title("Maximal Repeat Detection Optimization")
    plt.grid(True,alpha=0.3)
    plt.legend()
    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths()
    )

    save_plot("02_maximal_repeat_optimization.png")


def plot_total_runtime_comparison(comparison_rows):
    baseline_times = []
    optimized_sa_times = []
    final_times = []

    for length in SEQUENCE_LENGTHS:

        baseline = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="baseline"
        )

        optimized_sa = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="optimized_sa"
        )

        final = filter_rows(
            comparison_rows,
            length,
            repeat_type="maximal",
            implementation="final"
        )

        baseline_times.append(
            median_value(
                baseline,
                "total_time_ms"
            )
        )

        optimized_sa_times.append(
            median_value(
                optimized_sa,
                "total_time_ms"
            )
        )

        final_times.append(
            median_value(
                final,
                "total_time_ms"
            )
        )

    plt.figure(figsize=(9, 6))

    plt.plot(
        SEQUENCE_LENGTHS,
        baseline_times,
        marker="o",
        label="Baseline"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        optimized_sa_times,
        marker="o",
        label="Optimized SA"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        final_times,
        marker="o",
        label="Final"
    )

    plt.xlabel("Sequence length")
    plt.ylabel("Median total runtime (ms)")
    plt.title(
        "Total Runtime: "
        "Baseline vs Optimized Implementations"
    )
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths()
    )

    save_plot("03_total_runtime_comparison.png")


def plot_release_runtime(release_rows):
    esa_times = []
    maximal_times = []
    supermaximal_times = []
    total_times = []

    for length in SEQUENCE_LENGTHS:

        maximal = filter_rows(
            release_rows,
            length,
            repeat_type="maximal"
        )

        supermaximal = filter_rows(
            release_rows,
            length,
            repeat_type="supermaximal"
        )

        esa_times.append(
            median_value(
                maximal,
                "esa_time_ms"
            )
        )

        maximal_times.append(
            median_value(
                maximal,
                "repeat_detection_time_ms"
            )
        )

        supermaximal_times.append(
            median_value(
                supermaximal,
                "repeat_detection_time_ms"
            )
        )

        total_times.append(
            median_value(
                maximal,
                "total_time_ms"
            )
        )

    plt.figure(figsize=(9, 6))

    plt.plot(
        SEQUENCE_LENGTHS,
        esa_times,
        marker="o",
        label="ESA construction"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        maximal_times,
        marker="o",
        label="Maximal repeats"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        supermaximal_times,
        marker="o",
        label="Supermaximal repeats"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        total_times,
        marker="o",
        label="Total runtime"
    )

    plt.xlabel("Sequence length")
    plt.ylabel("Median time (ms)")
    plt.title("Final Release Runtime Scalability")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths()
    )

    save_plot("04_runtime_scalability.png")


def plot_release_esa_components(release_rows):
    suffix_array_times = []
    inverse_sa_times = []
    lcp_times = []
    bwt_times = []

    for length in SEQUENCE_LENGTHS:

        rows = filter_rows(
            release_rows,
            length,
            repeat_type="maximal"
        )

        suffix_array_times.append(
            median_value(
                rows,
                "suffix_array_time_ms"
            )
        )

        inverse_sa_times.append(
            median_value(
                rows,
                "inverse_suffix_array_time_ms"
            )
        )

        lcp_times.append(
            median_value(
                rows,
                "lcp_time_ms"
            )
        )

        bwt_times.append(
            median_value(
                rows,
                "bwt_time_ms"
            )
        )

    plt.figure(figsize=(9, 6))

    plt.plot(
        SEQUENCE_LENGTHS,
        suffix_array_times,
        marker="o",
        label="Suffix Array"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        inverse_sa_times,
        marker="o",
        label="Inverse Suffix Array"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        lcp_times,
        marker="o",
        label="LCP Array"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        bwt_times,
        marker="o",
        label="BWT"
    )

    plt.xlabel("Sequence length")
    plt.ylabel("Median construction time (ms)")
    plt.title("ESA Construction Components")
    plt.grid(True,alpha=0.3)
    plt.legend()
    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths()
    )

    save_plot("05_esa_components.png")


def plot_release_memory(release_rows):
    esa_memory = []
    peak_memory = []

    for length in SEQUENCE_LENGTHS:

        rows = filter_rows(
            release_rows,
            length,
            repeat_type="maximal"
        )

        esa_bytes = median_value(
            rows,
            "esa_memory_bytes"
        )

        peak_bytes = median_value(
            rows,
            "peak_memory_bytes"
        )

        esa_memory.append(esa_bytes /(1024 * 1024))

        peak_memory.append(peak_bytes /(1024 * 1024))

    plt.figure(figsize=(9, 6))

    plt.plot(
        SEQUENCE_LENGTHS,
        esa_memory,
        marker="o",
        label="ESA estimated memory"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        peak_memory,
        marker="o",
        label="Peak process memory"
    )

    plt.xlabel("Sequence length")
    plt.ylabel("Memory (MiB)")
    plt.title("Memory Scalability")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths()
    )

    save_plot("06_memory_scalability.png")


def plot_release_repeat_counts(release_rows):
    maximal_counts = []
    supermaximal_counts = []

    for length in SEQUENCE_LENGTHS:

        maximal = filter_rows(
            release_rows,
            length,
            repeat_type="maximal"
        )

        supermaximal = filter_rows(
            release_rows,
            length,
            repeat_type="supermaximal"
        )

        maximal_counts.append(
            median_value(
                maximal,
                "repeat_count"
            )
        )

        supermaximal_counts.append(
            median_value(
                supermaximal,
                "repeat_count"
            )
        )

    plt.figure(figsize=(9, 6))

    plt.plot(
        SEQUENCE_LENGTHS,
        maximal_counts,
        marker="o",
        label="Maximal repeats"
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        supermaximal_counts,
        marker="o",
        label="Supermaximal repeats"
    )

    plt.xlabel("Sequence length")
    plt.ylabel("Number of repeats")
    plt.title("Detected Repeats by Sequence Length")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths()
    )

    save_plot("07_repeat_counts.png")


def write_speedup_summary(comparison_rows):
    SPEEDUP_OUTPUT_FILE.parent.mkdir(parents=True, exist_ok=True)

    with open(SPEEDUP_OUTPUT_FILE, "w", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)

        writer.writerow([
            "sequence_length",
            "baseline_sa_ms",
            "optimized_sa_ms",
            "sa_speedup",
            "baseline_maximal_ms",
            "optimized_sa_maximal_ms",
            "final_maximal_ms",
            "maximal_speedup_vs_optimized_sa",
            "maximal_speedup_vs_baseline",
            "baseline_total_ms",
            "final_total_ms",
            "total_speedup"
        ])

        for length in SEQUENCE_LENGTHS:

            baseline = filter_rows(
                comparison_rows,
                length,
                repeat_type="maximal",
                implementation="baseline"
            )

            optimized_sa = filter_rows(
                comparison_rows,
                length,
                repeat_type="maximal",
                implementation="optimized_sa"
            )

            final = filter_rows(
                comparison_rows,
                length,
                repeat_type="maximal",
                implementation="final"
            )

            baseline_sa = median_value(
                baseline,
                "suffix_array_time_ms"
            )

            optimized_sa_time = median_value(
                optimized_sa,
                "suffix_array_time_ms"
            )

            baseline_maximal = median_value(
                baseline,
                "repeat_detection_time_ms"
            )

            optimized_sa_maximal = median_value(
                optimized_sa,
                "repeat_detection_time_ms"
            )

            final_maximal = median_value(
                final,
                "repeat_detection_time_ms"
            )

            baseline_total = median_value(
                baseline,
                "total_time_ms"
            )

            final_total = median_value(
                final,
                "total_time_ms"
            )

            sa_speedup = (
                baseline_sa /
                optimized_sa_time
            )

            maximal_speedup_vs_optimized_sa = (
                optimized_sa_maximal /
                final_maximal
            )

            maximal_speedup_vs_baseline = (
                baseline_maximal /
                final_maximal
            )

            total_speedup = (
                baseline_total /
                final_total
            )

            writer.writerow([
                length,
                round(baseline_sa, 4),
                round(optimized_sa_time, 4),
                round(sa_speedup, 4),
                round(baseline_maximal, 4),
                round(optimized_sa_maximal, 4),
                round(final_maximal, 4),
                round(maximal_speedup_vs_optimized_sa, 4),
                round(maximal_speedup_vs_baseline, 4),
                round(baseline_total, 4),
                round(final_total, 4),
                round(total_speedup, 4)
            ])

    print(f"Written: {SPEEDUP_OUTPUT_FILE}")


def main():
    print("Reading benchmark data...")

    comparison_rows = read_csv(COMPARISON_FILE)

    release_rows = read_csv(RELEASE_FILE)

    print(
        f"Comparison rows: "
        f"{len(comparison_rows)}"
    )

    print(
        f"Release rows: "
        f"{len(release_rows)}"
    )

    print()

    plot_suffix_array_optimization(comparison_rows)
    plot_maximal_repeat_optimization(comparison_rows)
    plot_total_runtime_comparison(comparison_rows)
    plot_release_runtime(release_rows)
    plot_release_esa_components(release_rows)
    plot_release_memory(release_rows)
    plot_release_repeat_counts(release_rows)
    write_speedup_summary(comparison_rows)

    print()

    print(
        "All benchmark plots and "
        "summary files generated successfully."
    )


if __name__ == "__main__":
    main()