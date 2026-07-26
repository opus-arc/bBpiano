# HammerLab2.0 工程立项与执行路线

## 1. 立项目标

HammerLab2.0 要解决的不是“再写一个 `F = Kx^p`”，而是把羊毛毡的速率相关、记忆、卸载滞回、接触时长和弦反作用变成可辨识、可替换、可实时运行的工程模块。最终验收目标：

- 物理层：在刚性台砧/真实锤实验数据上，同时复现 `F(t)`、`x(t)`、`F-x` 滞回环、冲量、回弹速度和接触时长。
- 耦合层：同一锤核接入 digital waveguide driving-point admittance 后保持稳定、被动、无一采样虚假延迟。
- 感知层：固定弦、音板、辐射与渲染条件时，模型参数变化对早期瞬态的影响方向和强度与 Pianoteq 8 的 Hammer hardness 扫描一致。
- 实时层：release 构建中无锁、无分配、固定上界计算，在目标声部数与 oversampling 下留出明确 CPU 余量。

“与 Pianoteq 8 频谱一致”只作为系统级约束，不能单独证明锤模型一致。最终音频是锤、弦、耦合、音板、辐射与后处理的乘积；未知系统中多个参数可能给出相同频谱。因此本项目采用两阶段 identification：先用可观测力学量确定锤毡，再用 Pianoteq 8 的受控灵敏度曲线做黑盒校验。

## 2. 当前初始化状态

工程位于仓库顶层 `HammerLab2.0/`，不会修改或依赖旧 `AcousticLab/HammerLab`，也不会被主 Xcode 同步目录自动编进 App target。

当前实现：

- `include/hammerlab2/FeltModels.hpp`：固定容量实时锤毡核。
- `include/hammerlab2/Experiment.hpp`、`src/Experiment.cpp`：接触求解器、负载、trace 和指标。
- `apps/hammerlab2_cli.cpp`：单 case CLI。
- `tests/test_models.cpp`：单调性、松弛、被动参数约束、三模型接触与收敛测试。
- `configs/`：力学 smoke matrix 和 Pianoteq 8 probe matrix。
- `tools/`：矩阵执行、MIDI 生成、采集验证、力 trace 与音频评估。
- `References/`：14 篇论文，均已通过 `pdfinfo`、首屏文本抽取与首屏渲染检查。

工程 seed 在 48 kHz、4× oversampling 下已跑通；它只用于验证基础设施，不代表完成拟合。

## 3. 模型路线

### M0：无记忆幂律基线

\[
F(t)=K[x(t)]_+^p
\]

它便宜且可作为回归基线，但没有加载/卸载差异，也不能表现 wool felt 的历史依赖。它不应成为最终模型。

### M1：Stulov 单松弛支路

当前实现把非线性压缩 `u=x^p` 送入一个精确离散的一阶记忆支路：

\[
z_{n+1}=e^{-\Delta t/\tau}z_n+\epsilon(1-e^{-\Delta t/\tau})u_{n+1},
\quad F=K[u-z]_+.
\]

这是工程上可控的最小 hereditary 模型：有速率依赖、滞回和松弛时间常数，且每支路只需一个状态。参数必须满足 `epsilon >= 0` 且总和小于 1，以保留正的长期刚度。

### M2：双支路 Prony / QLV

单一 `tau` 很难同时描述极短碰撞前沿与较慢卸载尾部。双支路使用“fast + slow”松弛谱，在仍然固定成本的条件下拟合更真实的羊毛毡质感。这是第一版 production candidate；只有当跨速度、跨音区数据证明需要时才扩展到三支路。

### M3：速度/音区连续参数场

真实锤头质量、毡密度、曲率和 voicing 随键位连续变化。不要为 88 键存 88 组互不相关参数；拟合 `K(note)`、`p(note)`、`epsilon(note)`、`tau(note)` 的低阶样条，并检查随 impact velocity 的系统偏差。若参数明显随速度漂移，应增加结构变量，而不是继续硬塞 lookup table。

### M4：柔性 hammer shank 与接触几何

Chabassier–Duruflé 的柔性锤柄模型提示高能量敲击下，锤头并非理想集中质量。该层应在 M2 通过后再加入一个低阶锤柄模态/等效顺应性。弦径与锤冠曲率影响实际接触面积；需要用不同直径 string dummy 或分布式接触验证，不能只靠换 `K` 掩盖。

## 4. Digital waveguide 接口

实验层现有 `resistive-port` 是最小 driving-point impedance，用于验证接触反馈方向与隐式求解。接入真实 DWG 时应把它替换为如下端口契约：

```text
hammer predicts force F[n]
        ↓
string port returns contact-point velocity v_s[n] from incident waves + F[n]
        ↓
compression velocity = v_h[n] - v_s[n]
        ↓
implicit/iterated solve commits hammer, felt-memory and outgoing waves once
```

要求：

- 接触力只能推开物体，不能产生拉力。
- felt state 与 waveguide state 在迭代中使用 trial copy，收敛后只 commit 一次。
- 不在 force path 中插入任意一采样延迟；若为实时预算改用显式/预测校正算法，必须用 oversampling convergence test 量化误差。
- 三弦 unison 的锤力是共享接触几何下的耦合问题，不能简单复制三份独立锤力。先用等效并联 driving-point admittance，再评估分布式三接触点。
- 音频线程不写 CSV、不建目录、不分配 vector；当前 trace 仅属于 HammerLab 离线 harness。

## 5. 可重复实验与指标

每个力学 case 必须保存：输入矩阵 SHA-256、二进制 SHA-256、Git commit/dirty 状态、完整参数、trace 与 summary。硬门槛：

| 层级 | 主指标 | 首轮建议门槛 |
| --- | --- | --- |
| 数值 | finite、solver residual、采样率收敛 | 全部 finite/converged；2× 内部采样率后峰值与接触时长变化 < 1% |
| 力学 | peak force、contact time、impulse、rebound | 归一化 force RMSE < 5%；各标量误差 < 5% |
| 滞回 | `F-x` 环面积与加载/卸载曲线 | 符号正确；能量误差 < 10% |
| 实时 | realtime factor、worst-case iterations | 单声部锤核 < 音频预算的 10%；整机按最大复音压力测试 |
| 黑盒 | PT8 参数灵敏度、多窗口谱差 | 先匹配变化方向，再优化幅度；不能只报整段 FFT |

建议门槛是启动值，获得真实台架重复性后应以测量噪声地板重新制定。

## 6. Pianoteq 8 黑盒评估设计

### 6.1 Baseline 锁定

必须记录 Pianoteq **8 的具体补丁版本**与 edition。固定 instrument/preset、condition、调律、velocity curve、strike point、string length、soundboard impedance、麦克风/输出、采样率和所有后处理。关闭 reverb、effects、limiter、hammer noise 与踏板；保留 250 ms 前静音用于可靠 onset detection。

### 6.2 一次只改变一个物理控制

Pianoteq 的 voicing 面板把 Hammer hardness 分成低、中、高力度区域。第一轮 DOE：

- 15 个 baseline：C2/C3/C4/C5/C6 × velocity 32/80/120。
- 只扫对应力度区的 Hammer hardness，例如 baseline 两侧各 3 个等间距位置。
- Hammer tone 与 Hammer noise 不参与锤毡辨识；它们会把输出修饰与接触力学混在一起。
- 每个 case 至少重复渲染一次并做逐样本一致性检查，确认引擎设置没有随机化或 humanization。

### 6.3 比较对象

音频先 onset 对齐但不峰值归一化地比较 peak level；再峰值归一化比较形状：

- 0–5 ms：冲击前沿与高频注入。
- 5–20 ms：接触结束、弦早期传播与多弦 beating 起点。
- 20–100 ms：确认差异没有被弦/音板参数错误放大。
- 对每个硬度 step 计算 `d(metric)/d(hardness)`；HammerLab 模型必须先匹配导数符号和随 velocity/note 的趋势。

不能从 Pianoteq 8 WAV 唯一恢复内部 `F(t)`。可行的“偷走声音”应解释为合法的 black-box system identification：把自有许可证渲染的音频当不可分发的测试 oracle，学习其可观测行为，而不是提取代码或声库。真正的 hammer 参数仍应由公开论文或自建刚性台砧/力传感器实验确定。

## 7. 参数拟合 pipeline

1. **Synthetic recovery**：用已知参数生成 trace，再从扰动初值反推，验证 optimizer 与参数尺度。
2. **Stulov 数据复现**：数字化论文的 `F(t)`、`x(t)` 或 `F-x` 图；保留 digitization provenance，不把图片读数冒充原始数据。
3. **自建台架**：锤头撞击 load cell + 位移/速度测量；每个速度至少 5 次，分层估计 measurement noise。
4. **分阶段优化**：先 `mass/K/p`，再 `epsilon/tau`；损失函数联合 force waveform、contact time、impulse、rebound 和 hysteresis area。
5. **跨速度 hold-out**：不能每个 velocity 独立拟合。用部分速度训练，剩余速度验证结构是否正确。
6. **DWG-in-loop**：固定已校准字符串端口，重测稳定性和早期谱。
7. **Pianoteq 8 sensitivity gate**：只比较受控 hardness 扫描的响应趋势；不把 PT8 当参数真值。
8. **实时移植**：冻结模型结构，生成音区样条参数；在音频 callback 上做 worst-case profiler、denormal/NaN 和线程安全测试。

后续 optimizer 推荐先实现 bounds-aware derivative-free 多启动（例如 CMA-ES/Nelder–Mead），得到稳定基线后再用论文 13 的 differentiable/physics-informed 思路做梯度校准。不要一开始就用神经网络替代锤毡；它会掩盖不可辨识性和被动性问题。

## 8. 下一阶段明确工作包

### WP1：把论文曲线变成可验证数据

- 为 Stulov 2000/2005 的每条曲线建立 `data/measured/<source>/<figure>/`。
- 保存原图页码、轴单位、digitizer CSV、重复性/读取误差和 `metadata.json`。
- 新增 `evaluate_force.py` 的批量 leaderboard 与参数拟合器。

### WP2：真实 DWG driving-point adapter

- 定义无所有权、无分配的 `StringPort` concept。
- 接入当前工程的弦模型但保持 compile-time 隔离。
- 增加能量审计、三弦并联端口、不同 strike point 与采样率收敛测试。

### WP3：Pianoteq 8 首个正式数据集

- 当前机器只应在安装并验证为 8.x.x 后采集；验证器会拦截其他版本。
- 完成 baseline 和 Hammer hardness sweep manifest。
- 输出 per-case JSON、跨音区 sensitivity heatmap 和回归阈值。

### WP4：实时生产化

- 把 M2 核改造成 header-only policy 或现有 engine component，参数在 note-on 时准备。
- 对 48/96 kHz、1×/2×/4×、最大复音执行 benchmark。
- 为非收敛设置可审计 fallback，而不是静默输出爆音或 NaN。

完成顺序必须是 WP1 → WP2 → WP3 → WP4。否则很容易用未知的弦/音板误差去“拟合锤子”，得到听起来接近但不能泛化的模型。
