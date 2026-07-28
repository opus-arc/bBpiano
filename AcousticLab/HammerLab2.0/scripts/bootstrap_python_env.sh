#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 -m venv --without-scm-ignore-files "$root/.venv"
"$root/.venv/bin/python" -m pip install --upgrade pip
"$root/.venv/bin/python" -m pip install -r "$root/requirements-lock.txt"
"$root/.venv/bin/python" -c "import numpy; print('HammerLab Python environment ready: NumPy', numpy.__version__)"
