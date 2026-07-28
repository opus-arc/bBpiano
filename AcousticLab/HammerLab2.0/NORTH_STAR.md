# HammerLab2.0 北极星：目标、判据与工程管线

> 本文是 HammerLab2.0 的最高优先级目标文档。若 `README.md`、`PROJECT.md`、实验脚本或临时研究方向与本文冲突，以本文为准。

## 1. 唯一目标

HammerLab2.0 只为解决一个工程问题：

> 找到一个能够替换 bBpiano 当前 Bank 简化锤模型、行为更接近可信钢琴锤、可以稳定接入现有 digital waveguide、并满足实时预算的锤弦接触模型。

HammerLab2.0 不是最终音源，不是论文复现项目，也不是为了无限扩展锤毡研究功能。它是候选锤模型的试验场、辨识器和淘汰器。

最终闭环必须收束为：

```text
当前 Bank 基线
      ↓
提出最小复杂度候选模型
      ↓
接入同一 bBpiano DWG 弦端口
      ↓
比较弦激励、Pianoteq 行为指纹、稳定性和实时成本
      ↓
无明确收益 / 不稳定 / 太昂贵 ──> 淘汰
      ↓
最简单且通过全部门槛的模型 ──> 替换 Bank
```

## 2. HammerLab2.0 必须回答的四个问题

每项设计和工具都必须直接服务以下至少一个问题：

1. 候选模型是否表现出当前 Bank 无法表现、且有证据支持的锤毡行为？
2. 候选模型产生的弦激励是否在力度、硬度和 partial 分布上更接近目标行为？
3. 候选模型接入真实 bBpiano DWG 后是否稳定、被动、可重复？
4. 候选模型是否足够简单，能够在完整音区和最大目标复音下实时运行？

不能帮助回答这四个问题的功能不进入当前主线。

## 3. 当前 Bank 的基线地位

当前 Bank/Hammer-P 方案近似使用：

\[
F=K[x]_+^p
\]

并通过延迟力反馈与弦交互。它必须作为 M0 基线保留，因为任何新模型都需要证明自己比它更好，而不是仅仅拥有更多参数。

Bank 的主要表达限制是：

- 只有一条固定的非线性压缩曲线；
- 不能独立表达表层、中层、深层硬度；
- 缺少明确的加载/卸载差异和锤毡记忆；
- soft 与 fortissimo 的差异主要来自压缩量变化，材料结构自由度不足；
- 参数变化对各 partial 的作用逻辑没有被测量和约束。

替换模型不需要复刻某篇论文或 Pianoteq 内部公式，但必须用可观测证据证明这些限制得到了有价值的改善。

## 4. 我们所说的“激励信号”

钢琴锤的激励不是一段预先保存的波形。它是锤状态和弦状态共同作用产生的闭环结果：

```text
锤速度、质量、压缩历史、硬度剖面
                    +
弦在击弦点的 incident velocity / driving-point admittance
                    ↓
              接触模型与求解器
                    ↓
共享接触力 F_contact[n]
                    ↓
每根弦的 outgoing-wave / velocity injection
                    ↓
各 partial 的初始能量、相位与后续弦响应
```

HammerLab2.0 的核心输出因此是：

- 共享接触力 `F_contact[n]`；
- 每根弦的实际注入信号；
- 接触时长、冲量和注入能量；
- 各 partial 的初始激励能量；
- 上述量对 velocity 和 hardness 控制的变化率；
- 数值稳定性、能量一致性和实时成本。

`F(t)` 是激励的物理载体和诊断量，不是项目终点。最终必须观察它如何转化为真实 DWG 中的 wave injection 和 partial excitation。

## 5. Pianoteq 8 的角色：黑盒行为 oracle

Pianoteq 8 不只是“听感方向校准”，也不被当作内部参数真值。它的主要用途是提供可重复查询的黑盒行为：

> 在固定 instrument、string、soundboard、output 和 velocity 条件下，Piano、Mezzo、Forte 三个 Hammer hardness 控制如何改变各 partial 的初始激励比例。

我们只使用自有许可证产生的合法输出，不检查或修改程序二进制，不提取内部代码或受限素材，不分发 reference WAV。

### 5.1 差分消去未知下游系统

对固定 note、velocity 和输出链，第 \(k\) 个 partial 可近似写成：

\[
Y_k(h)=H_k X_k(h)
\]

其中：

- \(X_k(h)\) 是锤弦接触对该 partial 的激励；
- \(H_k\) 是弦、琴桥、音板、辐射和输出链的固定传递；
- \(Y_k(h)\) 是最终 WAV 中可观测的 partial。

只扰动 Hammer hardness 时，局部近似下 \(H_k\) 不变，因此：

\[
\frac{\partial \log |Y_k|}{\partial h}
\approx
\frac{\partial \log |X_k|}{\partial h}
\]

这不能恢复 Pianoteq 的内部模型，却能得到其可观测的 partial 调控逻辑。

### 5.2 行为 Jacobian 指纹

定义三个标准化控制：

\[
\mathbf h=[h_P,h_M,h_F]
\]

对每个 partial 提取接触结束附近的初始能量：

\[
\mathbf y=
[\log E_1,\log E_2,\ldots,\log E_N]
\]

用中央差分估计：

\[
J_{k,q}(note,v)=
\frac{
y_k(note,v,h_q+\delta)-y_k(note,v,h_q-\delta)
}{
2\delta
}
\]

最终 reference fingerprint 是：

```text
J_PT[note, velocity, partial, time/estimation-window, hardness-control]
```

它描述：

- 每个 hardness 控制主要作用在哪些力度区域；
- 每个 partial 的能量按什么比例变化；
- 高频激励对 hardness 的敏感度；
- Piano、Mezzo、Forte 控制如何平滑重叠；
- 这种调控逻辑如何随 note 和 velocity 迁移。

### 5.3 不能直接依赖 raw WAV 残差

逐样本残差会被 onset、相位、unison beating、弦损耗、音板和麦克风差异主导。主比较对象必须是：

1. 绝对 partial 初始能量，用于保留响度和总注入能量；
2. 归一化 partial distribution，用于比较音色比例；
3. hardness Jacobian，用于比较调控逻辑；
4. Jacobian 随 velocity 的轨迹，用于比较表层到深层的连续过渡；
5. 必要时的二阶差分/Hessian，用于比较控制交叉和非线性。

Raw waveform residual 只作为最终辅助诊断，不作为候选模型的主排名依据。

### 5.4 Partial 初始能量估计

不能仅对固定 0–5 ms 窗口做 FFT；低音 partial 在该窗口中可能不足一个周期。目标分析器应：

1. 检测并亚采样对齐 onset；
2. 估计实际 partial 频率；
3. 在接触后的可用窗口拟合阻尼复正弦；
4. 将复振幅反推到接触结束附近；
5. 由复振幅计算各 partial 的初始能量和相对相位；
6. 单独保存最早期 broadband transient 指标。

## 6. 第一代候选模型

第一轮不追求终极模型，只测试最小的结构增量：

| ID | 候选 | 目的 |
| --- | --- | --- |
| M0 | 当前 Bank/Hammer-P | 必须保留的复杂度和声音基线 |
| M1 | 连续 depth-hardness backbone | 测试表层/中层/深层硬度是否带来正确的力度选择性 |
| M2 | M1 + 1 个记忆支路 | 测试最小滞回/松弛状态是否提供额外可观测价值 |
| M3 | M1 + 2 个记忆支路 | 仅在 M2 无法覆盖快慢时间尺度时保留 |

柔性锤柄、分布式接触、三支路以上记忆、神经网络和复杂接触几何暂不进入第一轮。只有现有候选出现可定位且无法用更简单结构解决的残差时，才允许增加复杂度。

### 6.1 深度硬度不是 velocity lookup

Piano、Mezzo、Forte 不能实现为三个 velocity 区间直接切换三套 `K`。正确因果关系是：

```text
impact velocity
      ↓
实际压缩深度 x(t)
      ↓
访问连续 hardness-versus-depth profile
      ↓
生成接触力和弦激励
```

推荐的 M1 backbone 使用始终为正的切线刚度：

\[
k_t(x)=k_{\mathrm{base}}(x)
\exp\left(
h_P B_P(x)+h_M B_M(x)+h_F B_F(x)
\right)
\]

\[
F_{\mathrm{eq}}(x)=\int_0^x k_t(s)\,ds
\]

\(B_P,B_M,B_F\) 是浅、中、深压缩区的平滑重叠基函数。这一形式保证力连续、切线刚度为正，并允许三个控制拥有局部但不硬切换的深度作用范围。

该形式是第一代推荐候选，不是不可修改的最终真理；它必须与更简单基线一起接受实验淘汰。

## 7. 模型选择损失

候选模型在同一输入矩阵上生成自己的 fingerprint：

```text
J_candidate[note, velocity, partial, window, hardness-control]
```

主损失为：

\[
\mathcal L =
\lambda_0 \mathcal L_{\mathrm{partial}}
+\lambda_J \mathcal L_{\mathrm{Jacobian}}
+\lambda_V \mathcal L_{\mathrm{velocity}}
+\lambda_E \mathcal L_{\mathrm{energy}}
+\lambda_P \mathcal L_{\mathrm{physics}}
+\lambda_C \mathcal L_{\mathrm{complexity}}
\]

其中：

- `L_partial`：baseline partial 比例与绝对能量误差；
- `L_Jacobian`：三个 hardness 控制对各 partial 的一阶响应误差，主目标；
- `L_velocity`：Jacobian 随力度变化轨迹的误差；
- `L_energy`：接触冲量、总注入能量和能量残差；
- `L_physics`：不拉弦、有限、被动、收敛、合理接触时长等硬约束；
- `L_complexity`：状态数、迭代上界和 CPU 成本。

必须同时报告候选相对 M0 Bank 的改善量。获胜者不是绝对 loss 最低的模型，而是：

> 在 hold-out 上显著优于 Bank、超过 Pianoteq 重复渲染噪声地板，并满足稳定性与实时门槛的最简单模型。

阈值只有在测出 Pianoteq 重复性和本引擎数值噪声后才能冻结，不能提前用任意数字包装进展。

## 8. 证据的优先级

证据按以下顺序使用：

1. **硬约束**：单边接触、有限、稳定、被动、能量方向正确；
2. **Pianoteq differential fingerprint**：partial 比例及其 hardness/velocity Jacobian；
3. **引擎内激励**：真实 DWG wave injection、接触力、接触时长和总能量；
4. **真实锤与论文数据**：限制参数范围、淘汰不合理结构、解释残差；
5. **最终听感**：确认改善有感知价值，但不能单独证明模型正确。

论文曲线和真实台架不是主线终点。它们用于防止黑盒拟合得到非物理、不可泛化的解。

## 9. 端到端工程顺序

下列阶段按依赖顺序执行。每一阶段都必须满足退出条件，才能把下一阶段宣布为完成。

### 9.1 管线产物契约

最终管线由稳定的数据产物连接，而不是由只能人工重复的临时命令连接：

```text
study config
    ↓
Pianoteq capture manifest + reference WAV
    ↓
reference partial table
    ↓
J_PT fingerprint
                              current Bank / candidate config
                                           ↓
                              DWG trace + candidate WAV
                                           ↓
                              candidate partial table
                                           ↓
                              J_candidate fingerprint
                                           ↓
                 comparison report + residual tensor
                                           ↓
                       optimizer / candidate selection
                                           ↓
                   hold-out report + frozen parameters
                                           ↓
                         generated realtime artifact
```

每一层至少需要以下稳定产物：

| 层 | 输入 | 输出 |
| --- | --- | --- |
| Study | note、velocity、hardness perturbation、固定环境 | `study.json` 或等价版本化配置 |
| Oracle capture | study + Pianoteq 8 | capture manifest、checksum、reference WAV |
| Feature extraction | WAV + manifest | partial frequency、complex amplitude、decay、energy table |
| Fingerprint | baseline/`±δ`/`±2δ` feature tables | Jacobian、置信区间、noise floor |
| Candidate render | candidate parameters + real `StringPort` | force/wave trace、summary、candidate WAV |
| Comparison | `J_PT` + `J_candidate` | 分维度 residual、综合 loss、Bank-relative improvement |
| Fitting | comparison + bounds + hard constraints | fitted parameters、训练日志、失败原因 |
| Validation | frozen candidate + hold-out study | hold-out report、稳定性和实时报告 |
| Export | 获胜模型与参数场 | bbpl/bBpiano 可直接读取的版本化实时参数 |

目标终态应提供一个总编排入口，语义类似：

```sh
python3 tools/run_study.py configs/studies/c4_hardness_v1.json
```

该入口只负责编排各阶段和校验产物，不把采集、分析、渲染、拟合逻辑重新复制一份。任何阶段失败时必须停止，并保留可诊断的 manifest 和 failure report。

### E0：冻结契约并修好实验骨架

**建设内容**

- 定义版本化 `ExperimentConfig`、trace、summary 和 fingerprint schema；
- summary 保存所有 felt、solver、port、duration、Git/binary/hash 参数；
- non-finite 或 non-converged 时 CLI 返回非零；
- 建立 1×/2×/4×/8× convergence suite；
- 建立接触功、锤动能、端口注入/吸收能量审计；
- 将 C++ 单测、matrix、schema 和 Python 工具测试接入 CI。

**退出条件**

- 任意正式 case 可由 manifest 单独重现；
- 非收敛不能被流水线当作成功；
- seed matrix 在声明采样率下通过冻结的数值和能量门槛。

### E1：建立 Pianoteq 8 oracle 采集管线

**建设内容**

- 锁定具体 Pianoteq 8.x.x、edition、instrument/preset 和 random state；
- 锁定 velocity curve、condition、strike point、string/soundboard、output；
- 关闭 reverb、effects、limiter 等可关闭干扰项；对无法完全关闭的 Hammer Noise 使用最小值并以重复组量化其噪声地板；
- 支持对标准化 `h_P/h_M/h_F` 做 `±δ`、`±2δ` 扰动；
- 保存 WAV checksum、完整 host 参数、重复组和 capture manifest；
- 第一轮只采 C4 的密集 velocity 网格，稳定后再扩音区。

第一轮建议 velocity：

```text
32, 41, 55, 70, 84, 98, 112, 120
```

每个 velocity 包含 baseline、三个控制各 `±δ`，每个 case 至少重复两次。

**退出条件**

- 重复渲染差异和随机性噪声地板已量化；
- 参数扰动以准确数值记录，不依赖截图或手工描述；
- 不同版本、preset 或输出条件不会混入同一数据集。

### E2：建立 partial 与 Jacobian 分析管线

**建设内容**

- 实现 onset、partial frequency、complex amplitude 和 decay estimator；
- 输出 absolute partial energy 与 normalized partial distribution；
- 实现 `±δ` 中央差分、`±2δ` 稳定性检查；
- 生成 `J_PT`、velocity sensitivity map 和重复性置信区间；
- 只在需要区分一阶等价候选时增加 Hessian；
- 用 synthetic modal signals 建立 recovery test。

**退出条件**

- synthetic 数据能在规定误差内恢复 partial 初始能量；
- 同一 Pianoteq case 的重复 fingerprint 差异小于 hardness 扰动效应；
- 能生成一份固定版本的 C4 reference fingerprint。

### E3：把当前 Bank 接入 HammerLab 的真实 DWG

**建设内容**

- 定义无所有权、无音频线程分配的 `StringPort`；
- port 支持读取 incident velocity、trial response、commit/abort 和能量统计；
- 用 adapter 接入 bBpiano 当前 `StringModel`，避免 HammerLab 直接依赖其所有权；
- 先接 M0 Bank，确保 HammerLab 与主引擎对同一 case 输出一致；
- 实现双弦/三弦共享总接触力和等效并联 admittance。

**退出条件**

- M0 在 HammerLab 与 bbpl 的 contact force/wave injection 可对齐；
- trial iteration 只 commit 一次；
- 无一采样虚假延迟，无未审计的能量增长；
- 同一端口可在不改实验器的情况下切换候选模型。

### E4：实现并筛选第一代候选

**建设内容**

- 实现 M1 depth-hardness backbone；
- M1 通过后再增加 M2 单记忆支路；
- 只有残差证据支持时才增加 M3 双记忆支路；
- 每个候选生成与 `J_PT` 同维度的 `J_candidate`；
- 对 Bank 和候选使用完全相同的 note、velocity、port 和分析器。

**退出条件**

- Piano hardness 主要影响浅压缩/低力度，Forte hardness 随深压缩增强；
- 三个控制随力度连续交叠，没有 velocity 硬切换；
- 至少一个候选在 Jacobian hold-out 上稳定优于 Bank；
- 若没有候选通过，保留 Bank 并回到模型假设，而不是继续堆工具。

### E5：闭合参数搜索与残差诊断

**建设内容**

- 先用有限差分计算 candidate Jacobian；只有成本需要时再引入 autodiff；
- 实现有边界、多起点优化；
- 联合优化 baseline partial、Jacobian、velocity trajectory 和物理约束；
- 把残差按 note、velocity、partial、control 分解；
- 使用论文/台架数据解释剩余残差，决定是参数问题还是模型结构问题；
- 对新增状态设置明确的残差收益门槛。

**退出条件**

- synthetic recovery 通过；
- 训练 velocity 上的改善能迁移到 hold-out velocity；
- 参数不需要随每个 velocity 独立漂移；
- 更复杂候选只有在可重复 residual 上有明确收益时才获准保留。

### E6：跨音区验证与参数场

**建设内容**

- 从 C4 扩展到代表性 C2/C3/C4/C5/C6；
- 对 note 和物理锤尺寸建立低阶连续参数场；
- 保留 velocity 作为动力学输入，不把它变成材料参数 lookup；
- 检查低音长窗口、三弦 unison、高音短接触的分析偏差；
- 冻结 train/validation/hold-out 音高与力度集合。

**退出条件**

- 同一模型结构跨音区成立；
- 参数随 note 平滑，不需要 88 套独立补丁；
- hold-out note/velocity 的 Jacobian 改善仍超过 Bank 和噪声地板。

### E7：生产化与替换 Bank

**建设内容**

- 冻结获胜模型结构和参数生成方式；
- note-on 前准备参数，audio callback 内无锁、无分配；
- 定义非收敛、NaN、denormal 和极端输入 fallback；
- benchmark 48/96 kHz、目标 oversampling 和最大复音；
- 建立 Bank 与新模型可切换的回归路径；
- 先进入 bbpl，再进入 bBpiano App。

**退出条件**

- 实时预算、最大复音、线程和数值压力测试通过；
- 候选在冻结 reference/hold-out 上持续优于 Bank；
- 主引擎 A/B 能确认改善来自锤弦激励，而不是额外 EQ 或后处理；
- 完成回滚路径后才允许替换默认 Bank。

## 10. 当前阶段

当前代码完成了 E0 的实验骨架和 E1 的首条自动化纵向切片：

- 三个原型 felt kernel；
- 简化负载和离线隐式求解；
- trace、summary、smoke matrix；
- MIDI probe 和成对 WAV 评估原型；
- CrossOver/Pianoteq 8.4.3 无界面 renderer；
- 配置驱动、可恢复、逐 case 记录 provenance 的 study runner；
- C4 三层 hardness、三点扰动、三次重复的 27-case heartbeat；
- WAV 格式/静音/削波 gate，以及基于 normalized RMS residual 的重复噪声与 hardness 效应分离 gate；
- Python/NumPy 锁定环境和 C++/Python/smoke 联合自检。

尚未完成：

- E0 的全部收敛/能量/CI 收尾；
- 最终 oracle preset/condition 冻结，以及 C4 密集 velocity、`±δ/±2δ` 的正式 Pianoteq hardness differential 数据集；
- partial 初始能量估计器；
- Jacobian fingerprint；
- 真实 bBpiano `StringPort`；
- depth-hardness 候选；
- candidate fingerprint 优化；
- 生产实时验证。

下一段工程先把已通过的 heartbeat 扩成 E1 正式 C4 DOE，并并行收完 E0 的数值契约；随后严格进入 E2、E3。不能跳过 oracle fingerprint 和真实 DWG 基线，直接把新模型塞进主引擎。

## 11. 决策规则

每次准备增加模型、指标或工具前，回答：

1. 它帮助比较候选与 Bank 的哪一个可观测行为？
2. 它会进入哪个固定数据产物或验收门槛？
3. 如果它失败，我们会淘汰什么假设？
4. 如果它成功，它是否改变模型选择或生产决策？

若四个问题都没有明确答案，该工作不进入当前 HammerLab2.0 主线。
