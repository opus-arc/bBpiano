# HammerLab2.0

> [!IMPORTANT]
> 开始设计或实现前先阅读 [NORTH_STAR.md](NORTH_STAR.md)。它定义 HammerLab2.0 的唯一目标、Pianoteq differential/Jacobian 行为辨识方法、候选模型淘汰规则和端到端工程顺序；若其他文档与它冲突，以它为准。

HammerLab2.0 是一个与旧 `AcousticLab/HammerLab` 完全隔离的候选锤模型试验场。它的唯一目标，是找到一个在可观测行为上稳定优于当前 Bank/Hammer-P 基线、能够实时接入 bBpiano digital waveguide 的最简单锤弦接触模型。

> [!IMPORTANT]
> HammerLab2.0 已完成 Pianoteq 自动采集的第一条纵向切片，但还不是已经标定的钢琴锤模型。当前结果只证明本机自动化与 hardness 可观测性成立，不能直接用于改造 bBpiano，也不能据此声称已经复现真实羊毛毡质感或达到 Pianoteq 级别。

本工程未来的黑盒参考对象固定为 **Pianoteq 8**。主比较目标不是单个 WAV 是否相似，而是 Hammer hardness 对各 partial 初始能量的 differential/Jacobian 行为指纹是否趋同。采集验证器会拒绝版本号不是 `8.x.x` 的数据，避免混入其他主版本。但目前尚未建立正式 Pianoteq 8 baseline/hardness 数据集，也尚未完成 partial estimator、Jacobian fingerprint 和端到端候选筛选。

## 当前阶段

目前已经建立的是实验骨架，而不是可投入使用的完整 pipeline：

- 三个可替换的锤毡原型：无记忆幂律、单支路 Stulov 遗传模型、双支路 Prony 松弛模型。
- 无堆分配的逐样本 felt kernel；trace、报告与文件 I/O 位于非实时实验层。
- 隐式接触迭代，以及力、压缩量和内部记忆变量的 trace。
- 峰值力、接触时长、冲量、回弹速度、滞回功、求解残差与 realtime factor 等基础指标。
- 刚性台砧和电阻性 waveguide driving-point load 两种简化测试负载。
- 单元测试、smoke case、CSV 参数矩阵 runner 与实验 manifest/schema。
- 15 组确定性 Pianoteq 8 MIDI 单音探针、采集 manifest 模板和数据集验证器。
- 可直接驱动本机 CrossOver/Pianoteq 8.4.3 的无界面 renderer、可恢复 study runner、逐 case provenance 与严格 WAV gate。
- C4 三层 hardness 的 27-case heartbeat：Piano/Mezzo/Forte 各做 `-0.2/0/+0.2`、每点三次重复，并以残差噪声地板而非 PCM 哈希相等判定重复性。
- 固定 Python/NumPy 版本的本地分析环境 bootstrap。
- 力曲线评估器，以及 onset 对齐后的 0–5、5–20、20–100 ms 分窗音频频谱评估器。
- 14 篇本地参考论文及页数、用途、来源和 SHA-256 校验索引。

这里的刚性台砧和电阻性端口只用于检查原型的数值行为，不是 bBpiano 真实弦模型的 driving-point impedance。当前测试通过仅表示代码能在 seed 参数下稳定执行，不代表物理准确性或听感已经通过验证。

## 尚未完成

HammerLab2.0 目前还不能真正开始用于锤模型生产，关键缺口包括：

- 尚未从论文图表或真实撞击台架获得可用于标定的 `F(t)`、`x(t)`、`F-x` 数据。
- `mass`、`K`、`p`、`epsilon`、`tau` 和端口阻抗等参数仍是启动实验用的 seed。
- 尚未实现连接 bBpiano DWG 弦模型的真实、无所有权、无分配 `StringPort`。
- HammerLab2.0 felt kernel 尚未接入 bbpl 或 bBpiano App 的 `HammerModel`。
- 已锁定自动化 heartbeat 的 `U4 Small` preset、干扰参数和输出链，但尚未冻结最终 oracle preset/condition。
- 已完成 27-case heartbeat，尚未完成 C4 密集 velocity、`±δ/±2δ` 的正式 oracle 批量采集。
- 尚未把 Pianoteq 8 reference 渲染、bbpl candidate 渲染和批量评分接成一条命令。
- 尚未实现受约束参数优化、跨音高/力度 hold-out、leaderboard 和可视化报告。
- 尚未进行最大复音、非收敛 fallback、denormal、NaN、线程安全和音频 callback 压力测试。

在上述环节完成之前，现有输出不能作为 HammerLab2.0 与 Pianoteq 8 行为一致的证据。

## 运行当前数值自检

以下命令只用于确认工程骨架能够编译和运行，不会生成一个可直接使用的钢琴锤模型。

从 bBpiano 仓库根目录执行：

```sh
cd AcousticLab/HammerLab2.0
make test
make smoke
python3 tools/run_matrix.py configs/smoke_matrix.csv
```

生成未来 Pianoteq 8 实验使用的 15 个 MIDI 探针：

```sh
python3 tools/generate_probe_midis.py \
  configs/pianoteq8_probe_matrix.csv \
  data/reference/pianoteq8/midi
```

生成的 `build/`、`runs/`、`reports/` 和 Pianoteq reference 数据默认不进入 Git。

## Pianoteq 8.4.3 自动 heartbeat

这条命令会自动生成 MIDI、通过 CrossOver 无界面调用本机 Pianoteq、注入全部固定参数、渲染 WAV、逐 case 校验并写 manifest；不需要手工导出 WAV：

```sh
cd AcousticLab/HammerLab2.0
make pianoteq-heartbeat
```

当前 study 定义在
[`configs/studies/pt843_c4_hardness_heartbeat.json`](configs/studies/pt843_c4_hardness_heartbeat.json)，默认生成 27 个本地 WAV：

```text
C4 × {velocity 41/Piano, 70/Mezzo, 98/Forte}
   × {hardness -0.2, baseline, +0.2}
   × 3 repeats
```

结果保存在仓库外的本地数据根目录 `/Users/muzhi/Documents/HammerLab2.0LocalData/pianoteq8/8.4.3/pt843_c4_hardness_heartbeat/`，其中 `manifest.json` 记录版本、程序哈希、配置哈希、case checksum 和恢复状态，`heartbeat_report.json` 记录重复残差、hardness 效应和噪声分离度。再次运行同一命令会逐文件复验 checksum 和格式后恢复，不会重复渲染有效 case；`--force` 才会重做全部渲染。

自动链路为：

```text
study JSON
  -> 展开 27 cases
  -> 生成确定性 MIDI
  -> 核对 Pianoteq 版本与二进制 SHA-256
  -> CrossOver 启动独立的 headless Pianoteq
  -> 加载 U4 Small 并用 --set-param 注入参数
  -> 输出 WAV
  -> 格式/静音/削波/checksum gate
  -> case render.json + 增量 manifest
  -> 重复残差与 hardness 效应 gate
  -> heartbeat_report.json + 进程退出码
```

它不会读取或操纵当前可见 Pianoteq 窗口的临时状态。即使 GUI 正打开另一个 instrument，headless 进程仍由 study 明确加载 `U4 Small`、使用 `--no-prefs`，然后应用参数覆盖。

当前参数 ID 与 Pianoteq 8.4.3 UI/参数名称的对应如下：

| ID | Pianoteq 参数 | 当前值 | 作用 |
| --- | --- | --- | --- |
| 6 | Equalizer Switch | Off | 关闭 EQ |
| 29 | Hammer Noise | 0.10 | 降到 UI 允许的最小值；不是完全关闭 |
| 30 | Hammer Hard. Piano | 0.8/1.0/1.2 | 浅层/弱奏区 hardness 扰动 |
| 31 | Hammer Hard. Mezzo | 0.8/1.0/1.2 | 中层/中力度区 hardness 扰动 |
| 32 | Hammer Hard. Forte | 0.8/1.0/1.2 | 深层/强奏区 hardness 扰动 |
| 33–40 | Spectrum Profile 1–8 | 0 | 保持八段 spectrum profile 中性 |
| 52 | Sympathetic Resonance | 0 | 关闭共鸣弦影响 |
| 53 | Duplex Scale Resonance | 0 | 关闭 duplex scale 共鸣 |
| 54 | Quadratic Effect | 0 | 关闭该非线性下游效应 |
| 152/155/156 | Damper/Pedal/Key Release Noise | Off | 关闭机械噪声层 |
| 159 | Bounce Switch | Off | 关闭 mallet bounce |
| 183/192/201 | Eff 1/2/3 Switch | Off | 关闭三个效果槽 |
| 210 | Reverb Switch | Off | 关闭混响 |
| 219 | Limiter Switch | Off | 关闭限制器 |

`0.8/1.0/1.2` 不是三个 velocity 档位开关，而是同一个连续 voicing 行为的三个实验采样点。heartbeat 只在 velocity 41 扰动 Piano、velocity 70 扰动 Mezzo、velocity 98 扰动 Forte，用来证明三个控制都可被自动寻址且效应高于噪声；正式 DOE 仍要在多个 velocity 上分别扰动三个控制，测量它们连续交叠的 Jacobian。

heartbeat gate 不是要求 Pianoteq 的随机细节逐字节相等，而是要求：

- 同组重复的最大 normalized RMS residual 不超过 `0.005`；
- hardness 改动的最小 normalized RMS effect 不低于 `0.01`；
- hardness effect 至少为本组重复噪声的 `10` 倍。

`U4 Small` 目前是自动化 heartbeat 的工程基准，不等于已经决定的最终研究 oracle。正式 C4 DOE 开始前仍需冻结最终 preset 和 condition。

`.venv/` 位于工程内但被 Git 忽略；Pianoteq reference 数据位于仓库外的本地数据根目录。两者都可直接在 Finder/Codex 中检查和试听，但不会干扰 Xcode 的 Source Control，也不应上传。

## 运行单个原型实验

```sh
build/hammerlab2 simulate \
  --case-id c4_v1_stulov \
  --model stulov \
  --load resistive-port \
  --vin 1.0 \
  --mass 0.010 \
  --stiffness-n-mm-p 400 \
  --exponent 2.4 \
  --epsilon1 0.51 \
  --tau1 0.0004 \
  --trace runs/c4_v1_stulov/trace.csv \
  --summary runs/c4_v1_stulov/summary.json
```

这些参数只用于让实验可运行。它们不是 Pianoteq 参数，也不是对任何真实钢琴、锤头或羊毛毡的拟合结果。

## 当前音频评估原型

音频评估器使用锁定的 NumPy 环境：

```sh
./scripts/bootstrap_python_env.sh
.venv/bin/python tools/evaluate_audio.py \
  PT8_REFERENCE.wav \
  CANDIDATE.wav \
  --output reports/example.json
```

评估器会进行 onset 对齐，并分别比较早期瞬态窗口。Pianoteq reference 已可由上面的 study runner 自动生成；评估器目前仍不会调用 bbpl 或自动拟合参数。

最终音频同时受到锤、弦、弦间耦合、音板、辐射和输出链影响，因此单个 WAV 的频谱接近不能唯一证明锤模型正确。正式实验将优先比较 Hammer hardness 改变后各指标的方向和灵敏度，再比较绝对误差。

## Pianoteq 8 数据集计划

自动 heartbeat 已经证明采集链可运行。扩展成正式 oracle 前必须进一步锁定并记录：

- Pianoteq 8 的完整补丁版本、edition、instrument pack 和 preset。
- condition、调律、velocity curve、strike point、string length 和 soundboard impedance。
- 麦克风或 output 配置、主输出电平和采样格式。
- reverb、effects、limiter 等可关闭层关闭；Hammer Noise 降至 Pianoteq 8.4.3 允许的最小值 `0.10`，并由重复组量化残余噪声。
- 44.1 kHz、单声道、24-bit PCM、关闭 normalization 和 dither。

现有 15 个 MIDI probe 可继续作为跨音区 baseline smoke，但正式 differential DOE 先从 C4 开始，使用 velocity 32/41/55/70/84/98/112/120。每个 velocity 保存 baseline，并分别对 Piano、Mezzo、Forte hardness 做 `±δ` 与 `±2δ`；每个 case 至少重复一次，用于测量噪声地板。C4 的 partial/Jacobian 管线通过后，再扩展 C2/C3/C5/C6。不同版本、preset 或输出配置的 WAV 不能混入同一数据集。

未来正式数据集的验证入口为：

```sh
python3 tools/validate_pianoteq8_capture.py \
  data/reference/pianoteq8/baseline/manifest.json \
  data/reference/pianoteq8/baseline/audio
```

该命令目前只验证已有数据集的版本、文件数量和音频格式，不负责采集数据。

## 目标 pipeline

HammerLab2.0 最终希望形成下面的闭环：

```text
Pianoteq 8 hardness 扰动 ──> partial 初始能量 ──> J_PT
                                                    │
当前 Bank + 候选模型 ──> 同一 bBpiano DWG ──> J_candidate
                                                    │
                       Jacobian residual + 物理/实时门槛
                                                    │
                      最简单获胜模型 ──> 替换 Bank
```

实施顺序固定为：

1. 收紧实验契约、失败语义、收敛与能量审计。
2. 建立 Pianoteq 8 hardness differential oracle 数据集。
3. 建立 partial 初始能量与 Jacobian fingerprint 分析器。
4. 用真实 `StringPort` 把当前 Bank 接入 HammerLab，锁定主引擎基线。
5. 依次筛选 depth-hardness、单记忆和双记忆候选。
6. 用 Jacobian residual、hold-out、物理约束和复杂度联合选型。
7. 跨音区验证后，再进行实时生产化并替换 Bank。

完整建设内容和每阶段退出条件见 [NORTH_STAR.md](NORTH_STAR.md)。

## 研究与数据边界

Pianoteq 8 输出只作为受控黑盒行为参考。不要提取或修改程序二进制、绕过授权，或分发受许可证限制的渲染素材。reference WAV 应作为本地、不可提交的数据保存；Git 中只跟踪实验配置、代码、指标定义和不含受限音频的报告。

最高优先级目标与工程顺序见 [NORTH_STAR.md](NORTH_STAR.md)。物理模型与实验细节见 [PROJECT.md](PROJECT.md)。论文目录与用途见 [References/INDEX.md](References/INDEX.md)。
