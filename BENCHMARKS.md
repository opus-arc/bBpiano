# bBpiano — Benchmarks & Acoustic Evaluation

*ViSQOL / LAION-CLAP · L0 / L1 / L2-Horowitz comparison*

This report documents measured audio-similarity scores from earlier bBpiano development stages alongside the L2-Horowitz evaluation. It preserves the original per-piece observations, recalculates summary statistics directly from those observations, and distinguishes the limits of each metric. It does **not** present CPU throughput or real-time performance measurements.

本文整理 bBpiano 各研发阶段及 L2-Horowitz 的音频相似性评测数据，完整保留逐曲目记录，重新核算汇总统计，并明确各项指标的适用边界。**本文不包含 CPU 吞吐量或实时计算性能的测量结果。**

**In this report / 内容导览**  
[Overview](#1-overview) · [Methods](#2-evaluation-methodology) · [Results](#3-aggregate-results) · [Observations](#4-observations--limitations) · [Reproducibility](#5-reproducibility--data) · [ViSQOL detail](#appendix-a--visqol-audio-48-khz-detailed-results) · [CLAP detail](#appendix-b--laion-clap-detailed-results)

---

## 1. Overview

The objective is to compare the behavior of several piano synthesis implementations on aligned audio material, and to identify where further research may be useful. **A higher score indicates stronger similarity to the chosen reference under that particular metric—not a definitive judgment of physical accuracy or musical quality.**

这些实验旨在比较不同钢琴合成实现对于参考材料的响应，并定位可能值得进一步研究的问题。**较高分数仅表示在特定评价方法下，与对应参考音频更加相似；不能据此直接判定物理精度或音乐品质的优劣。**

| Item | Scope |
| :-- | :-- |
| Evaluation methods | ViSQOL Audio (48 kHz); LAION-CLAP cosine similarity |
| Test groups | Scale; Polyphony; Performance |
| Performance excerpts | 12 selections associated with MAESTRO |
| Scale & Polyphony reference | Pianoteq 9 |
| Performance reference | Yamaha Disklavier recordings from the MAESTRO dataset |
| Compared engines | Pianoteq 9 (Performance only); SF2 Grand Piano; bBpiano L0-100c; L0-beta; L1-Clavier; L2-Horowitz |
| Data status | All ViSQOL rows re-evaluated in audio mode, 48 kHz; historical CLAP retained |

> **Configuration / 配置：** Archived L1 notes record soundboard OFF for ViSQOL and ON for CLAP. This rerun uses retained L1 WAVs, including two external archive files documented in the protocol; original render binaries and all switch settings are not independently recoverable. Horowitz uses C1425. The scoring mode is now shared, but the original synthesis chains differ.
> 本轮统一评分程序与模式，保留各版已有 WAV；不能据此认定旧版音板和渲染增益与 Horowitz 相同。

## 2. Evaluation Methodology

### 2.1 Reference material

- **Scale** — scale passages covering bass through high treble. The original experiment focused on subsystems other than aliasing; the provided record does not document a dedicated aliasing-control protocol.  
  **音阶**——覆盖低音至极高音域；原始说明重点考察混叠之外的子系统，但未记录专门的混叠控制方案。
- **Polyphony** — chords across the same register ranges, intended to probe coupled behavior.  
  **复音**——跨音域和弦，重点观察多声部与系统耦合的响应。
- **Performance** — 12 musical excerpts evaluated against recorded Yamaha Disklavier references associated with MAESTRO.  
  **演奏**——12 段音乐片段，以 MAESTRO 中 Yamaha Disklavier 的录音为参考。

Pianoteq 9 serves as the **reference**, rather than as a separate scored engine, for the Scale and Polyphony groups. It appears as a scored comparison engine for Performance. The retained WAV filenames and hashes are now recorded; the original excerpt-cutting commands are not recovered.

在音阶与复音测试中，Pianoteq 9 是**参考信号**，因此不作为独立的评分列；在演奏测试中，它则参与评分比较。本轮登记了保留 WAV 的文件名和哈希；原始截取命令仍未完整恢复。

### 2.2 Metrics

**ViSQOL** estimates full-reference perceptual audio quality using comparisons of audio signal representations. Its values here should be read as *similarity estimates under the documented experimental setup*, rather than a universal measure of instrument realism.

**ViSQOL** 使用参考音频与待测音频的信号表征进行比较，以估计感知音频质量。其分数只能在对应实验条件下解读，不能视为普适的钢琴真实感尺度。

**LAION-CLAP** produces learned audio embeddings; the reported statistic is **cosine similarity** between embeddings. It captures a different notion of similarity from ViSQOL, and does not directly measure compliance with acoustic governing equations or physical-model accuracy.

**LAION-CLAP** 基于学习得到的音频嵌入表征计算**余弦相似度**。它与 ViSQOL 衡量的相似性不同，也不能直接检验模型是否满足声学物理规律。

> The two metrics use **different numerical scales**. Their scores must not be added, averaged together, or compared as if they shared a common unit.  
> 两项指标的数值尺度不同，不应相加、混合求均值或直接比较绝对大小。

**Horowitz protocol / 本次口径：** 22 original MIDI files are rendered in full at 88.2 kHz (5 scales, 5 chord tests, 12 performances), retaining the accepted v55 model, original gain, C1425 soundboard and 2Z route for both metrics. No external time shift, gain fit or EQ is applied. CLAP uses the original non-fusion HTSAT-tiny/Roberta API, 630k+AudioSet checkpoint, 48 kHz mono and a recorded seed per pair; it independently selects a 10-second crop from each long file. ViSQOL is reported exclusively in **48 kHz audio mode**, using the official libsvm quality mapping. The reporting mode was selected by the user on 2026-11-04; the already computed audio-mode observations are unchanged.

Historical ViSQOL has been re-evaluated from **100 retained WAV/reference pairs**, using the same 48 kHz audio API, libsvm model and preprocessing as Horowitz. No rerender, EQ, external alignment, normalization or crop was applied. Historical CLAP values are unchanged. [Historical rerun protocol](doc/benchmarks/data/legacy-visqol-audio48-20261104.md) · [Horowitz protocol](doc/benchmarks/data/horowitz-protocol-20261104.md).

旧版 100 组 WAV 已使用相同的 48 kHz 音频模式重测；旧版渲染配置与当前音板仍有差别，同一评分模式不等于合成条件完全相同。CLAP 原始记录保持不变。

## 3. Aggregate Results

All aggregate statistics in this section refer **only to the 12 Performance excerpts**. The arithmetic mean, minimum, and maximum are recomputed from the individual values in Appendices A and B. Values are rounded to four decimal places; the detailed tables and accompanying CSV preserve the original supplied precision.

本节统计量**仅基于 12 段演奏测试**，由附录中的逐曲目数值重新计算。下表保留四位小数；附录与 CSV 保留原始记录的数值精度。

### 3.1 ViSQOL Audio · 48 kHz · Performance (n = 12)

| Engine | Pianoteq 9 | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | **bBpiano L1-Clavier** | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: | --: |
| Mean | 3.9688 | 3.9090 | 3.7364 | 3.7301 | 2.8795 | 3.7273 |
| Min | 2.3916 | 2.3846 | 2.1675 | 2.1154 | 1.5884 | 2.1319 |
| Max | 4.3762 | 4.4079 | 4.1420 | 4.3366 | 3.6455 | 4.2293 |

### 3.2 LAION-CLAP · Performance (n = 12)

| Engine | Pianoteq 9 | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | **bBpiano L1-Clavier** | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: | --: |
| Mean | 0.8045 | 0.8283 | 0.3213 | 0.4628 | 0.5833 | 0.6838 |
| Min | 0.6436 | 0.7059 | 0.1505 | 0.1436 | 0.3961 | 0.5204 |
| Max | 0.9413 | 0.8852 | 0.5253 | 0.6226 | 0.7419 | 0.8438 |

### 3.3 Register profiles · Scale & Polyphony

Register-wise results are listed in Appendices A and B. / 逐音域结果见附录 A、B。

## 4. Observations & Limitations

**What the recorded results show / 已记录的现象**

- L2-Horowitz completed all 22 audio comparisons without a failed metric. Its 12-piece Performance mean is **3.7273** in ViSQOL Audio (48 kHz), and **0.6838** in CLAP. Both use the same fixed C1425 output.
- Historical CLAP values are retained with their original settings; L1-Clavier has a Performance mean of **0.5833**. The different software/crop records prevent a controlled claim of improvement.
- All 100 historical ViSQOL pairs now have finite 48 kHz audio-mode results. No missing value was imputed.

本轮 22 个文件的两项公开指标全部完成。Horowitz 演奏组 ViSQOL Audio（48 kHz）均值为 **3.7273**、CLAP 均值为 **0.6838**。旧版 100 组 ViSQOL 已按相同模式重测并填入原表。

**Interpretation boundaries / 解释边界**

- The excerpts do not establish population-level or listener-tested superiority. No confidence intervals, independent listening-study outcomes, or repeated-measurement distributions are supplied.
- The original notes report CLAP score variation of approximately **5–10%** across repeated evaluations, with occasional deviations over **30%**. Without logs, seeds, checkpoint identifiers, and preprocessing records, these remain **project observations**, not a characterized intrinsic variance of the CLAP model.
- ViSQOL and CLAP scores are **not** measurements of real-time DSP throughput, latency, CPU load, memory use, or stability under polyphonic stress.
- The effect of enabling the L1 soundboard cannot be inferred causally from a comparison between two different metrics or their aggregate means.

这些测试并未构成独立听感实验，也未提供置信区间或完整的重复测量分布。CLAP 的波动幅度来自原始项目记录，尚不能据此认定模型固有的统计方差。本报告亦不能用于推断 CPU 负载、延迟或复音压力下的实时稳定性。

## 5. Reproducibility & Data

The original notes identify the reference family, excerpt count, metric names, and L1 soundboard switches. They do **not** specify every parameter needed for an independently reproducible run. The following information should be recorded in subsequent evaluations rather than inferred retrospectively.

原始资料提供了参考数据集、片段数量、指标名称及 L1 音板设置，但不足以完整复现实验。后续测试宜补充以下信息，不应对旧数据进行事后猜测。

| Item | Status in archival record |
| :-- | :-- |
| Dataset family | MAESTRO / Yamaha Disklavier noted |
| Exact excerpt IDs, MIDI files & boundaries | Not specified |
| bBpiano commit IDs / binary build identifiers | Not specified |
| ViSQOL implementation/version/model, input settings | Not specified |
| LAION-CLAP checkpoint/version/seed | Not specified |
| Sample rate, channel mapping, normalization, alignment | Alignment claimed for ViSQOL; detailed procedure not specified |
| Repeat count & dispersion | CLAP variability described; raw repeat logs not supplied |
| L1 ViSQOL configuration | `soundboard_active_ = false` |
| L1 CLAP configuration | `soundboard_active_ = true` |

**Machine-readable records:** [`doc/benchmarks/data/scores.csv`](doc/benchmarks/data/scores.csv) — 200 metric scores, preserving the originally supplied numeric precision, grouped by metric, workload, excerpt and engine. This file is a historical archive, not the current 48 kHz comparison; its mode metadata is incomplete. Current rerun and Horowitz observations are linked below.

**Horowitz records:** [22-file scores](doc/benchmarks/data/horowitz-acoustic-20261104.csv) · [Input/model hashes, versions and controls](doc/benchmarks/data/horowitz-reproducibility-20261104.json). Files with any metric failure: 0/22; no missing values imputed. The CSV reports CLAP and ViSQOL Audio (48 kHz) only.

**Historical audio-mode rerun:** [100 full-precision scores](doc/benchmarks/data/legacy-visqol-audio48-20261104.csv) · [Hashes, file metadata, controls and summaries](doc/benchmarks/data/legacy-visqol-audio48-20261104.json). Five engines: 12 Pianoteq 9 performances and 22 files each for SF2, L0-100c, L0-beta and L1-Clavier.

**Historical source archive:** the original per-piece observations and their prior arithmetic corrections remain in `scores.csv` and Git history. They are not relabeled as 48 kHz audio-mode results. / 原始旧分数保留在档案与 Git 历史中，不改称本轮音频模式结果。

---

## Appendix A — ViSQOL Audio (48 kHz) Detailed Results

**Reference / 参考音频:** Scale and Polyphony → Pianoteq 9; Performance → Yamaha Disklavier (MAESTRO). **Mode: audio, 48 kHz. Horowitz soundboard: fixed C1425, active.** All historical columns are newly measured from retained WAVs using the same scoring protocol. The reference for Scale and Polyphony is not a scored column.

### A.1 Scale · 音阶

| Piece | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | bBpiano L1-Clavier | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: |
| Bass_scale | 4.424200380 | 4.363051741 | 4.297579136 | 4.443580299 | 4.248367265 |
| Tenor_scale | 4.387839055 | 4.166121111 | 4.218146538 | 4.329735597 | 4.247230156 |
| Middle_scale | 4.396494369 | 4.291327060 | 4.228124523 | 4.318879110 | 4.346348807 |
| Treble_scale | 4.413414240 | 4.321254142 | 4.267068697 | 4.256710993 | 4.347853981 |
| High Treble_scale | 4.278597090 | 4.249721369 | 4.254644204 | 4.214875883 | 4.268506549 |

### A.2 Polyphony · 复音

| Piece | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | bBpiano L1-Clavier | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: |
| Bass_chords | 4.075318486 | 4.049459826 | 3.828888219 | 4.071424752 | 3.917348060 |
| Tenor_chords | 4.302181353 | 3.848413399 | 4.081010335 | 4.147169668 | 4.017783996 |
| Middle_chords | 4.429507354 | 4.207200058 | 4.109647382 | 4.228955828 | 4.267530752 |
| Treble_chords | 4.418428484 | 4.290431660 | 4.263959429 | 4.217292317 | 4.359619418 |
| High Treble_chords | 4.300004779 | 4.271707505 | 4.264218900 | 4.217709283 | 4.293531525 |

### A.3 Performance · 演奏

| Piece | Pianoteq 9 | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | bBpiano L1-Clavier | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: | --: |
| Etude-Tableaux Op.39 No.5 | 3.817000078 | 3.672703435 | 3.490518201 | 3.352580247 | 2.426202970 | 3.368613719 |
| Images, Book II "Poissons d'or" | 3.985439610 | 3.854889315 | 3.716701942 | 3.700522362 | 2.711158687 | 3.762762687 |
| Piano Sonata "From the Street" | 4.167020216 | 4.109953635 | 3.974758035 | 3.922133144 | 3.497896535 | 3.988232934 |
| Prel. Chor. Fug. | 4.051897892 | 3.977610382 | 3.750905417 | 3.637150956 | 3.121439112 | 3.652724542 |
| Prelude and Fugue in A Minor, S.462/1 | 2.391572157 | 2.384615573 | 2.167470157 | 2.115385134 | 1.588366669 | 2.131912984 |
| Prelude and Fugue in D Major, BWV 874 | 3.982382349 | 4.005645814 | 3.951628280 | 3.925495159 | 2.609105729 | 3.940715499 |
| Sonata No.28 Op.101 | 4.164191494 | 4.104117220 | 3.982605356 | 4.008843616 | 3.206676663 | 3.990819107 |
| Sonata No.4 Op.30 | 4.173010568 | 4.155450951 | 3.935641037 | 3.995892135 | 3.194341112 | 3.950166347 |
| Sonata in B Minor | 4.037381183 | 3.918462097 | 3.635552545 | 3.567503173 | 2.898926733 | 3.566472780 |
| Sonata in D Major K.576 | 4.376154302 | 4.407896685 | 4.142024515 | 4.336581542 | 2.630235775 | 4.229334430 |
| Sonata in D Minor K.141 | 4.194408963 | 4.093317904 | 3.987535340 | 4.140206168 | 3.023627990 | 4.096353026 |
| Sonata in F Minor Op.5 | 4.284997746 | 4.223109551 | 4.101510002 | 4.059178430 | 3.645469096 | 4.049194548 |

---

## Appendix B — LAION-CLAP Detailed Results

**Reference / 参考音频:** Scale and Polyphony → Pianoteq 9; Performance → Yamaha Disklavier (MAESTRO). **L1-Clavier soundboard: on.** Values are cosine similarities in the selected embedding space. Bold entries retain the highest value among the historical L0/L1-era scored engines.

### B.1 Scale · 音阶

| Piece | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | bBpiano L1-Clavier | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: |
| Bass_scale | **0.829674840** | 0.591492534 | 0.387630433 | 0.702944458 | 0.790994346 |
| Tenor_scale | **0.896856070** | 0.371731043 | 0.300049216 | 0.716075599 | 0.617875397 |
| Middle_scale | **0.720027626** | 0.233823359 | 0.168149158 | 0.619980037 | 0.838853061 |
| Treble_scale | **0.718633533** | 0.192326903 | 0.166393086 | 0.524722159 | 0.656388104 |
| High Treble_scale | **0.746628761** | 0.304847121 | 0.317520738 | 0.488264501 | 0.469691724 |

### B.2 Polyphony · 复音

| Piece | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | bBpiano L1-Clavier | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: |
| Bass_chords | 0.737208426 | 0.712322891 | 0.491204530 | **0.901042461** | 0.855468512 |
| Tenor_chords | **0.898284912** | 0.612981141 | 0.465305507 | 0.725399315 | 0.771525502 |
| Middle_chords | **0.782639027** | 0.344589412 | 0.309159577 | 0.654511511 | 0.750000715 |
| Treble_chords | **0.825937271** | 0.233994871 | 0.203228608 | 0.647006929 | 0.709462881 |
| High Treble_chords | **0.679073453** | 0.220188931 | 0.261892319 | 0.536056995 | 0.485908329 |

### B.3 Performance · 演奏

| Piece | Pianoteq 9 | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta | bBpiano L1-Clavier | **bBpiano L2-Horowitz** |
| :-- | --: | --: | --: | --: | --: | --: |
| Etude-Tableaux Op.39 No.5 | 0.7640 | **0.8494** | 0.3476 | 0.5452 | 0.571054816 | 0.546307743 |
| Images, Book II "Poissons d'or" | **0.7816** | 0.7704 | 0.4385 | 0.4862 | 0.544904232 | 0.602656066 |
| Piano Sonata "From the Street" | **0.8993** | 0.8060 | 0.1505 | 0.5820 | 0.710870922 | 0.617153108 |
| Prel. Chor. Fug. | 0.7994 | **0.8852** | 0.4831 | 0.4735 | 0.741906404 | 0.652374387 |
| Prelude and Fugue in A Minor, S.462/1 | **0.9413** | 0.8776 | 0.5253 | 0.6226 | 0.509185374 | 0.843753278 |
| Prelude and Fugue in D Major, BWV 874 | 0.6436 | **0.8413** | 0.1691 | 0.4318 | 0.545421720 | 0.792984366 |
| Sonata No.28 Op.101 | 0.7851 | **0.8338** | 0.3241 | 0.4066 | 0.671635807 | 0.623790681 |
| Sonata No.4 Op.30 | 0.7962 | **0.8154** | 0.4225 | 0.5763 | 0.514670193 | 0.817364156 |
| Sonata in B Minor | **0.7088** | 0.7059 | 0.2329 | 0.5579 | 0.396069169 | 0.520417809 |
| Sonata in D Major K.576 | **0.8754** | 0.8503 | 0.2747 | 0.4318 | 0.608233094 | 0.833300769 |
| Sonata in D Minor K.141 | **0.8897** | 0.8641 | 0.2403 | 0.2966 | 0.544550717 | 0.695075631 |
| Sonata in F Minor Op.5 | 0.7701 | **0.8401** | 0.2468 | 0.1436 | 0.641184807 | 0.660300553 |

---

*Historical evaluation record · bBpiano / bBSonicLab. All rights to the software and its implementation remain governed by the applicable bBpiano license; this document does not amend that license.*
