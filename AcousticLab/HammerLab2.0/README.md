# HammerLab2.0

HammerLab2.0 是一个与旧 `AcousticLab/HammerLab` 完全隔离的钢琴锤毡建模实验工程。目标不是复刻某个最终音色，而是建立一套可以反复采集、拟合、回归和实时基准测试的 hammer-contact infrastructure，最终把通过验证的无分配 C++ 内核接入 digital waveguide 弦端口。

本工程的黑盒参考对象固定为 **Pianoteq 8**。采集验证器会拒绝版本号不是 `8.x.x` 的数据，避免混入其他主版本。

## 已有能力

- 三个可替换的锤毡模型：无记忆幂律、单支路 Stulov 遗传模型、双支路 Prony 松弛模型。
- 刚性台砧与电阻性 waveguide driving-point port 两种负载。
- 无堆分配的逐样本 felt kernel；实验 trace 与文件 I/O 位于非实时层。
- 隐式接触迭代、力/压缩/记忆变量 trace、接触时长、冲量、回弹、滞回功和实时因子。
- CSV 参数矩阵 runner、可复现 manifest、Pianoteq 8 MIDI 探针生成器、采集验证器。
- 力曲线评估和 onset 对齐后的 0–5、5–20、20–100 ms 多窗口音频频谱评估。
- 14 篇本地参考论文及校验索引。

## 五分钟跑通

```sh
cd /Users/muzhi/Documents/bBpiano/AcousticLab/HammerLab2.0
make test
make smoke
python3 tools/run_matrix.py configs/smoke_matrix.csv
python3 tools/generate_probe_midis.py \
  configs/pianoteq8_probe_matrix.csv \
  data/reference/pianoteq8/midi
```

音频评估依赖 NumPy：

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
.venv/bin/python tools/evaluate_audio.py PT8_REFERENCE.wav CANDIDATE.wav
```

单个物理实验可以直接调用：

```sh
build/hammerlab2 simulate \
  --case-id c4_v1_stulov \
  --model stulov --load resistive-port \
  --vin 1.0 --mass 0.010 \
  --stiffness-n-mm-p 400 --exponent 2.4 \
  --epsilon1 0.51 --tau1 0.0004 \
  --trace runs/c4_v1_stulov/trace.csv \
  --summary runs/c4_v1_stulov/summary.json
```

这些默认数值只是用于保证工程可运行的 seed，不是 Pianoteq 参数，也不是对任何具体钢琴的拟合结果。

## Pianoteq 8 参考采集

1. 必须使用你合法持有的 Pianoteq 8；记录完整版本、edition、instrument pack、preset 和输出配置。
2. 复制 `configs/pianoteq8_capture_manifest.template.json` 到数据集目录并填完所有字段。
3. 生成 15 个带 250 ms 前静音的固定单音 MIDI，逐个用同一 preset 离线渲染为无归一化 PCM WAV。
4. 关闭 reverb、effects、limiter、hammer noise；踏板抬起。保持调律、弦长、strike point、soundboard impedance、麦克风/输出不变。
5. 先采集 baseline；然后一次只改变一个 Hammer hardness 控制，另建 manifest。不要把不同 preset 或版本的 WAV 混在同一目录。
6. 运行：

```sh
python3 tools/validate_pianoteq8_capture.py \
  data/reference/pianoteq8/baseline/manifest.json \
  data/reference/pianoteq8/baseline/audio
```

Pianoteq 输出只能作为受控黑盒行为参考。不要提取程序二进制、绕过授权或分发其渲染素材；数据集的使用应遵守你的许可证。详细实验设计与判据见 [PROJECT.md](PROJECT.md)，论文用途见 [References/INDEX.md](References/INDEX.md)。
