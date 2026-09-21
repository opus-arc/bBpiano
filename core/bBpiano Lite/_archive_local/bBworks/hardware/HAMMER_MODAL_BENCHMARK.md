# bBpiano hammer modal-energy benchmark

## What this benchmark measures

This benchmark isolates the excitation produced by `HammerModel`. It does not
run the force through `StringModel`, the waveguide delay, boundary loss,
dispersion, a soundboard model, or an output filter.

For an ideal fixed-fixed string,

\[
y(x,t)=\sum_n q_n(t)\sin(n\pi x/L),
\qquad
M_n=\mu L/2,
\qquad
\omega_n=2\pi n f_0 .
\]

A point hammer force at normalized position \(r_h=x_h/L\) drives mode \(n\)
with \(\phi_n(r_h)=\sin(n\pi r_h)\). After the force pulse ends,

\[
E_n
=\frac12 M_n\left[\dot q_n^2(t_c)+\omega_n^2q_n^2(t_c)\right]
=\frac{\phi_n^2(r_h)}{2M_n}
  \left|\int F(t)e^{-i\omega_n t}\,dt\right|^2 .
\]

The second expression is what the test evaluates. It gives the same energy as
the state-space expression in the supplied image, but avoids observing the
waveguide after the collision.

## Fingerprint and distance

The fingerprint is the normalized vector

\[
p_n=E_n/\sum_k E_k .
\]

The primary distance is the square root of the Jensen-Shannon divergence using
base-2 logarithms. It is bounded:

- `0`: identical modal-energy distributions;
- `1`: disjoint distributions.

The report also includes a `0–100` similarity score, total-variation distance,
log-energy RMSE in dB, and modal-energy centroid error. These are kept
separate rather than hidden in an arbitrary weighted score.

The unnormalized sum may be divided by incident hammer energy as a scale
diagnostic:

\[
\frac{\sum_n E_n}{\frac12 m_h v_h^2}.
\]

This ratio is **not** a closed-system energy-conservation test. The recorded
force pulse is replayed into a separate ideal, lossless modal string; in that
counterfactual system the force is an externally prescribed actuator and its
work depends on the ideal string velocity. It may therefore differ from the
energy transferred in the original coupled waveguide and may exceed the
original hammer's incident energy without proving that `HammerModel` created
energy. Closed-system passivity must instead be checked in
`hammer_energy_guard.cpp`, using the coupled hammer and string states (including
the fractional-allpass storage energy).

## Authoritative comparison

For a hammer-only physical comparison, compare two force logs:

```sh
python hammer_modal_benchmark.py \
  --force-log candidate_force.txt \
  --target-force-log measured_target_force.txt \
  --hammer-mass-kg 0.015 \
  --impact-velocity 2.0 \
  --self-test \
  --output-dir benchmark
```

This is the preferred benchmark because both fingerprints have the same
physical meaning.

## Comparing a target audio recording

### Formal WAV benchmark: paired velocities

A single microphone WAV cannot separate the hammer excitation from the
soundboard and recording transfer function. The formal WAV benchmark therefore
uses the same note and recording setup at two or more measured hammer
velocities:

\[
P_n(v)=|H_n|^2 E_n(v),
\]

so that

\[
10\log_{10}\frac{P_n(v)}{P_n(v_0)}
-
10\log_{10}\frac{E_n(v)}{E_n(v_0)}
\]

cancels the fixed per-mode transfer \(H_n\). Create a manifest:

```csv
velocity_m_s,force_log,reference_wav,onset_ms
0.72,force_0p72.txt,target_0p72.wav,
1.31,force_1p31.txt,target_1p31.wav,
2.00,force_2p00.txt,target_2p00.wav,
4.00,force_4p00.txt,target_4p00.wav,
```

Run:

```sh
python hammer_modal_benchmark.py \
  --paired-manifest paired_velocity_manifest.csv \
  --baseline-velocity 0.72 \
  --audio-kind microphone \
  --frequency-mode measured \
  --minimum-snr-db -20 \
  --f0 261.626 \
  --modes 13 \
  --output-dir paired_benchmark
```

The primary result is modal-shape RMSE in dB after removing any per-file
constant gain offset. Raw level RMSE and the detected gain offset are also
reported. Do not normalize, compress, limit, denoise, or apply automatic gain
control to individual recordings.

`--frequency-mode measured` estimates one fixed modal-frequency grid from the
baseline/reference WAV and reuses it for every velocity. This avoids treating
small tuning or inharmonicity offsets as hammer-model error. Use
`--frequency-mode harmonic` for synthetic self-tests or when the target is
known to be exactly harmonic.

### Exploratory single-WAV proxy

```sh
python hammer_modal_benchmark.py \
  --force-log candidate_force.txt \
  --reference-wav target.wav \
  --audio-kind microphone \
  --f0 261.626 \
  --modes 32 \
  --output-dir benchmark
```

The WAV must be uncompressed PCM or IEEE-float WAV. The analyzer detects the
onset, skips the hammer contact region, and estimates the selected partial
amplitudes together with a windowed least-squares sinusoidal fit.

The single-WAV `--audio-kind microphone` result is only an acoustic proxy. A microphone recording
contains the soundboard transfer function, radiation, room, microphone
response, tuning, and string losses. Its numerical distance is reproducible,
but it is not an absolute measurement of HammerModel error.

Physical recovery is possible when the recording is a calibrated string
velocity or displacement measurement at a known point:

```sh
--audio-kind string-velocity
--pickup-position 0.857142857
```

or:

```sh
--audio-kind string-displacement
--pickup-position 0.857142857
```

Modes close to a pickup-position node are excluded because dividing by a near
zero mode shape is ill-conditioned.

## Target recording protocol

For useful repeatability:

1. Record one isolated string or one undamped unison string if possible.
2. Supply the actual hammer velocity and note fundamental.
3. Keep microphone, preamp gain, lid, room, and piano unchanged.
4. Record at least 24-bit/96 kHz when upper modes matter.
5. Include at least 50 ms of silence before the strike and 300 ms afterward.
6. Record at least five strikes per velocity. Compare the median fingerprint
   and report strike-to-strike uncertainty.
7. Prefer a bridge/string velocity sensor or a force-sensor hammer when the
   goal is physical hammer identification rather than acoustic similarity.

## Why Loris is optional

Loris is useful for tracking time-varying, slightly inharmonic partials in a
recorded piano note. It estimates sinusoidal frequency, amplitude, phase, and
bandwidth envelopes. Those amplitudes are observation-domain quantities; they
do not become physical string modal energies until pickup position and the
complete transfer path are compensated.

For this benchmark, simultaneous least-squares fitting at \(nf_0\) is simpler,
deterministic, and exactly matches the requested no-dispersion assumption.
Loris becomes useful later if the benchmark is extended to measured
inharmonic modal frequencies or time-dependent partial decay.
