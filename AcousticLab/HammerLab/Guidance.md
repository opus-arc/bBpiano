# HammerLab 使用说明

HammerLab 是离线测试锤子/弦模型的小实验台。它会编译临时二进制，跑指定 MIDI、初速度和时长，输出 CSV 和 SVG。

当前阶段：

- `normal` = 现有 `HammerModel` + 现有 `StringModel::Normal`
- `hammerf` = 现有 `HammerModel` + `StringModel::HammerFTest`

也就是说，`hammerf` 现在只用来验证弦的测试模式：无全通、无损耗、无色散。真正的 Hammer-F 锤子运动路径还没写进 `HammerModel`。

## 主入口

只用这个脚本：

```bash
AcousticLab/HammerLab/run_force_case.sh normal 60 4 0.002 25
AcousticLab/HammerLab/run_force_case.sh hammerf 60 4 0.002 25
```

参数顺序：

```text
mode midi vin duration yMax [probe]
```

- `mode`: `normal` 或 `hammerf`
- `midi`: MIDI 音高，C4 是 `60`，C5 是 `72`
- `vin`: 直接写入锤子的初始速度 `v_in`
- `duration`: 仿真时长，单位秒
- `yMax`: 力图 y 轴上限，单位 N
- `probe`: 可选，弦上观察点，默认 `0.7`

输出例子：

```text
AcousticLab/HammerLab/.Generated/normal_force_midi60_vin4_s0.002_p0.7.csv
AcousticLab/HammerLab/.Generated/normal_force_midi60_vin4_s0.002_p0.7.svg
AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7.csv
AcousticLab/HammerLab/.Generated/hammerf_force_midi60_vin4_s0.002_p0.7.svg
```

## 叠 Figure 5.4

默认叠 Hammer-F 文件名：

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh
```

指定某个 CSV：

```bash
AcousticLab/HammerLab/run_fig54_overlay.sh AcousticLab/HammerLab/.Generated/normal_force_midi60_vin4_s0.002_p0.7.csv
```
