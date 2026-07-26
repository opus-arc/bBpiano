#!/usr/bin/env python3
"""Run a reproducible HammerLab2.0 parameter matrix."""

import argparse
import csv
import datetime as dt
import hashlib
import json
import pathlib
import subprocess
import sys


def file_sha256(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_value(root: pathlib.Path, *args: str) -> str:
    result = subprocess.run(
        ["git", *args], cwd=root, text=True, capture_output=True, check=False
    )
    return result.stdout.strip() if result.returncode == 0 else "unavailable"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("matrix", type=pathlib.Path)
    parser.add_argument("--binary", type=pathlib.Path)
    parser.add_argument("--output", type=pathlib.Path)
    args = parser.parse_args()

    project = pathlib.Path(__file__).resolve().parents[1]
    repo = project.parent
    matrix = args.matrix.resolve()
    binary = (args.binary or project / "build" / "hammerlab2").resolve()
    timestamp = dt.datetime.now(dt.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    output = (args.output or project / "runs" / f"matrix_{timestamp}").resolve()
    output.mkdir(parents=True, exist_ok=False)

    with matrix.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    if not rows:
        raise ValueError("matrix has no cases")

    results = []
    for row in rows:
        case_id = row["case_id"]
        case_dir = output / case_id
        command = [
            str(binary), "simulate", "--case-id", case_id,
            "--model", row["model"], "--load", row["load"],
            "--sample-rate", row["sample_rate"], "--oversample", row["oversample"],
            "--duration", row["duration"], "--vin", row["vin"],
            "--mass", row["mass"], "--stiffness-n-mm-p", row["stiffness_n_mm_p"],
            "--exponent", row["exponent"], "--epsilon1", row["epsilon1"],
            "--tau1", row["tau1"], "--epsilon2", row["epsilon2"],
            "--tau2", row["tau2"], "--impedance", row["impedance"],
            "--trace", str(case_dir / "trace.csv"),
            "--summary", str(case_dir / "summary.json"),
        ]
        process = subprocess.run(command, text=True, capture_output=True, check=False)
        if process.returncode != 0:
            sys.stderr.write(process.stdout + process.stderr)
            return process.returncode
        results.append(json.loads((case_dir / "summary.json").read_text()))
        print(process.stdout.strip())

    manifest = {
        "schema_version": 1,
        "created_at_utc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "matrix": str(matrix),
        "matrix_sha256": file_sha256(matrix),
        "binary": str(binary),
        "binary_sha256": file_sha256(binary),
        "git_commit": git_value(repo, "rev-parse", "HEAD"),
        "git_status": git_value(repo, "status", "--short"),
        "case_count": len(results),
        "all_finite": all(item["finite"] for item in results),
        "all_converged": all(item["solver_converged"] for item in results),
    }
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return 0 if manifest["all_finite"] and manifest["all_converged"] else 2


if __name__ == "__main__":
    raise SystemExit(main())
