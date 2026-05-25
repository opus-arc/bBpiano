# HammerLab 使用说明

HammerLab 用来离线运行锤子/弦模型，并输出接触力随时间变化的 CSV 和 SVG。

## 运行力曲线

```bash
AcousticLab/HammerLab/run_force_case.sh normal 60 4 0.002 25
```

运行 Normal 模式。Normal 使用原来的 `HammerModel` 路径和原来的 `StringModel::Normal` 弦模型。

```bash
AcousticLab/HammerLab/run_force_case.sh hammerf 60 4 0.002 25
```

运行 Hammer-F 模式。Hammer-F 使用 `HammerModel::HammerF` 路径，并把测试弦切到 `StringModel::HammerFTest`。

命令参数顺序：

```text
mode midi vin duration yMax [probe]
```

参数含义：

- `mode`: `normal` 或 `hammerf`
- `midi`: MIDI 音高，例如 C4 是 `60`，C5 是 `72`
- `vin`: 直接写入锤子的初始速度 `v_in`，单位近似为 `m/s`
- `duration`: 仿真时长，单位是秒，例如 `0.002` 表示 2 ms
- `yMax`: 力图 y 轴上限，单位是 N
- `probe`: 可选，弦上观察点，范围 `0..1`，默认是 `0.7`

输出文件示例：

```text
AcousticLab/HammerLab/.Generated/normal_force_midi60_vin4_s0.002_p0.7.csv
AcousticLab/HammerLab/.Generated/normal_force_midi60_vin4_s0.002_p0.7.svg
AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7.csv
AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7.svg
```

CSV 里常用列：

- `time_sec`: 当前帧时间，单位是秒
- `F`: 当前记录的锤弦接触力，单位是 N
- `dy`: 锤毡压缩量
- `v_in`: 当前锤子速度
- `dv`: 当前锤毡压缩速度
- `sigma`: Normal 高斯注入的空间尺度；Hammer-F 单点注入时为 `0`
- `sample`: 指定观察点的弦速度输出

## 叠 Figure 5.4

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh
```

使用默认 Hammer-F CSV 叠到 Figure 5.4 参考图上。

默认输入：

```text
AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7.csv
```

默认输出：

```text
AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7_over_fig54.svg
```

也可以指定任意 CSV：

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh AcousticLab/HammerLab/.Generated/normal_force_midi60_vin4_s0.002_p0.7.csv
```

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7.csv
```

## 单独编译

```bash
bash AcousticLab/HammerLab/build.sh
```

只编译 HammerLab 临时二进制，不运行仿真。

编译输出位置：

```text
/private/tmp/bBpiano_HammerLab_Build/HammerLab
```
