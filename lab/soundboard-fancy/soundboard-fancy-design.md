# Soundboard Fancy：本地实验环境与 v2a Target 设计说明

## 1. 在 soundboard lab 中建立独立的本地 Python 环境

Soundboard 的离线实验代码应当与系统 Python 分离，避免 Zed、终端和依赖版本不一致。当前实验目录为 `bBpiano/lab/soundboard-fancy/`，建议直接在该目录内建立 `.venv`，使这一套环境只服务于 Soundboard 的分析、拟合与验证脚本。

进入实验目录：

```bash
cd "$(git rev-parse --show-toplevel)/lab/soundboard-fancy"
```

使用 `uv` 创建 Python 3.12 虚拟环境：

```bash
uv venv --python 3.12 .venv
```

安装当前实验需要的科学计算工具：

```bash
uv pip install --python .venv/bin/python numpy matplotlib scipy ruff
```

验证环境是否正常：

```bash
.venv/bin/python -c "import numpy, matplotlib, scipy; print('OK')"
```

如果输出 `OK`，说明环境已经可以使用。终端中运行实验脚本时，可以直接显式调用：

```bash
.venv/bin/python target_damping.py
```

也可以先激活环境：

```bash
source .venv/bin/activate
python target_damping.py
```

在 Zed 中，应将 Python Toolchain 指向：

```text
bBpiano/lab/soundboard-fancy/.venv
```

这样 Zed 的 Python language server、Ruff、代码运行与终端实验可以尽可能共享同一套依赖环境，避免“终端能 import，但编辑器报红”或反过来的情况。`.venv` 是本地运行环境，不应视为实验结果本身；是否提交到 Git 应由 `.gitignore` 控制，通常不提交其内部文件。

## 2. v2a Target 的目的

当前 Soundboard FDN 的旧损耗模型近似使用统一的 `T60 ≈ 0.34 s`，因此绝大多数隐式模态具有接近相同的衰减时间。这会导致模态损耗因子随频率近似满足 `η(f) ∝ 1/f`，与钢琴音板文献中常见的阻尼统计并不一致。v2a 的第一步不是直接设计 SOS 系数，而是先定义一个连续时间的目标阻尼规律 `α_target(f)`，再把它转换成每条 FDN delay branch 在一次 traversal 中应具有的目标衰减。这样可以把“我们想要什么物理阻尼”与“最后用什么数字滤波器实现”分开。

## 3. 基本模态阻尼关系

轻阻尼模态可以写成：

```math
x(t)=A_0 e^{-\alpha t}\cos(2\pi f t+\phi).
```

其中 `f` 是模态频率，`A0` 与 `φ` 由初始激励决定，`α` 是连续时间振幅衰减率，单位为 `s^-1`。其包络满足：

```math
A(t)=A_0 e^{-\alpha t}.
```

对于轻阻尼系统，modal loss factor `η` 与 `α` 近似满足：

```math
\eta \approx \frac{\alpha}{\pi f},
\qquad
\alpha \approx \pi f\eta.
```

而振幅下降 60 dB，即降到初始值的 `10^-3` 时：

```math
T_{60}=\frac{\ln 1000}{\alpha}.
```

因此 `η(f)`、`α(f)` 和 `T60(f)` 只是同一个阻尼规律的不同表达，其中 v2a 选择 `α(f)` 作为核心 target。

## 4. 当前 v2a Target 的来源

当前 target 不是直接复制某一篇论文的完整曲线，而是一条“文献约束下的工程目标”。主体参考 Ege、Boutillon 等对钢琴音板 modal damping 的实验与 reduced-model 结果，其中可可靠识别的模态损耗因子常落在约 `1%–3%`，统计中心约为 `2.3%`。因此 v2a 第一版采用：

```math
\eta_0=0.023
```

作为确定性的平均阻尼主干，并暂时不加入 mode-to-mode 的随机散布。由轻阻尼关系得到：

```math
\alpha_{wood}(f)=\pi\eta_0 f.
```

这构成中频主体。

低频部分不直接把 `η=2.3%` 外推到零频，因为这样会导致 `α→0`、`T60→∞`，而低阶真实模态还会受到 rim、边界等附加损耗影响。第一版因此保留旧模型的 `T60=0.34 s` 作为低频保护：

```math
\alpha_{floor}=\frac{\ln 1000}{0.34}\approx20.3169\ \mathrm{s^{-1}}.
```

它与 `α=πη0f` 的交点为：

```math
f_c=\frac{\alpha_{floor}}{\pi\eta_0}\approx281.18\ \mathrm{Hz}.
```

因此 `281 Hz` 不是实验测得的物理临界频率，而只是旧模型低频 floor 与 `2.3%` 主干的工程交点。

文献在约 `1.2–1.5 kHz` 区域观察到 damping rate 上升到约 `130 s^-1` 的趋势，并将其与声辐射损耗增强联系起来，因此 v2a 在 `1.2–1.5 kHz` 之间由 `πη0f` 平滑过渡到 `130 s^-1`。这里“趋向 130”的趋势来自实验，而具体采用 cubic smoothstep：

```math
s(x)=3x^2-2x^3
```

进行连接，是工程插值选择。`1.5–1.8 kHz` 暂时固定 `α=130 s^-1`，作为过渡桥梁；在 `1.8 kHz` 附近，`π·0.023·1800≈130.06 s^-1`，因此可以自然重新接回 `η≈2.3%` 的主干。`1.8–3 kHz` 再采用 `α=πη0f`。由于现有文献对更高频的直接约束有限，第一版不把 `η=2.3%` 无限制外推到 20 kHz，而是在 `3 kHz` 以后暂时固定：

```math
\alpha_{cap}=\pi\cdot0.023\cdot3000\approx216.77\ \mathrm{s^{-1}}.
```

因此当前完整 target 为：

```math
\alpha_{target}(f)=
\begin{cases}
20.3169, & f\le281\ \mathrm{Hz},\\
\pi(0.023)f, & 281<f<1200\ \mathrm{Hz},\\
\text{smooth transition to }130, & 1200\le f<1500\ \mathrm{Hz},\\
130, & 1500\le f<1800\ \mathrm{Hz},\\
\pi(0.023)f, & 1800\le f\le3000\ \mathrm{Hz},\\
216.77, & f>3000\ \mathrm{Hz}.
\end{cases}
```

其中低频 floor、`1.5–1.8 kHz` 平台以及 `3 kHz` 以上 cap 都应被明确视为工程假设，而不是直接实验测量结果。

## 5. 从连续时间阻尼转换到 FDN Branch Target

Soundboard FDN 的 8 条 delay 长度为：

```text
37, 87, 181, 271, 359, 492, 687, 721 samples
```

纯 delay 本身只改变相位：

```math
D_i(f)=e^{-j2\pi fL_i/F_s},
\qquad
|D_i(f)|=1.
```

所以真正负责衰减的是每条 branch 上的 loss filter。长度为 `Li` 的 delay 对应一次 traversal 的真实时间：

```math
\tau_i=\frac{L_i}{F_s}.
```

连续时间振幅按照 `A(t)=A0e^{-αt}` 衰减，因此第 `i` 条 branch 在频率 `f` 上一次 traversal 应具有：

```math
|H_i^{target}(f)|
=
\exp\left[-\alpha_{target}(f)\frac{L_i}{F_s}\right].
```

这意味着 8 条 branch 并不分别对应不同频段；每一条 branch 都携带宽带信号。频率 `f` 决定该模态单位时间应衰减多快，`Li/Fs` 决定这一条 branch 一次经过了多长时间。长 delay 每次 traversal 的 dB 损耗更深，是因为它对应更长的真实 elapsed time，而不是因为它“负责高频”。

`target_damping.py` 当前只完成这一层：定义 `α_target(f)`，计算派生的 `η(f)`、`T60(f)`，并生成 8 条 branch 的理想衰减曲线。它还没有决定最终的 SOS 系数。下一阶段才会用有限个二阶节去逼近这些 target curves，再通过完整 FDN 的 exact-pole 分析检查实际得到的 modal frequency、pole radius、`α`、`η` 与 `T60` 是否符合目标，同时确认没有引入不希望的闭环副作用。
