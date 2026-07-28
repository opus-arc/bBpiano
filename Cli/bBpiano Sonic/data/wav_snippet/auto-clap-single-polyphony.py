from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parent
AUTO_CLAP = Path("/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Sonic/core/eval/laion_clap/auto_clap.py")

REF_ENGINE = "Pianoteq 9"
ENGINES = [
    "bBpiano L0-100c",
    "bBpiano L0-beta",
#    "bBpiano L0-Pizzicato",
    "SF2 Grand Piano",
]

LEVELS = [
    "L1 Single",
    "L2 Polyphony",
]

for engine in ENGINES:
    for level in LEVELS:
        ref_dir = ROOT / REF_ENGINE / level
        test_dir = ROOT / engine / level

        if not test_dir.exists():
            print(f"Skip missing: {test_dir}")
            continue

        print(f"\n=== CLAP | {engine} | {level} ===")
        subprocess.run(
            [sys.executable, str(AUTO_CLAP), str(ref_dir), str(test_dir)],
            check=True
        )
