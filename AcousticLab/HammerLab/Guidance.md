# HammerLab

HammerLab is a small offline observer for the current C++ `HammerModel`.

It does not change the hammer/string connection. It instantiates the current model, runs it frame by frame, and records the hammer state into CSV. The SVG plot is generated from that CSV.

The C++ lab entry files use the `.cpp.lab` suffix so Xcode does not auto-compile them into the app target. Generated files and paper screenshots live in hidden `.Generated` / `.References` folders, and the temporary HammerLab binary is built under `/private/tmp/bBpiano_HammerLab_Build`, so Xcode does not copy lab-only files into the app bundle.

## Run One Case

From the project root:

```bash
AcousticLab/HammerLab/run_case.sh 69 80 0.08
```

Arguments:

- `69`: MIDI note number. Change this to look at another note/string set.
- `80`: MIDI velocity.
- `0.08`: duration in seconds.
- optional `0.7`: probe position on the string, from bridge-side `0.0` to far end `1.0`.

Example with explicit probe position:

```bash
AcousticLab/HammerLab/run_case.sh 69 80 0.08 0.2
```

Outputs:

- `AcousticLab/HammerLab/.Generated/midi69_v80_p0.7.csv`
- `AcousticLab/HammerLab/.Generated/midi69_v80_p0.7.svg`

## What The CSV Means

- `F`: hammer/string contact force.
- `dy`: hammer felt compression.
- `v_in`: hammer velocity state.
- `dv`: relative velocity term used by the current force update.
- `sigma`: Gaussian force-spreading width.
- `sample`: summed velocity at the selected probe position.
- `sample_a`, `sample_b`, `sample_c`: individual unison-string probe values.

The CSV/SVG show the current HammerModel plus its local string interaction path: the hammer computes force, distributes that force to its paired strings, advances those strings, and records the state. It is not the final app audio after all voices, realtime scheduling, mixing, damping, and output processing.

## How To Play With It

Try notes:

```bash
AcousticLab/HammerLab/run_case.sh 45 80 0.12
AcousticLab/HammerLab/run_case.sh 63 80 0.08
AcousticLab/HammerLab/run_case.sh 69 80 0.08
AcousticLab/HammerLab/run_case.sh 84 80 0.05
```

Try velocities:

```bash
AcousticLab/HammerLab/run_case.sh 69 30 0.08
AcousticLab/HammerLab/run_case.sh 69 80 0.08
AcousticLab/HammerLab/run_case.sh 69 120 0.08
```

Try string positions:

```bash
AcousticLab/HammerLab/run_case.sh 69 80 0.08 0.2
AcousticLab/HammerLab/run_case.sh 69 80 0.08 0.5
AcousticLab/HammerLab/run_case.sh 69 80 0.08 0.7
```

## Bank References
Most useful pages:

- `.References/figure 5.4.png`: force curve reference for C4.
- `.References/figure 5.7.png`: single-rate vs multi-rate force comparison.

## Bank Fig. 5.4 Scale

Bank Fig. 5.4 uses a 0-2 ms time window and 0-25 N force axis. To draw the current model on that same scale:

```bash
AcousticLab/HammerLab/run_bank_force_case.sh 60 2 0.002
```

In this project, A4 is MIDI 69, so C4 is MIDI 60. MIDI 72 is C5. The app maps the HammerLab velocity argument to hammer velocity by `v_in = velocity * 2.0`, so `velocity=2` corresponds to about 4 m/s, matching the impact velocity described near Bank Fig. 5.4.

This command does not change the model. It only rerenders the observed `F(t)` in the same coordinate window as the paper.

To look at a longer decay window while keeping the same force scale:

```bash
AcousticLab/HammerLab/run_bank_force_case.sh 60 2 0.004 0.7 25
```

Argument order:

```text
midi velocity seconds probe yMax
```

The current simplified HammerLab does not override `K/P/m`. It observes whatever constants are currently compiled into `HammerModel`.
