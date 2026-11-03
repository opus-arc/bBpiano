# Horowitz release evaluation · published metrics

Reporting policy updated at the user's request on 2026-11-04: publish CLAP and
ViSQOL Audio (48 kHz) only. The already computed scores are unchanged. Other
engines without a verified audio-mode rerun have no value in the current ViSQOL
tables. Raw research evidence and historical records remain archived separately.

The accepted v55 model, gain and 88 presets are frozen. The signed, obfuscated
standalone Horowitz executable matches v55 bit for bit in the all-key test.
All 22 listed MIDI files were rendered in full: five scales, five chord tests,
twelve MAESTRO performances. No gain fitting, EQ or post-render normalization.
Input hashes, durations, sample rates and over-fullscale counts are recorded.

CLAP uses the original non-fusion HTSAT-tiny/Roberta API, the 630k+AudioSet
checkpoint, mono 48 kHz loading and int16 round trip. Python/NumPy/Torch seeds
are fixed at 20261104 + row index. The API independently samples 10-second crops
from long files; this is not aligned or full-record waveform reconstruction.
Historical seeds and software are unknown; cross-version comparisons are descriptive.

ViSQOL uses audio mode, mono 48 kHz, use_speech_scoring=False and Google's
libsvm_nu_svr_model.txt quality mapping. No external time shift or gain fitting
is applied. The installed wrapper's audio-constructor typo is bypassed using
its underlying VisqolApi and explicit config. No installed package is modified.
The official guidance recommends short active clips; these retained whole-file
measurements follow the project's full-file comparison and are not listener MOS.

Both evaluators passed identical-input controls. All 22 pairs are complete and
finite. README headlines summarize only the 12-piece Performance group. Means
are arithmetic; different metrics are never combined. Full precision is retained
in CSV. The supplementary earlier diagnostic mode is not publicly reported here.

Official mode documentation: https://github.com/google/visqol#guidelines
