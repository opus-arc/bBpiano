# HammerLab2.0 工程立项与执行路线

> [!IMPORTANT]
> 本文保留物理模型、数值接口和实验细节。项目唯一目标、候选淘汰规则、Pianoteq differential/Jacobian 行为辨识方法与最新工程顺序以 [NORTH_STAR.md](NORTH_STAR.md) 为准。

## 1. 立项目标

HammerLab2.0 要解决的不是“再写一个 `F = Kx^p`”，也不是建立一个无限扩展的羊毛毡研究平台。它要在统一的 bBpiano DWG、Pianoteq 8 行为指纹、物理约束和实时预算下，筛选出能够替换当前 Bank/Hammer-P 的最简单锤模型。最终验收目标：

- 物理层：在刚性台砧/真实锤实验数据上，同时复现 `F(t)`、`x(t)`、`F-x` 滞回环、冲量、回弹速度和接触时长。
- 耦合层：同一锤核接入 digital waveguide driving-point admittance 后保持稳定、被动、无一采样虚假延迟。
- 行为层：固定弦、音板、辐射与渲染条件时，模型对 Piano/Mezzo/Forte Hammer hardness 的 partial-energy Jacobian 及其 velocity 轨迹与 Pianoteq 8 趋同，并在 hold-out 上优于 Bank。
- 实时层：release 构建中无锁、无分配、固定上界计算，在目标声部数与 oversampling 下留出明确 CPU 余量。

“与 Pianoteq 8 单个频谱一致”不能证明锤模型一致。最终音频是锤、弦、耦合、音板、辐射与后处理的乘积。本项目以 hardness differential/Jacobian fingerprint 为主要黑盒行为目标，用固定下游条件下的局部差分尽量消除弦和音板传递的干扰；公开论文和真实台架数据用于施加物理约束、淘汰错误结构，而不是取代行为辨识主线。

## 2. 当前工程状态

工程位于 `AcousticLab/HammerLab2.0/`，不会修改或依赖旧 `AcousticLab/HammerLab`，也不应被主 Xcode target 自动编进 App。

当前实现：

- `include/hammerlab2/FeltModels.hpp`：固定容量实时锤毡核。
- `include/hammerlab2/Experiment.hpp`、`src/Experiment.cpp`：接触求解器、负载、trace 和指标。
- `apps/hammerlab2_cli.cpp`：单 case CLI。
- `tests/test_models.cpp`：单调性、松弛、被动参数约束、三模型接触与收敛测试。
- `configs/`：力学 smoke matrix、Pianoteq 8 probe matrix、CrossOver 引擎配置和 27-case hardness heartbeat study。
- `tools/`：矩阵执行、MIDI 生成、Pianoteq 无界面 renderer、可恢复 study runner、采集验证、残差 gate、力 trace 与音频评估。
- `requirements-lock.txt`、`scripts/bootstrap_python_env.sh`：固定 Python 分析依赖。
- `References/`：14 篇论文，均已通过 `pdfinfo`、首屏文本抽取与首屏渲染检查。

工程 seed 在 48 kHz、4× oversampling 下已跑通；本机 Pianoteq 8.4.3 的 C4 三层 hardness、三点扰动、三次重复 heartbeat 也已自动跑通。二者只用于验证基础设施与可观测性，不代表完成拟合。

## 3. 模型路线

当前代码中的 power/Stulov/Prony kernel 是验证实验器和记忆支路数值行为的原型，不等同于最终候选编号。正式候选路线如下。

### M0：当前 Bank/Hammer-P 基线

\[
F(t)=K[x(t)]_+^p
\]

M0 必须尽可能复现主引擎当前行为，包括既有弦反馈方式。它是复杂度、CPU、激励和听感的共同基线；没有在 hold-out 上稳定优于 M0 的候选不得进入生产。

### M1：连续 depth-hardness backbone

第一代核心结构变量是连续的 `hardness(depth)`，而不是按 velocity 切换三套 `K`。力度只决定锤毡实际压缩到多深，Piano/Mezzo/Forte 控制对应浅、中、深区域的平滑重叠硬度基函数。

推荐以正切线刚度构造连续力曲线：

\[
k_t(x)=k_{\mathrm{base}}(x)
\exp\left(h_PB_P(x)+h_MB_M(x)+h_FB_F(x)\right),
\quad
F_{\mathrm{eq}}(x)=\int_0^x k_t(s)\,ds.
\]

M1 用于单独判断 depth profile 是否已经足以解释 Pianoteq hardness Jacobian；在 M1 失败前，不应先增加记忆状态。

### M2：M1 + 单松弛支路

将 M1 的非线性压缩坐标 `u=Φ(x,h_P,h_M,h_F)` 送入一个精确离散的一阶记忆支路：

\[
z_{n+1}=e^{-\Delta t/\tau}z_n+\epsilon(1-e^{-\Delta t/\tau})u_{n+1},
\quad F=K[u-z]_+.
\]

M2 是最小 hereditary 扩展，用于测试加载/卸载、速率相关和短期记忆能否解释 M1 剩余的稳定 residual。参数必须满足 `epsilon >= 0` 且总和小于 1。

### M3：M1 + 双松弛支路

只有当 M2 无法同时覆盖可重复的快、慢残差时间尺度时，才加入第二个 Prony/QLV 支路。M3 必须证明其 Jacobian/hold-out 收益超过重复性噪声和额外 CPU 成本，否则回退 M2。

### 延后研究项

真实锤头质量、毡厚、曲率和 voicing 随键位变化，但不要为 88 键存 88 套不相关参数。候选结构通过后，再为 `hardness_profile(note)`、`epsilon(note)`、`tau(note)` 建立低阶连续参数场。

柔性锤柄、分布式接触、三支路以上记忆和复杂接触几何全部延后。只有 M1–M3 都留下可定位且无法解释的 residual 时，才允许把它们提升为下一代候选。

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

必须记录 Pianoteq **8 的具体补丁版本**与 edition。固定 instrument/preset、condition、调律、velocity curve、strike point、string length、soundboard impedance、麦克风/输出、采样率和所有后处理。关闭 reverb、effects、limiter 等可关闭层；Pianoteq 8.4.3 的 Hammer Noise 最低只能设为 `0.10`，因此将其固定在最小值并用重复组测量残余噪声地板。保留 250 ms 前静音用于可靠 onset detection。

### 6.2 一次只改变一个物理控制

Pianoteq 的 voicing 面板把 Hammer hardness 分成低、中、高力度区域。第一轮 DOE 不急于铺开音区，而是先用 C4 建立足够密的 velocity differential：

- velocity 32/41/55/70/84/98/112/120；
- 每个 velocity 保存 baseline；
- Piano、Mezzo、Forte hardness 分别做 `±δ` 与 `±2δ`，一次只改变一个控制；
- 每个 case 至少重复一次，用重复差异建立数值噪声地板；
- C4 管线通过后再扩展 C2/C3/C5/C6 和 hold-out 音高。

- Hammer tone 与 Hammer noise 不参与锤毡辨识；它们会把输出修饰与接触力学混在一起。

### 6.3 比较对象

音频分析以 partial 初始能量和 hardness Jacobian 为主：

- 拟合接触后各 partial 的复振幅与衰减，并反推到接触结束附近；
- 同时保存绝对 partial energy 与归一化 partial distribution；
- 对 Piano、Mezzo、Forte 控制计算中央差分；
- 比较 `J[note, velocity, partial, hardness-control]`，而不只比较单个 FFT；
- 0–5、5–20、20–100 ms 窗口保留为 broadband transient 和残差诊断，不代替 modal estimator。

不能从 Pianoteq 8 WAV 唯一恢复内部 `F(t)`。可行的 black-box system identification 是把自有许可证渲染的音频当不可分发的测试 oracle，学习其可观测 differential behavior，而不是提取代码、模型参数或声库。论文和真实台架数据用于限制非物理解，不再被当作整条工程管线的第一终点。

## 7. 参数辨识原则

1. **Bank 始终参与比较**：所有 loss 和报告都必须给出候选相对 M0 Bank 的改善，而不是只给候选自己的绝对分数。
2. **Jacobian 是黑盒主目标**：联合比较 baseline partial、hardness Jacobian 及其 velocity 轨迹；单个 WAV 或单个频谱不能决定模型。
3. **Synthetic recovery 先行**：用已知参数和合成 modal signal 验证 partial estimator、有限差分和 optimizer。
4. **有限差分优先**：Pianoteq oracle 与第一版 candidate 都先使用 `±δ/±2δ`；只有计算成本成为瓶颈时再引入 autodiff。
5. **物理约束不可由拟合覆盖**：单边接触、稳定、被动、收敛和能量方向是硬门槛，不允许用更低频谱 loss 抵消。
6. **跨 velocity/note hold-out**：不能为每个 velocity 独立拟合。若参数必须随 velocity 漂移，优先怀疑缺少 depth-dependent 结构。
7. **残差驱动复杂度**：只有稳定、可重复、可定位的 residual 才能批准新状态或新模型结构。
8. **论文/台架用于约束**：用 `F(t)`、`x(t)`、`F-x`、冲量和回弹限制参数与结构，不把论文复现当作替换 Bank 的充分条件。

优化器可先采用 bounds-aware 多起点方法，之后再根据 candidate 的可微程度选择 Gauss–Newton/Levenberg–Marquardt、自动微分或其他局部精化方法。不要一开始用神经网络替代接触模型；它会掩盖不可辨识性、被动性和实时成本。

## 8. 最新工程工作包

完整建设内容和每阶段退出条件见 [NORTH_STAR.md](NORTH_STAR.md)。工程依赖顺序固定为：

1. **E0 实验骨架收尾**：完整配置契约、严格失败语义、收敛/能量测试和 CI。
2. **E1 Pianoteq oracle**：锁定环境，采集 C4 密集 velocity 的 hardness `±δ/±2δ` 与重复组。
3. **E2 行为分析器**：partial 初始能量估计、`J_PT`、置信区间和 synthetic recovery。
4. **E3 真实 DWG 基线**：设计 `StringPort`，先让当前 Bank 在 HammerLab 与 bbpl 输出对齐。
5. **E4 候选筛选**：依次测试 depth-hardness、单记忆、双记忆，不允许无证据堆复杂度。
6. **E5 参数搜索**：联合最小化 partial/Jacobian/velocity/energy residual，并按维度诊断。
7. **E6 跨音区验证**：扩展 C2–C6，建立低阶 note 参数场和固定 hold-out。
8. **E7 生产替换**：通过最大复音、fallback、线程和实时压力测试后，先进入 bbpl，再替换 App 默认 Bank。

在 E2 和 E3 完成之前，不应宣布某个新 felt 公式是 production candidate；在 E6 和 E7 完成之前，不应替换主引擎默认 Bank。
