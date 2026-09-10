

#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


def run_visqol(reference: Path, test: Path) -> float:
    result = subprocess.run(
        [
            "pyvisqol",
            str(reference),
            str(test),
        ],
        capture_output=True,
        text=True,
    )

    output = result.stdout + "\n" + result.stderr

    if result.returncode != 0:
        raise RuntimeError(
            f"pyvisqol failed with exit code {result.returncode}:\n{output[-2000:]}"
        )

    matches = re.findall(
        r"(?m)^\s*([-+]?\d*\.\d+(?:[eE][-+]?\d+)?|[-+]?\d+)\s*$",
        output,
    )

    if not matches:
        raise RuntimeError(
            f"Failed to parse ViSQOL score:\n{output[-2000:]}"
        )

    return float(matches[-1])


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Batch compare same-named WAV files in two folders using pyvisqol."
    )

    parser.add_argument(
        "reference_dir",
        help="Reference WAV directory",
    )

    parser.add_argument(
        "test_dir",
        help="Test WAV directory",
    )

    args = parser.parse_args()

    reference_dir = Path(args.reference_dir).expanduser().resolve()
    test_dir = Path(args.test_dir).expanduser().resolve()

    if not reference_dir.is_dir():
        raise RuntimeError(f"Reference directory not found: {reference_dir}")

    if not test_dir.is_dir():
        raise RuntimeError(f"Test directory not found: {test_dir}")

    ref_files = sorted(reference_dir.glob("*.wav"))

    if not ref_files:
        raise RuntimeError(f"No WAV files found in reference directory: {reference_dir}")

    scores: list[float] = []

    print()
    print("=" * 120)
    print(f"Reference : {reference_dir}")
    print(f"Test      : {test_dir}")
    print("=" * 120)

    for ref in ref_files:
        test = test_dir / ref.name

        if not test.exists():
            print(f"[MISSING] {ref.name}")
            continue

        try:
            score = run_visqol(ref, test)
            scores.append(score)

            print(
                f"{ref.stem:<80} "
                f"{score:.9f}"
            )

        except Exception as exc:
            print(f"[FAILED] {ref.name}")
            print(exc)

    print()
    print("=" * 120)

    if scores:
        print(f"Compared Files : {len(scores)}")
        print(f"Sum Score      : {sum(scores):.9f}")
        print(f"Mean Score     : {sum(scores) / len(scores):.9f}")
        print(f"Min Score      : {min(scores):.9f}")
        print(f"Max Score      : {max(scores):.9f}")
    else:
        print("No valid comparisons found.")


if __name__ == "__main__":
    main()
