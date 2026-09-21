# bBpiano Hammer Benchmark — Stage 1 Result

Date: 2026-06-23

## Stage decision

The stage winner is a compact one-branch nonlinear Maxwell felt with
independent parallel contact state for each unison string:

```text
model       = maxwell1
coupling    = per-string
K scale     = 1.2
P offset    = -0.2
epsilon     = 0.9
tau         = 200 us
```

The three per-string branches share one hammer mass. Their stiffnesses are
divided by the number of strings, so the total static stiffness remains equal
to the configured hammer stiffness instead of being multiplied by two or
three.

## Why this candidate won

The benchmark uses a hard mechanical gate before any audio ranking:

- finite force and state in 24 register/velocity cases;
- observable hammer-plus-string energy ratio below 1.05;
- positive contact duration and force;
- force spectrum must brighten from 1 m/s to 7 m/s.

The historical shared-contact implementation, pure power law, and direct
RT-425 derivative-power candidate all failed the energy gate in the highest
register. The selected Maxwell candidate passed every case with a maximum
observable energy ratio of 1.0.

Among passing candidates, the winner had the lowest A1–A7 attack proxy loss:

| Candidate | Attack proxy loss | Single-note ViSQOL | Median contact |
|---|---:|---:|---:|
| Maxwell per-string, selected | 0.27580 | 3.00259 | 1.372 ms |
| Maxwell per-string, softer | 0.27779 | 2.98227 | 1.383 ms |
| Maxwell per-string, faster | 0.27923 | 2.99939 | 1.395 ms |
| Maxwell shared contact | 0.33339 | — | 2.438 ms |

Pianoteq single notes are used only as a controlled attack proxy. They are not
treated as real-hammer ground truth.

## Twelve-piece real-piano guardrail

The same winner was rendered against the twelve selected MAESTRO/Disklavier
excerpts. ViSQOL improved on all twelve excerpts:

```text
legacy shared mean  = 2.00665
selected mean       = 2.23209
mean improvement    = +0.22544
```

This is encouraging but does not isolate the hammer: the score also contains
string loss, dispersion, pickup/radiation, damper, pedal, and soundboard
errors. Therefore it is a release guardrail, not the hammer fitting target.

CLAP was not included in the numerical decision. The local LAION-CLAP entry
still attempts to fetch the `facebook/bart-base` tokenizer from Hugging Face,
and that request was unavailable in the execution environment. More
importantly, CLAP is a broad semantic embedding and is less sensitive to the
short hammer-string attack than the onset metrics and ViSQOL used here.

## Real-time cost

Apple M4, 44.1 kHz, 88 keys active, median:

```text
legacy shared contact    9.04 us / 22.68 us, 39.86%, PASS
selected per-string      9.67 us / 22.68 us, 42.65%, PASS
```

The selected model costs about 7% more in this full-engine stress test and
still runs at approximately 2.34x real time with all 88 keys active.

## Audition files

Primary level-matched comparisons:

- `outputs/20260623T001110Z/audition_baseline_loudness_matched.wav`
- `outputs/20260623T001110Z/audition_winner_loudness_matched.wav`
- `outputs/20260623T001110Z/scarlatti_30s_baseline_loudness_matched.wav`
- `outputs/20260623T001110Z/scarlatti_30s_winner_loudness_matched.wav`

Raw engine-level files are in the same directory.

## Scope and remaining uncertainty

- `StringModel` was not modified; its existing fractional strike port already
  supplies the required per-string read/injection interface.
- This stage does not prove that `epsilon = 0.9` and `tau = 200 us` are unique
  material constants.
- The benchmark does show that independent string feedback plus a passive
  relaxation branch is a better engineering direction than the previous
  summed-velocity/equal-force structure.
- The next useful data is measured hammer force reconstructed on real strings
  at several registers and velocities. Without it, finer parameter search
  risks fitting Pianoteq and the unfinished output chain rather than felt.
