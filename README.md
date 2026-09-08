 # bBpiano - bBSonicLab Physical Modeling

![Piano Structure](./Doc/design/bBsonicLab_logo_transparent_fullsize.png)

<hr>
<div align="center" style="line-height: 1;">
  <a href="https://github.com/opus-arc/bBpiano/actions/workflows/engine-evaluation.yml" target="_blank"><img alt="bBpiano Engine Evaluation"
    src="https://github.com/opus-arc/bBpiano/actions/workflows/engine-evaluation.yml/badge.svg"/></a>
  <br>   
  <a href="https://github.com/opus-arc/bBpiano/milestone/1" target="_blank"><img alt="Foundational Research Progress"
    src="https://img.shields.io/github/milestones/progress-percent/opus-arc/bBpiano/1?label=foundational%20research"/></a>
  <a href="https://github.com/opus-arc/bBpiano/milestone/1" target="_blank"><img alt="Milestone Issues"
    src="https://img.shields.io/github/milestones/progress/opus-arc/bBpiano/1?label=issues"/></a>
  <br>
  <a href="https://opus-arc.github.io/bBpiano/"><b>Primary Research Document</b>👁️</a>
</div>

> Every resonance eventually fades.  
> What remains is the desire to keep creating, to keep moving, and perhaps, one day, to become as free as the bluebird.
>
> ***Bluebird** comes from David Bowie's final farewell, **Lazarus**:*
> *"Just like that bluebird, I'll be free."*

> 每一次共鸣最终都会消失。
>
> 剩下的是继续创造的愿望，继续前进，也许有一天，会像蓝鸟一样自由。
>
> bB 来自大卫·鲍伊最后的告别，**Lazarus**：
>
> *“Just like that bluebird, I'll be free.”*

##     1. Introduction

bBpiano is a physical modeling piano synthesis project inspired by Pianoteq 9, currently in an active research and development stage. At its core is a physically modeled piano engine, designed to remain lightweight and responsive while capturing the immediacy, presence, and expressive vitality of a live instrument.

bBpiano是一个受Pianoteq 9启发的物理建模钢琴合成项目，目前处于积极的研发阶段。 其核心是一个物理建模的钢琴引擎，旨在保持轻巧和响应，同时捕捉现场乐器的即时性、存在感和表现力。

> [!IMPORTANT]
> Readers are strongly encouraged to begin with [**From PDE to PCM: Physical Modeling in the Digital Domain**][].
> The report documents the theoretical foundations, mathematical derivations, engineering implementation process, and design rationale behind bBpiano, providing a complete path from physical equations to a working piano synthesis engine.
>
> Read Online: https://opus-arc.github.io/bBpiano/
>
> 强烈鼓励读者从从PDE到PCM：数字领域的物理建模开始。
>
> 该报告记录了bBpiano背后的理论基础、数学推导、工程实施过程和设计理由，提供了从物理方程到工作钢琴合成引擎的完整路径。
>
> 在线阅读：https://opus-arc.github.io/bBpiano/

### **A Note from bBSonicLab**

bBpiano is released as an open research project.

bBpiano 以开放研究项目的形式发布。

It is neither the product of a large company nor the work of a dedicated acoustics institute. Much of it has been built through curiosity, experimentation, and countless attempts to understand problems that often seemed larger than the people studying them.

它既非大型公司的产品，也并非出自专业的声学研究机构。它的大部分工作，源于好奇、实验，以及无数次试图理解那些常常显得远比研究者自身更加庞大的问题。

The project may be incomplete.

这个项目或许并不完整。

Its models may be imperfect.

它的模型或许并不完美。

Its understanding of the piano is certainly unfinished.

而它对于钢琴的理解，也无疑仍远未完成。

Yet we believe there is value in exploring these questions openly.

但我们相信，以开放的方式探索这些问题，本身便具有价值。

If previous generations left behind instruments, scores, recordings, and performances, perhaps our generation can also leave behind something of its own — algorithms, models, experiments, and a persistent desire to understand why sound moves us.

如果过去的世代为我们留下了乐器、乐谱、录音与演奏，那么或许，我们这一代人也能够留下些什么属于自己的东西——算法、模型、实验，以及一种始终不愿放弃的愿望：去理解声音为何能够触动我们。

This repository is one small shelter built around that pursuit.

这个仓库，便是围绕这份追寻搭起的一处小小庇护所。

Whatever knowledge, craftsmanship, beauty, or mistakes are contained within it are shared in the hope that others may continue the journey further.

无论其中包含的是知识、技艺、美，抑或错误，我们都愿将它们公开于此，并期待后来者能够沿着这条道路，继续走向更远的地方。

— bBSonicLab

## 2. **Project Philosophy**

Modern virtual pianos are predominantly based on sample playback. While high-quality sample libraries can achieve remarkable realism, they fundamentally rely on storing and replaying vast collections of recorded audio.

现代虚拟钢琴主要建立在采样回放技术之上。尽管高质量的采样音源能够达到令人惊叹的真实感，但从根本上说，它们仍依赖于存储并重放规模庞大的录音素材。

bBpiano explores a different direction.

bBpiano 探索的是另一条道路。

Rather than preserving the sound of an instrument as recordings, bBpiano investigates whether it is possible to preserve the instrument itself.

与其通过录音保存一件乐器的声音，bBpiano 更希望探索另一种可能：我们是否能够保存这件乐器本身。

The goal is not merely to reproduce waveforms, but to uncover the underlying principles that give rise to them — the relationships between vibration, energy, material, structure, and sound.

我们的目标并非只是复现波形，而是试图揭示产生这些波形的内在原理——振动、能量、材料、结构与声音之间彼此作用的关系。

Through mathematics, parameters, and computation, bBpiano seeks to reconstruct the essence of an acoustic piano, and ultimately allow it to produce a sound that listeners can no longer reliably distinguish from the physical instrument that inspired it.

通过数学、参数与计算，bBpiano 试图重构一架声学钢琴的本质，并最终使这一模型所产生的声音，达到听者无法再可靠地将其与作为原型的真实乐器区分开来的程度。

This approach offers several potential advantages:

这种方法具有若干潜在的优势：

- **Compact Representation — 紧凑的表征** 

  Instrument behavior is described by parameters and algorithms rather than multi-gigabyte sample libraries. 乐器的行为由参数与算法描述，而非依赖数以 GB 计的采样音源。

- **Continuous Expressiveness — 连续的表现力**

  Dynamics, articulation, and transitions emerge continuously from physical interactions rather than interpolation between discrete recordings.

  力度、奏法与状态之间的过渡直接从连续的物理相互作用中产生，而非通过离散录音样本之间的插值得到。

- **Physical Interpretability — 物理可解释性**

  Individual acoustic phenomena can be analyzed, modified, measured, and improved directly within the model.

  各种独立的声学现象都能够在模型内部被直接分析、修改、测量与改进。

- **Scalability — 可扩展性**

  Improvements to the underlying model benefit the entire instrument without requiring complete re-recording sessions.

  对底层模型的改进能够作用于整件乐器，而无需重新进行完整的录音采样。

- **Research Value — 研究价值**

  The instrument becomes an explorable physical system rather than a fixed collection of audio assets.

  乐器由此成为一个可以被探索、分析与实验的物理系统，而不再只是一组固定的音频素材。

  

The long-term vision is to investigate whether physically modeled instruments can simultaneously achieve:

从长期来看，bBpiano 希望探索：物理建模乐器是否能够同时实现以下目标：

- the realism expected from modern professional instruments,

  现代专业级虚拟乐器所要求的真实感；

- the responsiveness required for live performance,

  现场演奏所要求的即时响应能力；

- the portability demanded by contemporary computing environments,

  当代计算环境所要求的可移植性；

- and the compactness impossible for traditional sample-based approaches.

  以及传统采样式方法难以企及的紧凑性。

  

Ultimately, bBpiano asks a simple question:

归根结底，bBpiano 所追问的只是一个简单的问题：

Can the soul of an acoustic instrument be reconstructed through mathematics and computation alone?

**仅凭数学与计算，我们能否重构一件声学乐器的灵魂？**



## 3. bBpiano Physical Modeling Pipeline

```text
       Physics Domain
────────────────────────────

            MIDI
          演奏动作
             ↓

    Hammer Launch Velocity
   	  锤体对弦的激励速度
   
             ↓

   Tominaga–Sato 2026 
   two-layer nonlinear Maxwell-type impact hammer model
   
   Tominaga–Sato 2026 
   双层非线性麦克斯韦击锤模型
   
             ↓

     String Waveguide
     			波导弦
     			
             ↓
              
    			 FIR
   	有限长单位冲激响应
   （Finite Impulse Response）滤波器
    用于微调不同泛音的能量
    钢琴音符整体存在较稳定的泛音能量频谱曲线
    
             ↓
     
     Loss Biquid Filter
       二阶损失滤波器
   
             ↓
     
     Fractional Delay
     计算Group Delay
     修复FIR与Biquid带来的相位差
             ↓

     Dispersion Network
        模拟色散网络

             ↓

       Bridge Coupling
          音桥耦合
           
             ↓

        Soundboard
      模拟音板修整波形
       			
       			↓
       			
   Multiple pole pitch correction
   复极点副波峰模拟
  		
  					↓
  					
  	  PCM
  		Pulse Code Modulation
  		脉冲编码调制
  		离散数字信号
  		
  					↓
  					
  		Soundcard ouput
  		声卡输出
  			
  					↓
  					
  	The sound of the piano
  		类似钢琴的声响
       			
```

## 4. Quick Start

### Installation

```bash
brew install opus-arc/tap/bBpiano-L
```

## 5. Evaluation Results

To evaluate the acoustic realism and synthesis quality of bBpiano, we compare synthesized audio against reference recordings from the MAESTRO Yamaha Disklavier dataset. Baseline systems include Pianoteq 8 (physical modeling) and a conventional SF2 sampled piano. Current evaluations focus on model efficiency, representation-level similarity, and perceptual audio quality.

为了评估 bBpiano 在声学真实感与合成质量方面的表现，我们将其合成音频与 MAESTRO 数据集中 Yamaha Disklavier 的真实演奏录音进行比较。基准系统包括 Pianoteq 8（物理建模）以及传统的 SF2 采样钢琴。目前的评估主要关注模型效率、表征层面的相似度以及感知音频质量。

In addition to the MAESTRO Yamaha Disklavier dataset, selected evaluations also incorporate recordings from the Iowa Electronic Music Studios (Iowa EMS) Steinway Model B dataset. Since the Iowa dataset provides isolated piano recordings rather than aligned MIDI performances, note events and velocity information are automatically estimated using a pretrained piano transcription model to construct a unified benchmarking sequence. This allows direct comparisons between bBpiano, physical-modeling instruments, and sample-based pianos under controlled and reproducible conditions.

除 MAESTRO Yamaha Disklavier 数据集之外，部分评估还引入了 Iowa Electronic Music Studios（Iowa EMS）所提供的 Steinway Model B 钢琴录音。由于 Iowa 数据集提供的是独立钢琴录音，而非与 MIDI 对齐的演奏数据，因此我们使用预训练的钢琴转录模型自动估计其中的音符事件与力度信息，并据此构建统一的基准测试序列。由此，bBpiano、其他物理建模乐器以及采样式钢琴得以在受控且可复现的条件下进行直接比较。

*Detailed test results and statistical data are included in the attachment below.*

*完整的测试结果与统计数据收录于下方附件中。*Engine Overview

| Category | Maestro Dataset | Pianoteq 8 | SF2 (Grand Piano) | bBpiano L0-Pizzicato |
|:----------|:----------:|:----------:|:----------:|:----------:|
| Type | Reference Recording | Physical Modeling | Sample-Based | Physical Modeling |
| Size | 1–10 GB (test subset) | **380 KB** | 36 MB | 641 KB |
| Real-Time Synthesis | ❌ | **✅** | ❌ | **✅** |
| Polyphony | N/A | **5 × 88** (M4) | N/A | 1.57 × 88 (M4) |

---

### VISQOL

VISQOL evaluates perceptual audio quality by estimating similarity between synthesized and reference recordings.

VISQOL 通过估计合成音频与参考录音之间的相似程度，对感知音频质量进行评估。

The Scale and Polyphony sections use Pianoteq 9 as the standard, while the standard reference recordings for the performance sections are taken from the Yamaha Disklavier subset in the MAESTRO dataset. Higher values indicate stronger perceptual similarity to the reference recordings.

Scale 与 Polyphony 部分以 Pianoteq 9 作为参考标准，而 Performance 部分的标准参考录音则取自 MAESTRO 数据集中的 Yamaha Disklavier 子集。数值越高，表示待测音频与参考录音在感知层面具有越高的相似度。

| Method | VISQOL Score |
|:---------|---------:|
| MAESTRO Yamaha Disklavier | Reference |
| Pianoteq 9 | 2.8466 TBD |
| SF2 (Grand Piano) | **2.9008** TBD |
| bBpiano L0-Pizzicato | 2.2199 TBD |
| **bBpiano L0-100c** | 2.4260 TBD |
| bBpiano L0-beta | 2.3532 TBD |

#### **Pianoteq 8-Referenced Benchmark**

In addition to the MAESTRO-referenced performance evaluation, bBpiano maintains a separate benchmark using **Pianoteq 8 as the reference system**.

除了MAESTRO参考的绩效评估外，bBpiano还维护了一个单独的基准，使用Pianoteq 8作为参考系统。

| Method                     | VISQOL Score |
| :------------------------- | -----------: |
| Pianoteq 8                 |    Reference |
| **Tominaga et al. (2026)** |   3.0820 TBD |

---

### LAION-CLAP Similarity

The Scale and Polyphony sections use Pianoteq 9 as the standard, while the standard reference recordings for the performance sections are taken from the Yamaha Disklavier subset in the MAESTRO dataset. Higher values indicate a stronger similarity in the CLAP embedding space.

Scale 与 Polyphony 部分以 Pianoteq 9 作为参考标准，而 Performance 部分的标准参考录音则取自 MAESTRO 数据集中的 Yamaha Disklavier 子集。数值越高，表示待测音频与参考录音在 CLAP 嵌入空间中具有越高的相似度。

I believe the values provided by LAION-CLAP reflect more of an overall impression—such as tone, style, and mood—rather than physical accuracy. bBpiano aims to create a model that closely resembles a real piano, but isn’t limited to that style.

我们认为，LAION-CLAP 所给出的数值更多反映的是一种整体性的听觉印象——例如音色、风格与情绪——而非物理层面的准确性。bBpiano 的目标是构建一个能够高度逼近真实钢琴的模型，但它所能够产生的声音与表达，并不应被局限于对真实钢琴的单纯模仿。

| Method | Cosine Similarity |
|:---------|---------:|
| Pianoteq 9 | 0.8045 |
| SF2 (Grand Piano) | **0.8283** |
| bBpiano L0-Pizzicato | 0.2261 |
| **bBpiano L0-100c** | 0.3213 |
| bBpiano L0-beta | 0.4628 |

#### **Pianoteq 8-Referenced Benchmark**

In addition to the MAESTRO-referenced performance evaluation, bBpiano maintains a separate benchmark using **Pianoteq 8 as the reference system**.

除了MAESTRO参考的绩效评估外，bBpiano还维护了一个单独的基准，使用Pianoteq 8作为参考系统。

| Method                     | **Cosine Similarity** |
| :------------------------- | --------------------: |
| Pianoteq 8                 |             Reference |
| **Tominaga et al. (2026)** |            **0.9293** |

---

### Historical Progress

| Category | Benchmark | bBpiano L0-alpha | bBpiano L0-beta | bBpiano L0-100c | bBpiano L0-Pizzicato |
|:---------|:---------|---------:|---------:|---------:|---------:|
| Engine | Binary Size | 1.04 MB | 1.04 MB | 1.1 MB | 641 KB |
| Engine | Real-Time Synthesis | ✅ | ✅ | ✅ | ✅ |
| Engine | Polyphony | 5.21 | 11.27 | 23.34 | 1.57 × 88 |
| LAION-CLAP | Cosine Similarity (MAESTRO Reference) | - | 0.4628 | 0.3213 |  |
| VISQOL | TBD (MAESTRO Reference) | - | 2.3532 | 2.4260 |  |

## 6. License

This repository and all accompanying experimental data are currently released under the [PolyForm Internal Use License 1.0.0](https://github.com/opus-arc/bBpiano/blob/main/LICENSE).

此存储库和所有附带的实验数据目前根据PolyForm内部使用许可证1.0.0发布。

## 7. Contact

If you have any questions, please raise an issue or contact us at arcopus07@gmail.com or https://t.me/arcopus .

如果您有任何问题，请提出问题或通过arcopus07@gmail.com或https://t.me/arcopus与我们联系。





---

# **Appendix A. ViSQOL Benchmark Details**

The following table provides the complete ViSQOL scores used to generate the aggregate results reported in Section 5.

下表列出了用于生成第 5 节汇总结果的完整 ViSQOL 评分数据。

The Scale and Polyphony sections use Pianoteq 9 as the standard, while the standard reference recordings for the performance sections are taken from the Yamaha Disklavier subset in the MAESTRO dataset. Higher values indicate stronger perceptual similarity to the reference recordings.

Scale 与 Polyphony 部分以 Pianoteq 9 作为参考标准，而 Performance 部分的标准参考录音则取自 MAESTRO 数据集中的 Yamaha Disklavier 子集。数值越高，表示待测音频与参考录音在感知层面具有越高的相似度。

## **A.1 Per-Piece Results**

### Scale: 

Scales across different pitch ranges; the primary focus of the test is on subsystems that are not affected by aliasing.

覆盖不同音高区域的音阶测试；该测试主要关注不受混叠影响的各个子系统。

| Piece                                 | SF2 Grand Piano | bBpiano L0-Pizzicato | bBpiano L0-100c | bBpiano L0-beta |
| :------------------------------------ | --------------: | -------------------: | --------------: | --------------: |
| Bass_scale | **3.258543816** | | 2.279772871 | 2.494259218 |
| Tenor_scale | **2.294533822** | | 1.005804746 | 1.864653641 |
| Middle_scale | **2.880688398** | | 2.400253359 | 2.182012820 |
| Treble_scale | **2.836848379** | | 2.216778580 | 1.898298445 |
| High Treble_scale | **1.224602593** | | 1.047544827 | 1.093359043 |

### Polyphony:

Chords in different registers; the primary focus of the testing is the coupled system.

不同音域的和弦；测试的主要重点是耦合系统。

| Piece                                 | SF2 Grand Piano | bBpiano L0-Pizzicato | bBpiano L0-100c | bBpiano L0-beta |
| :------------------------------------ | --------------: | -------------------: | --------------: | --------------: |
| Bass_chords | **2.534555915** | | 2.077254981 | 2.166119601 |
| Tenor_chords | **2.672740293** | | 2.514874952 | 1.876183765 |
| Middle_chords | **3.429775842** | | 2.862829766 | 2.503931537 |
| Treble_chords | **2.521915846** | | 1.859617369 | 1.935985753 |
| High Treble_chords | **1.482525693** | | 1.222981887 | 1.139869631 |

### Perfomance:

This comprehensive test features performances selected from the MAESTRO dataset that, as much as possible, encompass the vast majority of techniques that demonstrate a piano’s quality.

这项综合测试的特点是从MAESTRO数据集中选择的表演，这些数据集尽可能涵盖展示钢琴质量的绝大多数技术。

| Piece                                 | Pianoteq 9 | SF2 Grand Piano | bBpiano L0-Pizzicato | bBpiano L0-100c | bBpiano L0-beta |
| :------------------------------------ | ---------: | --------------: | -------------------: | --------------: | --------------: |
| Etude-Tableaux Op.39 No.5             |     2.9136 |      **2.9863** |          2.285809614 |          2.6995 |          2.4852 |
| Images, Book II "Poissons d'or"       | **2.8029** |          2.7804 |          2.312521172 |          2.5644 |          2.3634 |
| Piano Sonata "From the Street"        |     2.9598 |      **3.0255** |          2.091438597 |          2.4664 |          2.3462 |
| Prel. Chor. Fug.                      |     2.9266 |      **3.0012** |          2.199275409 |          2.5196 |          2.2769 |
| Prelude and Fugue in A Minor, S.462/1 |     2.9671 |      **3.0924** |          2.336302759 |          2.5948 |          2.4029 |
| Prelude and Fugue in D Major, BWV 874 | **2.7859** |          2.6779 |          2.024254124 |          2.2438 |          2.2183 |
| Sonata No.28 Op.101                   |     2.8137 |      **2.8996** |          2.166522251 |          2.3586 |          2.3095 |
| Sonata No.4 Op.30                     |     2.7626 |      **2.8409** |          2.193601278 |          2.4185 |          2.2881 |
| Sonata in B Minor                     | **2.8854** |          2.8354 |          2.213168591 |          2.3827 |          2.2733 |
| Sonata in D Major K.576               |     2.8285 |      **3.0614** |          2.207048216 |          2.0482 |          2.4669 |
| Sonata in D Minor K.141               | **2.6867** |          2.5331 |          2.447243048 |          2.2945 |          2.4932 |
| Sonata in F Minor Op.5                |     2.9342 |      **2.9671** |          2.161659708 |          2.5207 |          2.3144 |

## **A.2 Aggregate Statistics**

| Engine               |       Mean |        Min |        Max |
| :------------------- | ---------: | ---------: | ---------: |
| Pianoteq 9           |     2.8466 | **2.6779** |     2.9671 |
| SF2 Grand Piano      | **2.9008** |     2.5331 | **3.0924** |
| bBpiano L0-Pizzicato |     2.2199 |     2.0243 |     2.4472 |
| bBpiano L0-100c      |     2.4260 |     2.0482 |     2.6995 |
| bBpiano L0-beta      |     2.3532 |     2.2183 |     2.4932 |

## **A.3 Reproducibility Notes**

- Reference dataset: Yamaha Disklavier (MAESTRO)
- Number of excerpts: 12
- Metric: ViSQOL
- Evaluation mode: pairwise comparison against aligned reference recordings

ViSQOL estimates perceptual audio similarity by modeling the relationship between spectral structures observed in the reference and synthesized signals. Higher scores indicate greater perceptual similarity to the original recording.

ViSQOL通过模拟参考信号中观察到的光谱结构和合成信号之间的关系来估计感知音频相似性。 分数越高，感知与原始录音的相似性越大。

Unlike embedding-based metrics such as CLAP, ViSQOL directly evaluates the audio signals themselves and therefore serves as a complementary measure of synthesis quality.

与CLAP等基于嵌入的指标不同，ViSQOL直接评估音频信号本身，因此作为合成质量的补充衡量标准。

> [!NOTE]
>
> An interesting observation is that while bBpiano L0-100c underperforms L0-beta in the CLAP benchmark, it achieves a slightly higher average ViSQOL score. This suggests that improvements in perceptual audio quality do not necessarily translate into higher embedding-space similarity, highlighting the importance of evaluating physical modeling instruments using multiple complementary metrics.
>
> 一个值得注意的现象是：尽管 bBpiano L0-100c 在 CLAP 基准测试中的表现低于 L0-beta，但它却取得了略高的平均 ViSQOL 分数。这表明，感知音频质量的提升并不一定会转化为嵌入空间中更高的相似度，也进一步说明了使用多种相互补充的评价指标来衡量物理建模乐器的重要性。



---

# Appendix B. CLAP Benchmark Details

The following table provides the complete per-piece LAION-CLAP similarity scores used to generate the aggregate results reported in Section 5.

下表列出了用于生成第 5 节汇总结果的完整逐曲目 LAION-CLAP 相似度评分。

The Scale and Polyphony sections use Pianoteq 9 as the standard, while the standard reference recordings for the performance sections are taken from the Yamaha Disklavier subset in the MAESTRO dataset. Higher values indicate a stronger similarity in the CLAP embedding space.

Scale 与 Polyphony 部分以 Pianoteq 9 作为参考标准，而 Performance 部分的标准参考录音则取自 MAESTRO 数据集中的 Yamaha Disklavier 子集。数值越高，表示待测音频与参考录音在 CLAP 嵌入空间中具有越高的相似度。

I believe the values provided by LAION-CLAP reflect more of an overall impression—such as tone, style, and mood—rather than physical accuracy. bBpiano aims to create a model that closely resembles a real piano, but isn’t limited to that style.

我们认为，LAION-CLAP 所给出的数值更多反映的是一种整体性的听觉印象——例如音色、风格与情绪——而非物理层面的准确性。bBpiano 的目标是构建一个能够高度逼近真实钢琴的模型，但它所能够产生的声音与表达，并不应被局限于对真实钢琴的单纯模仿。

## B.1 Per-Piece Results

### Scale:

Scales across different pitch ranges; the primary focus of the test is on subsystems that are not affected by aliasing.

覆盖不同音高区域的音阶测试；该测试主要关注不受混叠影响的各个子系统。

| Piece                                 | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta |
| :------------------------------------ | --------------: | --------------: | --------------: |
| Bass_scale | 0.829674840 | 0.591492534 | 0.387630433 |
| Tenor_scale | 0.896856070 | 0.371731043 | 0.300049216 |
| Middle_scale | 0.720027626 | 0.233823359 | 0.168149158 |
| Treble_scale | 0.718633533 | 0.192326903 | 0.166393086 |
| High Treble_scale | 0.746628761 | 0.304847121 | 0.317520738 |

### Polyphony:

Chords in different registers; the primary focus of the testing is the coupled system.

不同音域的和弦；测试的主要重点是耦合系统。

| Piece                                 | SF2 Grand Piano | bBpiano L0-100c | bBpiano L0-beta |
| :------------------------------------ | --------------: | --------------: | --------------: |
| Bass_chords | **0.737208426** | 0.712322891 | 0.491204530 |
| Tenor_chords | **0.898284912** | 0.612981141 | 0.465305507 |
| Middle_chords | **0.782639027** | 0.344589412 | 0.309159577 |
| Treble_chords | **0.825937271** | 0.233994871 | 0.203228608 |
| High Treble_chords | **0.679073453** | 0.220188931 | 0.261892319 |

### Perfomance:

This comprehensive test features performances selected from the MAESTRO dataset that, as much as possible, encompass the vast majority of techniques that demonstrate a piano’s quality.

这项综合测试的特点是从MAESTRO数据集中选择的表演，这些数据集尽可能涵盖展示钢琴质量的绝大多数技术。

| Piece                                 | Pianoteq 9 | SF2 Grand Piano | bBpiano L0-Pizzicato | bBpiano L0-100c | bBpiano L0-beta |
| :------------------------------------ | ---------: | --------------: | -------------------: | --------------: | --------------: |
| Etude-Tableaux Op.39 No.5             |     0.7640 |      **0.8494** |          0.244967222 |          0.3476 |          0.5452 |
| Images, Book II "Poissons d'or"       | **0.7816** |          0.7704 |          0.225847974 |          0.4385 |          0.4862 |
| Piano Sonata "From the Street"        | **0.8993** |          0.8060 |          0.159824803 |          0.1505 |          0.5820 |
| Prel. Chor. Fug.                      |     0.7994 |      **0.8852** |          0.284094602 |          0.4831 |          0.4735 |
| Prelude and Fugue in A Minor, S.462/1 | **0.9413** |          0.8776 |          0.289103746 |          0.5253 |          0.6226 |
| Prelude and Fugue in D Major, BWV 874 |     0.6436 |      **0.8413** |          0.006589664 |          0.1691 |          0.4318 |
| Sonata No.28 Op.101                   |     0.7851 |      **0.8338** |          0.252199650 |          0.3241 |          0.4066 |
| Sonata No.4 Op.30                     |     0.7962 |      **0.8154** |          0.187488467 |          0.4225 |          0.5763 |
| Sonata in B Minor                     | **0.7088** |          0.7059 |          0.316080213 |          0.2329 |          0.5579 |
| Sonata in D Major K.576               | **0.8754** |          0.8503 |          0.239892751 |          0.2747 |          0.4318 |
| Sonata in D Minor K.141               | **0.8897** |          0.8641 |          0.354716450 |          0.2403 |          0.2966 |
| Sonata in F Minor Op.5                |     0.7701 |      **0.8401** |          0.151858717 |          0.2468 |          0.1436 |

## B.2 Aggregate Statistics

| Engine               |       Mean |        Min |        Max |
| :------------------- | ---------: | ---------: | ---------: |
| Pianoteq 9           |     0.8045 |     0.6436 | **0.9413** |
| SF2 Grand Piano      | **0.8283** | **0.7059** |     0.8852 |
| bBpiano L0-Pizzicato |   0.2261ok |     0.0066 |     0.3547 |
| bBpiano L0-100c      |     0.3213 |     0.1505 |     0.5253 |
| bBpiano L0-beta      |     0.4628 |     0.1436 |     0.6226 |

## B.3 Reproducibility Notes

- Reference dataset: Yamaha Disklavier (MAESTRO)
- Number of excerpts: 12
- Embedding model: LAION-CLAP
- Similarity metric: cosine similarity

Repeated evaluations indicate that CLAP exhibits measurable stochastic variation. Under identical conditions, fluctuations of approximately 5–10% are common, while deviations exceeding 30% have occasionally been observed. Consequently, CLAP scores should be interpreted as approximate perceptual indicators rather than absolute measures of acoustic realism.

多次重复评估表明，CLAP 的结果存在可测量的随机波动。在相同条件下，约 5–10% 的分数变化较为常见，偶尔也会观察到超过 30% 的偏差。因此，CLAP 评分更适合作为一种近似的感知层面参考指标，而不应被视为衡量声学真实感的绝对尺度。

> [!NOTE]
>
> Although the benchmark results of **bBpiano L0-100c** are not as impressive as we had hoped, we still consider it one of our most meaningful models. Compared with the earlier L0-beta version, L0-100c represents an attempt to strike a balance between physical accuracy derived from real-world measurements and the pursuit of a beautiful, musically satisfying tone. Through this process, it gradually developed a unique voice and aesthetic character of its own. Whatever its position in the benchmark tables, it remains a model with a strong personality, carrying within it countless experiments, revisions, and the genuine effort of those who created it.
>
> 尽管 **bBpiano L0-100c** 的基准测试结果并没有达到我们最初所期待的水平，我们仍然认为它是整个项目中最有意义的模型之一。与更早的 L0-beta 版本相比，L0-100c 是一次在源于真实世界测量的物理准确性，与对优美且富有音乐性的音色追求之间寻找平衡的尝试。在这一过程中，它逐渐形成了属于自己的独特声音与审美特征。无论它最终位于基准测试表格中的什么位置，它依然是一个拥有鲜明个性的模型，其中承载着无数次实验与修改，也承载着创造它的人所付出的真实努力。



**bBSonicLab**

![page_end](./Doc/assets/page_end.PNG)

 







