# bBpiano Hammer–String Benchmark

This benchmark ranks hammer material and coupling candidates in three layers:

1. **Mechanical safety (hard gate)**
   - finite state and force;
   - maximum observable hammer-plus-string energy ratio below 1.05;
   - positive contact force and contact duration.
2. **Hammer–string behavior (primary diagnostics)**
   - contact time across register and impact velocity;
   - peak force, force impulse, rebound velocity;
   - energy transferred to the strings and first bridge-bound pass;
   - force-spectrum brightening with impact velocity;
   - multi-string force spread for per-string contact candidates.
3. **Attack similarity (ranking proxy)**
   - seven isolated A notes, MIDI 33–105, velocity 80;
   - onset-aligned attack envelope;
   - log spectrum, spectral centroid, and fundamental-relative band balance;
   - Pianoteq single notes are used only as a controlled engineering proxy,
     not as measured hammer ground truth.

The twelve MAESTRO/Disklavier excerpts, CLAP, and ViSQOL remain final-system
guardrails. They are deliberately not the inner-loop objective because they are
strongly affected by soundboard, radiation, damping, pedals, and note lifetime.

## Run

```sh
cd /Users/opusarc/Projects/XCodeProjects/bBpiano
hammer_benchmark/build.sh
/Users/opusarc/miniforge3/bin/python hammer_benchmark/run_benchmark.py
```

Outputs are written under `hammer_benchmark/outputs/<run-id>/`.

## Runtime overrides

The engine reads these once while each `HammerModel` is constructed:

- `BBPIANO_HAMMER_MODEL=legacy|power|rt425|maxwell1`
- `BBPIANO_HAMMER_COUPLING=shared|per-string`
- `BBPIANO_HAMMER_K_SCALE`
- `BBPIANO_HAMMER_P_OFFSET`
- `BBPIANO_HAMMER_EPSILON`
- `BBPIANO_HAMMER_TAU_US`

They make the historical implementation reproducible and let the offline
benchmark compare candidates without recompiling the audio engine.
