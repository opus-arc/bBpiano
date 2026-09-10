#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


CLAP_SCRIPT = Path(
    "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Sonic/core/eval/laion_clap/clap_embedding.py"
)


def run_clap(reference: Path, test: Path) -> float:
    result = subprocess.run(
        [
            "python",
            str(CLAP_SCRIPT),
            str(reference),
            str(test),
        ],
        capture_output=True,
        text=True,
    )

    output = result.stdout + "\n" + result.stderr

    matches = re.findall(
        r"(?m)^[-+]?\d*\.\d+(?:[eE][-+]?\d+)?$",
        output,
    )

    if not matches:
        raise RuntimeError(
            f"Failed to parse CLAP score:\n{output[-2000:]}"
        )

    return float(matches[-1])


def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "reference_dir",
        help="Reference wav directory",
    )

    parser.add_argument(
        "test_dir",
        help="Test wav directory",
    )

    args = parser.parse_args()

    reference_dir = Path(args.reference_dir)
    test_dir = Path(args.test_dir)

    ref_files = sorted(reference_dir.glob("*.wav"))

    if not ref_files:
        raise RuntimeError(
            f"No wav files found in {reference_dir}"
        )

    scores = []

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
            score = run_clap(ref, test)
            scores.append(score)

            print(
                f"{ref.stem:<80} "
                f"{score:.9f}"
            )

        except Exception as e:
            print(f"[FAILED] {ref.name}")
            print(e)

    print()
    print("=" * 120)

    if scores:
        print(f"Compared Files : {len(scores)}")
        print(f"Sum Score      : {sum(scores):.9f}")
        print(f"Mean Score     : {sum(scores)/len(scores):.9f}")
        print(f"Min Score      : {min(scores):.9f}")
        print(f"Max Score      : {max(scores):.9f}")

    else:
        print("No valid comparisons found.")


if __name__ == "__main__":
    main()
