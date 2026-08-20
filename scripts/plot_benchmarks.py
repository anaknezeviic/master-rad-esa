from pathlib import Path
import csv
from statistics import median

import matplotlib.pyplot as plt


BENCHMARK_FILE = Path(
    "data/processed/benchmarks/synthetic_scalability.csv"
)

OUTPUT_DIR = Path(
    "data/processed/plots"
)

SEQUENCE_LENGTHS = [
    10_000,
    50_000,
    100_000,
    250_000,
    500_000,
]


def read_csv(file_path):
    rows = []

    with open(
        file_path,
        newline="",
        encoding="utf-8",
    ) as file:

        reader = csv.DictReader(file)

        for row in reader:
            row["sequence_length"] = int(
                row["sequence_length"]
            )

            row["min_length"] = int(
                row["min_length"]
            )

            row["fasta_time_ms"] = float(
                row["fasta_time_ms"]
            )

            row["suffix_array_time_ms"] = float(
                row["suffix_array_time_ms"]
            )

            row["inverse_suffix_array_time_ms"] = float(
                row["inverse_suffix_array_time_ms"]
            )

            row["lcp_time_ms"] = float(
                row["lcp_time_ms"]
            )

            row["bwt_time_ms"] = float(
                row["bwt_time_ms"]
            )

            row["esa_time_ms"] = float(
                row["esa_time_ms"]
            )

            row["repeat_detection_time_ms"] = float(
                row["repeat_detection_time_ms"]
            )

            row["total_time_ms"] = float(
                row["total_time_ms"]
            )

            row["esa_memory_bytes"] = int(
                row["esa_memory_bytes"]
            )

            row["peak_memory_bytes"] = int(
                row["peak_memory_bytes"]
            )

            row["repeat_count"] = int(
                row["repeat_count"]
            )

            rows.append(row)

    return rows


def filter_rows(
    rows,
    sequence_length,
    repeat_type=None,
):
    result = []

    for row in rows:
        if (
            row["sequence_length"]
            != sequence_length
        ):
            continue

        if (
            row.get("implementation")
            != "final"
        ):
            continue

        if (
            repeat_type is not None
            and row["repeat_type"]
            != repeat_type
        ):
            continue

        result.append(row)

    return result


def median_value(rows, field):
    if not rows:
        raise RuntimeError(
            f"No rows available for field '{field}'."
        )

    return median(
        row[field]
        for row in rows
    )


def format_lengths():
    return [
        f"{length // 1000}k"
        for length in SEQUENCE_LENGTHS
    ]


def save_plot(file_name):
    OUTPUT_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )

    output_path = (
        OUTPUT_DIR / file_name
    )

    plt.tight_layout()

    plt.savefig(
        output_path,
        dpi=300,
        bbox_inches="tight",
    )

    plt.close()

    print(
        f"Written: {output_path}"
    )


def validate_repeat_counts(rows):
    print(
        "Validating repeat counts..."
    )

    for length in SEQUENCE_LENGTHS:

        maximal_rows = filter_rows(
            rows,
            length,
            repeat_type="maximal",
        )

        supermaximal_rows = filter_rows(
            rows,
            length,
            repeat_type="supermaximal",
        )

        if not maximal_rows:
            raise RuntimeError(
                "Missing maximal benchmark rows "
                f"for sequence length {length}."
            )

        if not supermaximal_rows:
            raise RuntimeError(
                "Missing supermaximal benchmark rows "
                f"for sequence length {length}."
            )

        maximal_counts = {
            row["repeat_count"]
            for row in maximal_rows
        }

        supermaximal_counts = {
            row["repeat_count"]
            for row in supermaximal_rows
        }

        if len(maximal_counts) != 1:
            raise RuntimeError(
                "Maximal repeat count is not stable "
                f"for sequence length {length}."
            )

        if len(supermaximal_counts) != 1:
            raise RuntimeError(
                "Supermaximal repeat count is not stable "
                f"for sequence length {length}."
            )

    print(
        "Repeat-count validation passed."
    )


def plot_runtime_scalability(rows):
    esa_times = []
    maximal_times = []
    supermaximal_times = []
    total_times = []

    for length in SEQUENCE_LENGTHS:

        maximal = filter_rows(
            rows,
            length,
            repeat_type="maximal",
        )

        supermaximal = filter_rows(
            rows,
            length,
            repeat_type="supermaximal",
        )

        esa_times.append(
            median_value(
                maximal,
                "esa_time_ms",
            )
        )

        maximal_times.append(
            median_value(
                maximal,
                "repeat_detection_time_ms",
            )
        )

        supermaximal_times.append(
            median_value(
                supermaximal,
                "repeat_detection_time_ms",
            )
        )

        total_times.append(
            median_value(
                maximal,
                "total_time_ms",
            )
        )

    plt.figure(
        figsize=(9, 6)
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        esa_times,
        marker="o",
        label="ESA construction",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        maximal_times,
        marker="o",
        label="Maximal repeats",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        supermaximal_times,
        marker="o",
        label="Supermaximal repeats",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        total_times,
        marker="o",
        label="Total runtime",
    )

    plt.xlabel(
        "Sequence length"
    )

    plt.ylabel(
        "Median time (ms)"
    )

    plt.title(
        "Final Implementation Runtime Scalability"
    )

    plt.grid(
        True,
        alpha=0.3,
    )

    plt.legend()

    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths(),
    )

    save_plot(
        "01_runtime_scalability.png"
    )


def plot_esa_components(rows):
    suffix_array_times = []
    inverse_sa_times = []
    lcp_times = []
    bwt_times = []

    for length in SEQUENCE_LENGTHS:

        current_rows = filter_rows(
            rows,
            length,
            repeat_type="maximal",
        )

        suffix_array_times.append(
            median_value(
                current_rows,
                "suffix_array_time_ms",
            )
        )

        inverse_sa_times.append(
            median_value(
                current_rows,
                "inverse_suffix_array_time_ms",
            )
        )

        lcp_times.append(
            median_value(
                current_rows,
                "lcp_time_ms",
            )
        )

        bwt_times.append(
            median_value(
                current_rows,
                "bwt_time_ms",
            )
        )

    plt.figure(
        figsize=(9, 6)
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        suffix_array_times,
        marker="o",
        label="SA-IS Suffix Array",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        inverse_sa_times,
        marker="o",
        label="Inverse Suffix Array",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        lcp_times,
        marker="o",
        label="LCP Array",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        bwt_times,
        marker="o",
        label="BWT",
    )

    plt.xlabel(
        "Sequence length"
    )

    plt.ylabel(
        "Median construction time (ms)"
    )

    plt.title(
        "ESA Construction Components"
    )

    plt.grid(
        True,
        alpha=0.3,
    )

    plt.legend()

    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths(),
    )

    save_plot(
        "02_esa_components.png"
    )


def plot_memory_scalability(rows):
    esa_memory = []
    peak_memory = []

    for length in SEQUENCE_LENGTHS:

        current_rows = filter_rows(
            rows,
            length,
            repeat_type="maximal",
        )

        esa_bytes = median_value(
            current_rows,
            "esa_memory_bytes",
        )

        peak_bytes = median_value(
            current_rows,
            "peak_memory_bytes",
        )

        esa_memory.append(
            esa_bytes
            / (1024 * 1024)
        )

        peak_memory.append(
            peak_bytes
            / (1024 * 1024)
        )

    plt.figure(
        figsize=(9, 6)
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        esa_memory,
        marker="o",
        label="ESA estimated memory",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        peak_memory,
        marker="o",
        label="Peak process memory",
    )

    plt.xlabel(
        "Sequence length"
    )

    plt.ylabel(
        "Memory (MiB)"
    )

    plt.title(
        "Memory Scalability"
    )

    plt.grid(
        True,
        alpha=0.3,
    )

    plt.legend()

    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths(),
    )

    save_plot(
        "03_memory_scalability.png"
    )


def plot_repeat_counts(rows):
    maximal_counts = []
    supermaximal_counts = []

    for length in SEQUENCE_LENGTHS:

        maximal = filter_rows(
            rows,
            length,
            repeat_type="maximal",
        )

        supermaximal = filter_rows(
            rows,
            length,
            repeat_type="supermaximal",
        )

        maximal_counts.append(
            median_value(
                maximal,
                "repeat_count",
            )
        )

        supermaximal_counts.append(
            median_value(
                supermaximal,
                "repeat_count",
            )
        )

    plt.figure(
        figsize=(9, 6)
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        maximal_counts,
        marker="o",
        label="Maximal repeats",
    )

    plt.plot(
        SEQUENCE_LENGTHS,
        supermaximal_counts,
        marker="o",
        label="Supermaximal repeats",
    )

    plt.xlabel(
        "Sequence length"
    )

    plt.ylabel(
        "Number of repeats"
    )

    plt.title(
        "Detected Repeats by Sequence Length"
    )

    plt.grid(
        True,
        alpha=0.3,
    )

    plt.legend()

    plt.xticks(
        SEQUENCE_LENGTHS,
        format_lengths(),
    )

    save_plot(
        "04_repeat_counts.png"
    )


def main():
    print(
        "Reading final synthetic benchmark data..."
    )

    rows = read_csv(
        BENCHMARK_FILE
    )

    final_rows = [
        row
        for row in rows
        if row.get("implementation") == "final"
    ]

    print(
        f"Final implementation rows: "
        f"{len(final_rows)}"
    )

    print()

    validate_repeat_counts(
        final_rows
    )

    print()

    plot_runtime_scalability(
        final_rows
    )

    plot_esa_components(
        final_rows
    )

    plot_memory_scalability(
        final_rows
    )

    plot_repeat_counts(
        final_rows
    )

    print()

    print(
        "All final scalability plots "
        "generated successfully."
    )


if __name__ == "__main__":
    main()