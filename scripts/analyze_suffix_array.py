from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


BENCHMARK_DIR = Path("data/processed/benchmarks")
PLOT_DIR = Path("data/processed/plots")

SYNTHETIC_INPUT = BENCHMARK_DIR / "suffix_array_comparison.csv"
REAL_INPUT = BENCHMARK_DIR / "real_suffix_array_comparison.csv"

SYNTHETIC_SUMMARY = BENCHMARK_DIR / "suffix_array_synthetic_summary.csv"
REAL_SUMMARY = BENCHMARK_DIR / "suffix_array_real_summary.csv"

SYNTHETIC_PLOT = PLOT_DIR / "05_suffix_array_scalability.png"
REAL_PLOT = PLOT_DIR / "06_suffix_array_real_datasets.png"
SYNTHETIC_SPEEDUP_PLOT = (
    PLOT_DIR / "07_suffix_array_synthetic_speedup.png"
)

REAL_SPEEDUP_PLOT = (
    PLOT_DIR / "08_suffix_array_real_speedup.png"
)


IMPLEMENTATION_ORDER = [
    "baseline",
    "optimized_sa",
    "sais",
]

IMPLEMENTATION_LABELS = {
    "baseline": "Baseline (prefix-doubling)",
    "optimized_sa": "Optimized prefix-doubling",
    "sais": "SA-IS",
}

REAL_DATASET_LABELS = {
    "sars_cov_2": "SARS-CoV-2",
    "mycoplasma_genitalium": "M. genitalium",
    "bacillus_subtilis": "B. subtilis",
    "ecoli_k12": "E. coli K-12",
}


def validate_repeat_counts(df: pd.DataFrame, name: str) -> None:
    counts = (
        df.groupby(["dataset", "implementation"])["repeat_count"]
        .nunique()
    )

    if (counts.max() != 1):
        raise RuntimeError(
            f"Inconsistent repeat counts between runs in {name}."
        )

    dataset_counts = (
        df.groupby("dataset")["repeat_count"]
        .nunique()
    )

    if dataset_counts.max() != 1:
        raise RuntimeError(
            f"Different implementations produced different "
            f"repeat counts in {name}."
        )

    print(f"{name}: repeat counts are consistent.")


def create_summary(df: pd.DataFrame) -> pd.DataFrame:
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
            suffix_array_time_ms=("suffix_array_time_ms", "median"),
            esa_time_ms=("esa_time_ms", "median"),
            repeat_detection_time_ms=("repeat_detection_time_ms", "median"),
            total_time_ms=("total_time_ms", "median"),
            peak_memory_bytes=("peak_memory_bytes", "median"),
            repeat_count=("repeat_count", "first"),
        )
    )

    baseline_times = (
        summary[
            summary["implementation"] == "baseline"
        ][["dataset", "suffix_array_time_ms"]]
        .rename(
            columns={
                "suffix_array_time_ms":
                    "baseline_suffix_array_time_ms"
            }
        )
    )

    optimized_times = (
        summary[
            summary["implementation"] == "optimized_sa"
        ][["dataset", "suffix_array_time_ms"]]
        .rename(
            columns={
                "suffix_array_time_ms":
                    "optimized_suffix_array_time_ms"
            }
        )
    )

    summary = summary.merge(
        baseline_times,
        on="dataset",
        how="left",
    )

    summary = summary.merge(
        optimized_times,
        on="dataset",
        how="left",
    )

    summary["speedup_vs_baseline"] = (
        summary["baseline_suffix_array_time_ms"]
        / summary["suffix_array_time_ms"]
    )

    summary["speedup_vs_optimized"] = (
        summary["optimized_suffix_array_time_ms"]
        / summary["suffix_array_time_ms"]
    )

    return summary


def plot_synthetic(summary: pd.DataFrame) -> None:
    fig, ax = plt.subplots(figsize=(9, 6))

    for implementation in IMPLEMENTATION_ORDER:
        subset = summary[
            summary["implementation"] == implementation
        ].sort_values("sequence_length")

        ax.plot(
            subset["sequence_length"],
            subset["suffix_array_time_ms"],
            marker="o",
            label=IMPLEMENTATION_LABELS[implementation],
        )

    ax.set_title(
        "Suffix Array Construction on Synthetic DNA Sequences"
    )
    ax.set_xlabel("Sequence length")
    ax.set_ylabel("Median construction time (ms)")
    ax.legend()
    ax.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(SYNTHETIC_PLOT, dpi=300)
    plt.close(fig)


def plot_real(summary: pd.DataFrame) -> None:
    ordered_datasets = [
        "sars_cov_2",
        "mycoplasma_genitalium",
        "bacillus_subtilis",
        "ecoli_k12",
    ]

    pivot = summary.pivot(
        index="dataset",
        columns="implementation",
        values="suffix_array_time_ms",
    )

    pivot = pivot.reindex(ordered_datasets)
    pivot = pivot[IMPLEMENTATION_ORDER]

    pivot.index = [
        REAL_DATASET_LABELS.get(dataset, dataset)
        for dataset in pivot.index
    ]

    ax = pivot.plot(
        kind="bar",
        figsize=(10, 6),
    )

    ax.set_title(
        "Suffix Array Construction on Real Genomic Sequences"
    )
    ax.set_xlabel("Dataset")
    ax.set_ylabel("Median construction time (ms, log scale)")
    ax.set_yscale("log")
    ax.legend(
        [
            IMPLEMENTATION_LABELS[i]
            for i in IMPLEMENTATION_ORDER
        ]
    )

    plt.xticks(rotation=20, ha="right")
    plt.tight_layout()
    plt.savefig(REAL_PLOT, dpi=300)
    plt.close()

def plot_synthetic_speedup(
    summary: pd.DataFrame,
) -> None:

    sais = summary[
        summary["implementation"] == "sais"
    ].sort_values("sequence_length")

    fig, ax = plt.subplots(figsize=(9, 6))

    ax.plot(
        sais["sequence_length"],
        sais["speedup_vs_optimized"],
        marker="o",
        label="SA-IS vs Optimized SA",
    )

    ax.axhline(
        y=1.0,
        linestyle="--",
        linewidth=1,
        label="Equal performance",
    )

    ax.set_title(
        "SA-IS Speedup over Optimized Prefix-Doubling "
        "on Synthetic DNA"
    )

    ax.set_xlabel("Sequence length")
    ax.set_ylabel("Speedup")

    ax.grid(True, alpha=0.3)
    ax.legend()

    for _, row in sais.iterrows():
        ax.annotate(
            f"{row['speedup_vs_optimized']:.2f}x",
            (
                row["sequence_length"],
                row["speedup_vs_optimized"],
            ),
            textcoords="offset points",
            xytext=(0, 8),
            ha="center",
        )

    fig.tight_layout()
    fig.savefig(
        SYNTHETIC_SPEEDUP_PLOT,
        dpi=300,
    )

    plt.close(fig)

def plot_real_speedup(
    summary: pd.DataFrame,
) -> None:

    ordered_datasets = [
        "sars_cov_2",
        "mycoplasma_genitalium",
        "bacillus_subtilis",
        "ecoli_k12",
    ]

    sais = summary[
        summary["implementation"] == "sais"
    ].copy()

    sais["dataset"] = pd.Categorical(
        sais["dataset"],
        categories=ordered_datasets,
        ordered=True,
    )

    sais = sais.sort_values("dataset")

    labels = [
        REAL_DATASET_LABELS.get(dataset, dataset)
        for dataset in sais["dataset"]
    ]

    speedups = sais[
        "speedup_vs_optimized"
    ].tolist()

    positions = range(len(labels))

    fig, ax = plt.subplots(figsize=(9, 6))

    ax.bar(
        positions,
        speedups,
    )

    ax.axhline(
        y=1.0,
        linestyle="--",
        linewidth=1,
    )

    ax.set_title(
        "SA-IS Speedup over Optimized Prefix-Doubling "
        "on Real Genomes"
    )

    ax.set_xlabel("Dataset")
    ax.set_ylabel("Speedup")

    ax.set_xticks(list(positions))
    ax.set_xticklabels(
        labels,
        rotation=20,
        ha="right",
    )

    for position, speedup in zip(
        positions,
        speedups,
    ):
        ax.text(
            position,
            speedup,
            f"{speedup:.2f}x",
            ha="center",
            va="bottom",
        )

    fig.tight_layout()

    fig.savefig(
        REAL_SPEEDUP_PLOT,
        dpi=300,
    )

    plt.close(fig)

def print_sais_results(
    summary: pd.DataFrame,
    title: str,
) -> None:
    sais = summary[
        summary["implementation"] == "sais"
    ].sort_values("sequence_length")

    print()
    print(title)
    print("-" * len(title))

    for _, row in sais.iterrows():
        dataset = row["dataset"]

        print(
            f"{dataset}: "
            f"{row['suffix_array_time_ms']:.3f} ms | "
            f"{row['speedup_vs_baseline']:.2f}x vs baseline | "
            f"{row['speedup_vs_optimized']:.2f}x vs optimized"
        )


def main() -> None:
    PLOT_DIR.mkdir(parents=True, exist_ok=True)

    synthetic = pd.read_csv(SYNTHETIC_INPUT)
    real = pd.read_csv(REAL_INPUT)

    validate_repeat_counts(
        synthetic,
        "Synthetic benchmark",
    )

    validate_repeat_counts(
        real,
        "Real benchmark",
    )

    synthetic_summary = create_summary(synthetic)
    real_summary = create_summary(real)

    synthetic_summary.to_csv(
        SYNTHETIC_SUMMARY,
        index=False,
    )

    real_summary.to_csv(
        REAL_SUMMARY,
        index=False,
    )

    plot_synthetic(synthetic_summary)
    plot_real(real_summary)
    plot_synthetic_speedup(synthetic_summary,)
    plot_real_speedup(real_summary,)

    print_sais_results(
        synthetic_summary,
        "Synthetic datasets - SA-IS",
    )

    print_sais_results(
        real_summary,
        "Real datasets - SA-IS",
    )

    print()
    print("Written:")
    print(f"  {SYNTHETIC_SUMMARY}")
    print(f"  {REAL_SUMMARY}")
    print(f"  {SYNTHETIC_PLOT}")
    print(f"  {REAL_PLOT}")
    print(f"  {SYNTHETIC_SPEEDUP_PLOT}")
    print(f"  {REAL_SPEEDUP_PLOT}")


if __name__ == "__main__":
    main()