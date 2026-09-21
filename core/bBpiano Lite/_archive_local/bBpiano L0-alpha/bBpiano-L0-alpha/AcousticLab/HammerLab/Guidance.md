# HammerLab 命令说明

HammerLab 用来离线跑锤子/弦模型，输出 CSV 和 SVG，方便检查接触力 `F`、锤毡压缩量 `dy`、锤子速度 `v_in` 和弦输出 `sample`。

## 单条力曲线

```bash
AcousticLab/HammerLab/run_force_case.sh normal 60 4 0.002 25
```

运行原始演奏模式：原有 Gaussian hammer + 完整 `StringModel::Normal` 弦。

```bash
AcousticLab/HammerLab/run_force_case.sh hammerf 60 4 0.002 25
```

运行 Hammer-F 论文实验模式：`HammerModel::HammerF` + 单弦 `StringModel::HammerFTest`。这个模式用于复现 Bank 的理想单弦图，不是 app 里听到的演奏模式。

```bash
AcousticLab/HammerLab/run_force_case.sh perform 60 4 0.002 25
```

运行 Hammer-F 演奏模式：`HammerModel::HammerFPerform` + 完整 `StringModel::Normal` 弦。这个模式对应 app 里切到 Hammer-F 后的新击键。

参数顺序：

```text
mode midi vin duration forceYMax [probe]
```

参数含义：

- `mode`: `normal`、`hammerf` 或 `perform`
- `midi`: MIDI 音高，C4 是 `60`，C5 是 `72`
- `vin`: 直接写入 hammer 的初速度，近似单位 `m/s`
- `duration`: 仿真时长，单位秒；`0.002` 表示 2 ms
- `forceYMax`: 力图 y 轴上限，单位 N
- `probe`: 可选，弦输出观察点，范围 `0..1`，默认 `0.7`

## Normal / Perform 叠图

```bash
AcousticLab/HammerLab/run_normal_perform_overlay.sh 60 4 0.002 0.7 25
```

同时运行 `normal` 和 `perform`，并把两者的 `F` 与 `sample` 画在同一张 SVG 里。

参数顺序：

```text
midi vin duration probe forceYMax
```

输出文件：

```text
AcousticLab/HammerLab/.Generated/normal_force_midi60_vin4_s0.002_p0.7.csv
AcousticLab/HammerLab/.Generated/perform_force_midi60_vin4_s0.002_p0.7.csv
AcousticLab/HammerLab/.Generated/normal_vs_perform_midi60_vin4_s0.002_p0.7.svg
```

## 叠 Figure 5.4

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh
```

把默认 Hammer-F CSV 叠到 Figure 5.4 参考图上。

默认输入：

```text
AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7.csv
```

也可以指定 CSV：

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh AcousticLab/HammerLab/.Generated/perform_force_midi60_vin4_s0.002_p0.7.csv
```

## 单独编译

```bash
bash AcousticLab/HammerLab/build.sh
```

编译输出：

```text
/private/tmp/bBpiano_HammerLab_Build/HammerLab
```

## CSV 常用列

- `time_sec`: 当前帧时间，单位秒
- `F`: 当前锤弦接触力，单位 N
- `dy`: 锤毡压缩量
- `v_in`: 当前 hammer 速度
- `dv`: 当前锤毡压缩速度
- `sigma`: Normal 高斯注入宽度；Hammer-F 单点注入时为 `0`
- `sample`: 指定观察点的弦速度输出
