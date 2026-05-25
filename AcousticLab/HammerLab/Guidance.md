# HammerLab

HammerLab is a small offline observer for the C++ hammer/string experiments.

It instantiates a model, runs it frame by frame, records the hammer state into CSV, and renders the force trace as SVG.

The C++ lab entry files use the `.cpp.lab` suffix so Xcode does not auto-compile them into the app target. Generated files and paper screenshots live in hidden `.Generated` / `.References` folders, and the temporary HammerLab binary is built under `/private/tmp/bBpiano_HammerLab_Build`, so Xcode does not copy lab-only files into the app bundle.

## Current Scripts

There are two main force-trace scripts. Both use direct hammer velocity `vin` in m/s-like units.

Fresh-pull note: this repository currently still instantiates `HammerModel` in `main.cpp.lab`. The `--model baseline|bank` flag is already accepted so the scripts and generated filenames match the previous experiment, but the two modes will only diverge after the new Bank mode is added to `HammerModel`.

Argument order:

```text
midi vin duration probe yMax
```

Run the baseline-labelled path:

```bash
AcousticLab/HammerLab/run_baseline_force_case.sh 60 4 0.002 0.7 25
```

Run the Bank-labelled path:

```bash
AcousticLab/HammerLab/run_bank_force_case.sh 60 4 0.002 0.7 25
```

Overlay a generated C4 CSV on top of Bank Fig. 5.4:

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh AcousticLab/HammerLab/.Generated/bank_force_midi60_vin4_s0.002_p0.7.csv
```

The overlay script assumes the Fig. 5.4 axes are `0-2 ms` and `0-25 N`, and writes:

```text
AcousticLab/HammerLab/.Generated/bank_force_midi60_vin4_s0.002_p0.7_over_fig54.svg
```

If you want a custom output name:

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh input.csv output.svg
```

Arguments:

- `60`: MIDI note number. In this project, C4 is MIDI 60 and C5 is MIDI 72.
- `4`: initial hammer velocity `vin`. For Bank Fig. 5.4-style C4 tests, use `vin=4`.
- `0.002`: duration in seconds.
- `0.7`: probe position on the string for the `sample` columns. The force plot itself uses `F`.
- `25`: SVG y-axis maximum in Newtons.

### `vin` vs `velocity`

The current force-trace scripts use `vin`, not MIDI-style `velocity`.

- `vin`: direct initial hammer velocity assigned to the hammer model's `v_in`. For example, `vin=4` means the hammer starts with `v_in = 4`.
- `velocity`: the older MIDI-style argument used by legacy HammerLab scripts such as `run_case.sh`. In that old path, HammerLab converted it with `v_in = velocity * 2.0`, so `velocity=2` produced `v_in=4`.

Do not mix the two when comparing plots. For Bank Fig. 5.4-style tests, use:

```bash
AcousticLab/HammerLab/run_bank_force_case.sh 60 4 0.002 0.7 25
```

not `60 2 ...`, because the second argument is now `vin` directly.

For a Bank Fig. 5.7-style C5 test with `v_h0 = 6 m/s`:

```bash
AcousticLab/HammerLab/run_bank_force_case.sh 72 6 0.0022 0.7 50
```

Outputs:

- `AcousticLab/HammerLab/.Generated/baseline_force_midi60_vin4_s0.002_p0.7.csv`
- `AcousticLab/HammerLab/.Generated/baseline_force_midi60_vin4_s0.002_p0.7.svg`
- `AcousticLab/HammerLab/.Generated/bank_force_midi60_vin4_s0.002_p0.7.csv`
- `AcousticLab/HammerLab/.Generated/bank_force_midi60_vin4_s0.002_p0.7.svg`

## What The CSV Means

- `F`: hammer/string contact force.
- `dy`: hammer felt compression.
- `v_in`: hammer velocity state.
- `dv`: relative velocity term used by the current force update.
- `sigma`: Gaussian width in the current HammerModel path; expected to become unused or zero in the isolated Bank single-point path.
- `sample`: summed velocity at the selected probe position.
- `sample_a`, `sample_b`, `sample_c`: individual string probe values. In the future single-string Bank mode, `sample_b` and `sample_c` should be zero.

The CSV/SVG show a local hammer/string lab path. They are not the final app audio after all voices, realtime scheduling, damping, mixing, and output processing.

## Baseline vs Bank

Both current scripts instantiate `HammerModel` in this fresh pull. The mode names are reserved for the next implementation step:

- `baseline`: should preserve the current/original Gaussian multi-string HammerModel behavior.
- `bank`: should become the isolated Bank-style path with velocity upsampling, hammer substeps at `Ts / 2`, force downsampling, and staggered single-point injection at `left[M_int]` / `right[M_int + 1]`.

Important time-scale note:

- `StringModel::Ts = 1 / 44100` is the DWG/string sample period.
- `Ts / 2 = 1 / 88200` is the Bank hammer substep period.
- Neither is the acoustic period of the string. The acoustic frequency is represented through the delay-line length.

Target Bank string mode: integer-grid lossless/nondispersive DWG for the C4 reference experiment. It should bypass boundary fractional delay, loss, and dispersion filters.

The older `run_case.sh` still exists for the original `HammerModel` observer, but it is not the main Bank experiment path.

## Bank References
Most useful pages:

- `.References/figure 5.4.png`: force curve reference for C4.
- `.References/figure 5.7.png`: single-rate vs multi-rate force comparison.

## Bank Fig. 5.4 Scale

Bank Fig. 5.4 uses a 0-2 ms time window and 0-25 N force axis. To draw the current Bank path on that same scale:

```bash
AcousticLab/HammerLab/run_bank_force_case.sh 60 4 0.002 0.7 25
```

In this project, A4 is MIDI 69, C4 is MIDI 60, and C5 is MIDI 72. The current script takes direct `vin`, so `vin=4` means the hammer is initialized with `v_in = 4`.

To look at a longer decay window while keeping the same force scale:

```bash
AcousticLab/HammerLab/run_bank_force_case.sh 60 4 0.004 0.7 25
```
