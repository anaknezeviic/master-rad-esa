from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


BENCHMARK_DIR = Path("data/processed/benchmarks")
PLOT_DIR = Path("data/processed/plots")

SYNTHETIC_INPUT = (
    BENCHMARK_DIR / "repeat_algorithm_comparison.csv"
)

REAL_INPUT = (
    BENCHMARK_DIR / "real_repeat_algorithm_comparison.csv"
)

SYNTHETIC_SUMMARY = (
    BENCHMARK_DIR / "repeat_algorithm_synthetic_summary.csv"
)

REAL_SUMMARY = (
    BENCHMARK_DIR / "repeat_algorithm_real_summary.csv"
)


def load_and_validate(path: Path) -> pd.DataFrame:
    if not path.exists():
        raise FileNotFoundError(
            f"Benchmark file not found: {path}"
        )

    df = pd.read_csv(path)

    required_columns = {
        "dataset",
        "implementation",
        "sequence_length",
        "repeat_detection_time_ms",
        "peak_memory_bytes",
        "repeat_count",
    }

    missing = required_columns - set(df.columns)

    if missing:
        raise ValueError(f"Missing columns in {path}: " + ", ".join(sorted(missing)) )

    expected_implementations = {"sais", "final"}

    actual_implementations = set(
        df["implementation"].unique()
    )

    if not actual_implementations.issubset(
        expected_implementations
    ):
        raise ValueError(
            "Unexpected implementations found: "
            + ", ".join(
                sorted(
                    actual_implementations
                    - expected_implementations
                )
            )
        )

    return df


def validate_repeat_counts(
    df: pd.DataFrame,
    label: str,
) -> None:
    grouped = (
        df.groupby(
            ["dataset", "implementation"]
        )["repeat_count"]
        .nunique()
    )

    if (grouped != 1).any():
        raise ValueError(
            f"{label}: repeat counts are not stable "
            "across repeated runs."
        )

    counts = (
        df.groupby(
            ["dataset", "implementation"]
        )["repeat_count"]
        .first()
        .unstack()
    )

    if "sais" not in counts.columns:
        raise ValueError(
            f"{label}: missing sais benchmark results."
        )

    if "final" not in counts.columns:
        raise ValueError(
            f"{label}: missing final benchmark results."
        )

    inconsistent = (
        counts["sais"] != counts["final"]
    )

    if inconsistent.any():
        bad_datasets = counts.index[
            inconsistent
        ].tolist()

        raise ValueError(
            f"{label}: repeat counts differ between "
            f"sais and final for: {bad_datasets}"
        )

    print(
        f"{label}: repeat-count validation passed."
    )


def build_summary(df: pd.DataFrame) -> pd.DataFrame:
    summary = (
        df.groupby(
            [
                "dataset",
                "implementation",
                "sequence_length",
            ],
            as_index=False,
        )
        .agg(
            median_repeat_time_ms=(
                "repeat_detection_time_ms",
                "median",
            ),
            median_peak_memory_bytes=(
                "peak_memory_bytes",
                "median",
            ),
            repeat_count=(
                "repeat_count",
                "first",
            ),
        )
    )

    times = summary.pivot(
        index=[
            "dataset",
            "sequence_length",
            "repeat_count",
        ],
        columns="implementation",
        values="median_repeat_time_ms",
    ).reset_index()

    memory = summary.pivot(
        index=[
            "dataset",
            "sequence_length",
            "repeat_count",
        ],
        columns="implementation",
        values="median_peak_memory_bytes",
    ).reset_index()

    times = times.rename(
        columns={
            "sais": "baseline_repeat_time_ms",
            "final": "optimized_repeat_time_ms",
        }
    )

    memory = memory.rename(
        columns={
            "sais": "baseline_peak_memory_bytes",
            "final": "optimized_peak_memory_bytes",
        }
    )

    result = times.merge(
        memory,
        on=[
            "dataset",
            "sequence_length",
            "repeat_count",
        ],
    )

    result["speedup"] = (
        result["baseline_repeat_time_ms"]
        / result["optimized_repeat_time_ms"]
    )

    result["time_reduction_percent"] = (
        1.0
        - (
            result["optimized_repeat_time_ms"]
            / result["baseline_repeat_time_ms"]
        )
    ) * 100.0

    result["memory_overhead_percent"] = (
        (
            result["optimized_peak_memory_bytes"]
            / result["baseline_peak_memory_bytes"]
        )
        - 1.0
    ) * 100.0

    return result.sort_values(
        "sequence_length"
    ).reset_index(drop=True)


def print_summary(
    title: str,
    summary: pd.DataFrame,
) -> None:
    print()
    print(title)
    print("=" * len(title))

    for _, row in summary.iterrows():
        print(
            f"{row['dataset']}: "
            f"baseline={row['baseline_repeat_time_ms']:.3f} ms, "
            f"optimized={row['optimized_repeat_time_ms']:.3f} ms, "
            f"speedup={row['speedup']:.2f}x, "
            f"time reduction={row['time_reduction_percent']:.1f}%, "
            f"memory overhead={row['memory_overhead_percent']:.1f}%"
        )


def plot_synthetic_runtime(
    summary: pd.DataFrame,
) -> None:
    plt.figure(figsize=(8, 5))

    plt.plot(
        summary["sequence_length"],
        summary["baseline_repeat_time_ms"],
        marker="o",
        label="Baseline",
    )

    plt.plot(
        summary["sequence_length"],
        summary["optimized_repeat_time_ms"],
        marker="o",
        label="Optimized",
    )

    plt.xlabel("Sequence length")
    plt.ylabel("Median repeat-detection time (ms)")
    plt.title(
        "Maximal Repeat Detection on Synthetic DNA"
    )

    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    output = (
        PLOT_DIR
        / "12_repeat_algorithm_scalability.png"
    )

    plt.savefig(output, dpi=300)
    plt.close()


def plot_synthetic_speedup(
    summary: pd.DataFrame,
) -> None:
    plt.figure(figsize=(8, 5))

    plt.plot(
        summary["sequence_length"],
        summary["speedup"],
        marker="o",
    )

    plt.axhline(
        y=1.0,
        linestyle="--",
    )

    for _, row in summary.iterrows():
        plt.annotate(
            f"{row['speedup']:.2f}x",
            (
                row["sequence_length"],
                row["speedup"],
            ),
            textcoords="offset points",
            xytext=(0, 8),
            ha="center",
        )

    plt.xlabel("Sequence length")
    plt.ylabel(
        "Speedup over baselinen"
    )

    plt.title(
        "Maximal Repeat Detection Speedup "
        "on Synthetic DNA"
    )

    plt.tight_layout()

    output = (
        PLOT_DIR
        / "13_repeat_algorithm_synthetic_speedup.png"
    )

    plt.savefig(output, dpi=300)
    plt.close()


def plot_real_runtime(
    summary: pd.DataFrame,
) -> None:
    label_map = {
    "sars_cov_2": "SARS-CoV-2",
    "mycoplasma_genitalium": "M. genitalium",
    "bacillus_subtilis": "B. subtilis",
    "ecoli_k12": "E. coli K-12",
    }

    labels = summary["dataset"].map(label_map)

    x = range(len(summary))

    width = 0.35

    plt.figure(figsize=(9, 5))

    plt.bar(
        [value - width / 2 for value in x],
        summary["baseline_repeat_time_ms"],
        width=width,
        label="Baseline",
    )

    plt.bar(
        [value + width / 2 for value in x],
        summary["optimized_repeat_time_ms"],
        width=width,
        label="Optimized",
    )

    plt.xticks(
        list(x),
        labels,
        rotation=20,
        ha="right",
    )

    plt.ylabel("Median repeat-detection time (ms)")
    plt.title(
        "Maximal Repeat Detection on Real Genomes"
    )

    plt.legend()
    plt.tight_layout()

    output = (
        PLOT_DIR
        / "14_repeat_algorithm_real_datasets.png"
    )

    plt.savefig(output, dpi=300)
    plt.close()


def plot_real_speedup(
    summary: pd.DataFrame,
) -> None:
    label_map = {
        "sars_cov_2": "SARS-CoV-2",
        "mycoplasma_genitalium": "M. genitalium",
        "bacillus_subtilis": "B. subtilis",
        "ecoli_k12": "E. coli K-12",
    }

    labels = summary["dataset"].map(label_map)

    plt.figure(figsize=(9, 5))

    bars = plt.bar(
        labels,
        summary["speedup"],
    )

    plt.axhline(
        y=1.0,
        linestyle="--",
    )

    for bar, speedup in zip(
        bars,
        summary["speedup"],
    ):
        plt.text(
            bar.get_x()
            + bar.get_width() / 2,
            bar.get_height(),
            f"{speedup:.2f}x",
            ha="center",
            va="bottom",
        )

    plt.ylabel(
        "Speedup over baseline"
    )

    plt.title(
        "Maximal Repeat Detection Speedup "
        "on Real Genomes"
    )

    plt.xticks(
        rotation=20,
        ha="right",
    )

    plt.tight_layout()

    output = (
        PLOT_DIR
        / "15_repeat_algorithm_real_speedup.png"
    )

    plt.savefig(output, dpi=300)
    plt.close()


def main() -> None:
    PLOT_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )

    synthetic = load_and_validate(SYNTHETIC_INPUT)

    real = load_and_validate(REAL_INPUT)

    validate_repeat_counts(
        synthetic,
        "Synthetic benchmark",
    )

    validate_repeat_counts(
        real,
        "Real benchmark",
    )

    synthetic_summary = build_summary(
        synthetic
    )

    real_summary = build_summary(
        real
    )

    synthetic_summary.to_csv(
        SYNTHETIC_SUMMARY,
        index=False,
    )

    real_summary.to_csv(
        REAL_SUMMARY,
        index=False,
    )

    print_summary(
        "Synthetic maximal-repeat benchmark",
        synthetic_summary,
    )

    print_summary(
        "Real maximal-repeat benchmark",
        real_summary,
    )

    plot_synthetic_runtime(
        synthetic_summary
    )

    plot_synthetic_speedup(
        synthetic_summary
    )

    plot_real_runtime(
        real_summary
    )

    plot_real_speedup(
        real_summary
    )

    print()
    print("Summary files written to:")
    print(SYNTHETIC_SUMMARY)
    print(REAL_SUMMARY)

    print()
    print("Plots written to:")
    print(
        PLOT_DIR
        / "12_repeat_algorithm_scalability.png"
    )
    print(
        PLOT_DIR
        / "13_repeat_algorithm_synthetic_speedup.png"
    )
    print(
        PLOT_DIR
        / "14_repeat_algorithm_real_datasets.png"
    )
    print(
        PLOT_DIR
        / "15_repeat_algorithm_real_speedup.png"
    )


if __name__ == "__main__":
    main()