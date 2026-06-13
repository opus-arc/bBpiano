# HammerLab 命令说明

HammerLab 当前接入的是 `bBpiano Lite` 的 C++ core，用来离线跑 Lite runtime 里的 Hammer-P 击弦路径，并输出接触力 `F` 随时间变化的 CSV 和 SVG。

## 单条力曲线

```bash
AcousticLab/HammerLab/run_force_case.sh 60 4 0.002 25
```

参数顺序：

```text
midi vin duration forceYMax [probe]
```

参数含义：

- `midi`: MIDI 音高，C4 是 `60`，C5 是 `72`
- `vin`: 直接写入 hammer 的初速度，近似单位 `m/s`
- `duration`: 仿真时长，单位秒；`0.002` 表示 2 ms
- `forceYMax`: 力图 y 轴上限，单位 N
- `probe`: 可选，弦输出观察点，范围 `0..1`，默认 `0.7`

输出文件示例：

```text
AcousticLab/HammerLab/.Generated/lite_hammerp_force_midi60_vin4_s0.002_p0.7.csv
AcousticLab/HammerLab/.Generated/lite_hammerp_force_midi60_vin4_s0.002_p0.7.svg
```

## 叠 Figure 5.4

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh
```

默认输入：

```text
AcousticLab/HammerLab/.Generated/lite_hammerp_force_midi60_vin4_s0.002_p0.7.csv
```

也可以指定 CSV：

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh AcousticLab/HammerLab/.Generated/lite_hammerp_force_midi60_vin4_s0.002_p0.7.csv
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
- `sample`: 指定观察点的弦速度输出
