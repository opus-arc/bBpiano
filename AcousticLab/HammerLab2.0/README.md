# HammerLab2.0

HammerLab2.0 是一个与旧 `AcousticLab/HammerLab` 完全隔离的钢琴锤毡建模研究工程。它的长期目标是建立一套可重复采集、参数辨识、黑盒对照、回归测试和实时性能验证的 hammer-contact infrastructure，最终把通过验证的无分配 C++ 锤毡内核接入 bBpiano 的 digital waveguide 弦端口。

> [!IMPORTANT]
> HammerLab2.0 当前仍处于初始化阶段，只是后续研究的起点。现有代码可以编译、运行数值实验和验证工具，但还不是已经标定的钢琴锤模型，不能直接用于改造 bBpiano，也不能据此声称已经复现真实羊毛毡质感或达到 Pianoteq 级别。

本工程未来的黑盒参考对象固定为 **Pianoteq 8**。采集验证器会拒绝版本号不是 `8.x.x` 的数据，避免混入其他主版本。但目前尚未建立正式 Pianoteq 8 baseline/hardness 数据集，也尚未完成端到端自动采集与拟合。

## 当前阶段

目前已经建立的是实验骨架，而不是可投入使用的完整 pipeline：

- 三个可替换的锤毡原型：无记忆幂律、单支路 Stulov 遗传模型、双支路 Prony 松弛模型。
- 无堆分配的逐样本 felt kernel；trace、报告与文件 I/O 位于非实时实验层。
- 隐式接触迭代，以及力、压缩量和内部记忆变量的 trace。
- 峰值力、接触时长、冲量、回弹速度、滞回功、求解残差与 realtime factor 等基础指标。
- 刚性台砧和电阻性 waveguide driving-point load 两种简化测试负载。
- 单元测试、smoke case、CSV 参数矩阵 runner 与实验 manifest/schema。
- 15 组确定性 Pianoteq 8 MIDI 单音探针、采集 manifest 模板和数据集验证器。
- 力曲线评估器，以及 onset 对齐后的 0–5、5–20、20–100 ms 分窗音频频谱评估器。
- 14 篇本地参考论文及页数、用途、来源和 SHA-256 校验索引。

这里的刚性台砧和电阻性端口只用于检查原型的数值行为，不是 bBpiano 真实弦模型的 driving-point impedance。当前测试通过仅表示代码能在 seed 参数下稳定执行，不代表物理准确性或听感已经通过验证。

## 尚未完成

HammerLab2.0 目前还不能真正开始用于锤模型生产，关键缺口包括：

- 尚未从论文图表或真实撞击台架获得可用于标定的 `F(t)`、`x(t)`、`F-x` 数据。
- `mass`、`K`、`p`、`epsilon`、`tau` 和端口阻抗等参数仍是启动实验用的 seed。
- 尚未实现连接 bBpiano DWG 弦模型的真实、无所有权、无分配 `StringPort`。
- HammerLab2.0 felt kernel 尚未接入 bbpl 或 bBpiano App 的 `HammerModel`。
- 尚未锁定正式 Pianoteq 8 preset、voicing、输出链和重复性条件。
- 尚未完成 Pianoteq 8 baseline 与 Hammer hardness sweep 的正式批量采集。
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

音频评估器依赖 NumPy：

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
.venv/bin/python tools/evaluate_audio.py \
  PT8_REFERENCE.wav \
  CANDIDATE.wav \
  --output reports/example.json
```

评估器会进行 onset 对齐，并分别比较早期瞬态窗口。这个工具只能比较两个已经准备好的 WAV；它目前不会自动操作 Pianoteq 8、不会调用 bbpl，也不会自动拟合参数。

最终音频同时受到锤、弦、弦间耦合、音板、辐射和输出链影响，因此单个 WAV 的频谱接近不能唯一证明锤模型正确。正式实验将优先比较 Hammer hardness 改变后各指标的方向和灵敏度，再比较绝对误差。

## Pianoteq 8 数据集计划

正式采集尚未开始。开始采集前必须先锁定并记录：

- Pianoteq 8 的完整补丁版本、edition、instrument pack 和 preset。
- condition、调律、velocity curve、strike point、string length 和 soundboard impedance。
- 麦克风或 output 配置、主输出电平和采样格式。
- reverb、effects、limiter 和 hammer noise 关闭，踏板抬起。
- 44.1 kHz、单声道、24-bit PCM、关闭 normalization 和 dither。

计划中的第一组 DOE 为 C2/C3/C4/C5/C6 × velocity 32/80/120，共 15 个 baseline case。之后一次只改变对应力度区间的一个 Hammer hardness 控制，并为每个 sweep 单独保存 manifest。不同版本、preset 或输出配置的 WAV 不能混入同一数据集。

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
固定 MIDI 探针
    ├── Pianoteq 8 离线渲染 ──> reference WAV
    └── bbpl + candidate felt ─> candidate WAV
                                  │
                         onset 对齐与分窗评分
                                  │
                    受约束参数搜索与 hold-out 验证
                                  │
                     参数表生成与 C++ 实时回归测试
```

建议实施顺序：

1. 数字化并复现公开论文中的力学曲线，验证参数辨识器。
2. 建立真实锤头撞击台架，估计测量噪声与跨速度重复性。
3. 实现连接 bBpiano 弦模型的真实 `StringPort`，完成能量和采样率收敛测试。
4. 锁定 Pianoteq 8 条件，采集 baseline 与 hardness sensitivity 数据集。
5. 接通 Pianoteq 8 reference、bbpl candidate、批量评分和参数优化。
6. 进行跨音高/力度验证、最大复音压力测试，再考虑生产集成。

## 研究与数据边界

Pianoteq 8 输出只作为受控黑盒行为参考。不要提取或修改程序二进制、绕过授权，或分发受许可证限制的渲染素材。reference WAV 应作为本地、不可提交的数据保存；Git 中只跟踪实验配置、代码、指标定义和不含受限音频的报告。

详细实验设计、验收门槛与工作包见 [PROJECT.md](PROJECT.md)。论文目录与用途见 [References/INDEX.md](References/INDEX.md)。
