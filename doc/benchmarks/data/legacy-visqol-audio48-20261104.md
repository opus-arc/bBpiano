# Historical ViSQOL rerun · 48 kHz audio mode · November 2026

100 retained WAV/reference pairs: Pianoteq 9 (12 performances), SF2 Grand Piano,
L0-100c, L0-beta and L1-Clavier (5 scales, 5 chord tests and 12 performances each).
References are the same files and SHA-256 hashes used for Horowitz:
Pianoteq 9 for scales/chords; Yamaha Disklavier/MAESTRO for performances.

The pyvisqol 0.0.2 compiled API is configured explicitly with 48000 Hz,
`use_speech_scoring=false` and Google's `libsvm_nu_svr_model.txt`.
Librosa 0.11.0 loads complete files at 48 kHz, mono float32; arrays are converted
to float64 for Measure. There is no external alignment, gain fitting, EQ,
normalization, new rendering or excerpt shortening. Original tails and duration
differences remain. Each observation is written immediately and input hashes
are verified again after scoring. A previous Horowitz chord result is repeated
within 1e-9 and a reference/self control must exceed 4.5 before the batch.

Up to four separate scoring processes run concurrently with one native thread
each and a conservative memory admission budget. Every file has a timeout;
failed jobs are retained rather than filled with old or guessed scores.

Most L1 inputs come from the retained 2026-09-19 snapshot, with byte-verified
external or cached aliases where available. For 081 (Prel. Chor. Fug.) and
089 (Sonata in F Minor Op.5), the user authorized recovery using available
Clavier files; retained WAVs in the external L1 archive are scored directly.
Their exact identity to the unavailable iCloud originals is not established.
No file was rerendered; each actually scored input has its own recorded hash. The old notes say ViSQOL used soundboard OFF and CLAP used ON, but
the original binary/switch is not independently certified for every retained
WAV. Other historical render settings also differ from Horowitz/C1425.
The new table therefore shares a scoring mode, not identical synthesis chains.
Full-file observations should not be interpreted as a listener preference test.

The historical `scores.csv` and all CLAP observations remain untouched.
L0-alpha has no identified acoustic corpus; it has no imputed score.
Pizzicato is not reintroduced into the user's chosen comparison columns.

[Full-precision results](legacy-visqol-audio48-20261104.csv) ·
[Hashes, metadata, controls and summaries](legacy-visqol-audio48-20261104.json)
