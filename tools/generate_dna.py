import argparse
import random
from pathlib import Path


def generate_sequence(length: int, seed: int) -> str:
    random.seed(seed)
    alphabet = "ACGT"

    return "".join(
        random.choice(alphabet)
        for _ in range(length)
    )


def write_fasta(
    output_path: Path,
    header: str,
    sequence: str,
    line_width: int = 80
) -> None:
    output_path.parent.mkdir(
        parents=True,
        exist_ok=True
    )

    with output_path.open(
        "w",
        encoding="utf-8"
    ) as output:
        output.write(f">{header}\n")

        for i in range(
            0,
            len(sequence),
            line_width
        ):
            output.write(
                sequence[i:i + line_width]
                + "\n"
            )


def main() -> None:
    parser = argparse.ArgumentParser(
        description=(
            "Generate reproducible random DNA "
            "sequences for benchmarking."
        )
    )

    parser.add_argument(
        "--length",
        type=int,
        required=True,
        help="DNA sequence length."
    )

    parser.add_argument(
        "--seed",
        type=int,
        default=42,
        help="Random seed."
    )

    parser.add_argument(
        "--output",
        type=Path,
        required=True,
        help="Output FASTA file."
    )

    args = parser.parse_args()

    if args.length < 1:
        raise ValueError(
            "Sequence length must be at least 1."
        )

    sequence = generate_sequence(
        args.length,
        args.seed
    )

    header = (
        f"synthetic_dna_"
        f"length_{args.length}_"
        f"seed_{args.seed}"
    )

    write_fasta(
        args.output,
        header,
        sequence
    )

    print(
        f"Generated {args.length} bases: "
        f"{args.output}"
    )


if __name__ == "__main__":
    main()