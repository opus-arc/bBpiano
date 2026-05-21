# HammerLab

HammerLab is a small offline observer for the current C++ `HammerModel`.

It does not change the hammer/string connection. It instantiates the current model, runs it frame by frame, and records the hammer state into CSV. The SVG plot is generated from that CSV.

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

- `AcousticLab/HammerLab/Generated/midi69_v80_p0.7.csv`
- `AcousticLab/HammerLab/Generated/midi69_v80_p0.7.svg`

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

- `bank_hammer_page_07.png`: Bank Fig. 5.4, force curve reference for C4.
- `bank_hammer_page_08.png`: Bank Fig. 5.5, core hammer model.
- `bank_hammer_page_09.png`: Bank Fig. 5.6 and Eq. 5.1, multi-rate hammer-waveguide connection.
- `bank_hammer_page_11.png`: Bank Fig. 5.7, single-rate vs multi-rate force comparison.
