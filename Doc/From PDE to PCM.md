# From PDE to PCM: Physical Modeling in the Digital Domain

# 从波动方程到物理建模

**Author:** Ziyang Tan  

---

## Preface

## 前言

 All questions and research begin with a fundamental problem:

所有问题与研究都始于一个根本问题：

> **How can we represent musical instruments efficiently and accurately in terms of parameters and code?**  

> **我们如何高效且准确地将乐器转化为参数与代码？**

This work attempts to answer this question by starting from the most fundamental physical model and progressively building toward practical implementations.

本文试图从最基础的物理模型出发，逐步构建通向工程实现的路径，以回答这一问题。

---

## 1 Introduction  
## 1 引言

As pointed out in previous work, the piano is a particularly interesting instrument due to both its prominence in Western music and its complex physical structure.

正如已有研究所指出的，钢琴是一种非常具有代表性的乐器，不仅因为其在西方音乐中的重要地位，也因为其复杂的物理结构。

> *“The piano is a particularly interesting instrument, both for its prominence in western music and for its complex structure. Also, its control mechanism is simple (it basically reduces to key velocity), and physical control devices (MIDI keyboards) are widely available which is not the case for other instruments.”*  
>
> *“The source-based approach can be useful not only for synthesis purposes but also for gaining a better insight into the behavior of the instruments. However, as we are interested in efficient algorithms, the features modeled are only those considered to have audible effects. In general, there is a trade-off between the accuracy and the simplicity of the description.”*  
>
> — Bank et al., 2003

From an engineering perspective, this observation leads to a central question:

从工程角度来看，这一观察引出了一个核心问题：

> **How can we balance physical accuracy and computational efficiency in sound synthesis?**  

> **我们如何在物理真实与计算效率之间取得平衡？**

---

## 2 Research history

## 2 研究历史

全量化地建模任何一种乐器都绝非易事，尽管本文会声明钢琴是主要的研究对象，但钢琴拥有极其庞杂的物理信息，我相信在这样的研究过程中，我将听到的声音大部分也和钢琴本身没有太大关系，但同时也不难发现的是，物理方法建模钢琴并非一朝一夕，早在1987年Garnett就尝试通过数字波导替代全离散PDE的方法来模拟钢琴的全部系统，….



___

## 3 Pulse Code Modulation

## **3 脉冲编码调制**

在物理层面中**声音的本质是介质中的压力扰动传播**，可对于现代人来说，我接触到的声音通常都不是由一个物理声源发出，而是通过声卡，尽管我很想顺便研究声卡的工作原理，但作为一般情况下的最终输出，还是决定从PCM入手，所有的发声引擎都遵循：



>  UI 指令  -  引擎  -  PCM 流  -  声卡



如果我想在研究的过程中实时听到效果，我必须从右端开始并逐渐向左研究

**PCM（Pulse Code Modulation，脉冲编码调制）是一种将连续模拟信号转换为离散数字信号**的方法，广泛用于音频采集、存储与传输。

生成 PCM 需要完成三个步骤

1. **采样（Sampling）**：把连续时间信号变成离散时间点

   根据**奈奎斯特定理（Nyquist Theorem）**，为了避免**混叠（aliasing）**，**采样率必须大于或等于信号最高频率的 2 倍**，**人耳上限约 20 kHz**，故至少需要 40 kHz 的采样率，也就是至少要一秒钟记录音频函数 $$y(x,t)$$ 的 $$y$$ 值 40000 次，从而我们能够得到一个关于 y 在 $$t2-t1$$ 间的 $$y$$ 的离散数组

   

2. **量化（Quantization）**：把连续幅值变成有限级别

   通过采样获得的离散数组从数值上并不能直接喂给声卡，因为函数是理论无限连续的，采样时的精度完全取决于小数点后保留的位数，这显然是不可靠的，声卡作为一种物理器械有它固定的精度，现代声卡的精度（比如我的 Macbook Air 2025）， 尽管系统 Core Audio 的处理精度是 **32-bit float**，但声卡的最终输出（DAC）精度是 **24-bit float**，DAC是声卡中的一个核心部件，它的作用是将PCM转换成模拟电压然后传给放大器然后声音从喇叭放出。

   在解释精度的详细运算之前，我需要先解释**归一化**，也就是把采样离散数组无损映射在 $$[-1,1]$$ 之间，但我需要声明的是，这样的操作也不是必须的，

   我们假设可用值（精度）只有 3 bit（8 个等级，算法：$$2^3 = 8 $$  接着归一化离散）：
   -1.0, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75

   如果采样值是：0.68

   那就会根据如下的公式

   x_q[n] = round(x[n] / Δ) * Δ

   “找最近的台阶”，量化后为 0.75 ，

   如此一来这就在没有归一化的情况下量化了采样值，

   但假设采样值为 2.3，就会发生被截断为1，产生**clipping（削波）**从而**严重失真（clipping distortion）**

   故在量化之前进行归一化是为了避免这样对声音的影响，

   在有了这样的教训之后，我把采样数组统一地压缩为类似 [0.43214224, -0.10, 0.2222345, 0, 0, 0.78, 1, -1] 这样的形式再进行归一化

   

3. **编码（Encoding）**：把量化值转成二进制数据 

   > 模拟值 → 量化等级 → 二进制
   > 0.73 → Level 187 → 10111011

   接着通过声卡就能听见声音



不难看出，整个工作流最后的输出都将以PCM流的方式呈现

接着我会尝试使用 cpp 编写 y(x,t) （音频波）的生成器，选用 swift 编写接收并加工这一信息的声卡接口

cpp 在音频加工的领域拥有极好的生态，而 swift 拥有优美与完善的硬件生态，这是我选用这两种语言的原因

先谈 swift 应该接收什么样的数据类型，





___

## 4 The One-Dimensional String Model

## 4 一维弦振动模型

检验一个建模的好坏，最好的方式就是倾听它所发出的声响，而为了还原这样声音的同时又不希望伴随太大的工程难度，我只好聚焦它最核心的部分，尽管我明白击弦锤和音板都同等重要，但钢琴的主要储能部分仍然是弦，作为工程最开始的最简模型，为了简易地描述钢琴声响的由来，我需要检验这种扰动的根源: 弦的振动方程

“连续体”（continuum）是一种理想化模型，其核心含义是：**物质在空间中被视为连续分布的介质，而非由离散粒子组成**。这一假设使得我们可以用连续函数来描述物理量（如密度、速度、温度等）在空间和时间中的变化。在宏观尺度上，忽略物质的分子或原子结构，将其视为连续分布。

我不能够分别地去描述弦上每一个原子的运动过程，但弦在宏观上正是一个连续体

故弦在位置 x 时间 t 的位移 y 可以用连续函数 $$y(x,t)$$ 描述

我们引出一维弦振动公式并尝试证明它，这也是本文的第一个**PDE = Partial Differential Equation**偏微分方程: 

考虑一根拉紧的弦：弦的平衡位置沿 x 轴，横向位移记为 y(x,t)，弦上的张力大小为 T，线密度（单位长度质量）为  $$\mu$$

假设：振动很小，弦的伸长可以忽略，张力 T 近似处处相同，只考虑横向运动

取一小段弦元，取弦上从 $$x$$ 到 $$x+\Delta x$$ 的一小段，这小段弦两端都受张力，方向沿弦的切线方向。

左端张力：$$T$$，与 $$x$$ 轴夹角为 $$\theta(x,t)$$

右端张力：$$T$$，与 $$x$$ 轴夹角为 $$\theta(x+\Delta x,t)$$

因为只考虑横向振动，所以我们看 y 方向的受力。

列牛顿第二定律：

这段弦元质量为 $$\mu \Delta x$$，横向加速度约为 $$\frac{\partial^2 y}{\partial t^2}$$，因此横向合力满足

$$\mu \Delta x \frac{\partial^2 y}{\partial t^2} = T\sin\theta(x+\Delta x,t)-T\sin\theta(x,t)$$

即

$$\mu \Delta x \frac{\partial^2 y}{\partial t^2} = T\bigl[\sin\theta(x+\Delta x,t)-\sin\theta(x,t)\bigr]$$

因为振动很小，弦的斜率很小，所以

$$\tan\theta \approx \sin\theta \approx \frac{\partial y}{\partial x}$$

于是

$$\sin\theta(x,t)\approx \frac{\partial y}{\partial x}(x,t)$$

$$\sin\theta(x+\Delta x,t)\approx \frac{\partial y}{\partial x}(x+\Delta x,t)$$

代回去：

 $$\mu \Delta x \frac{\partial^2 y}{\partial t^2} = T\left[ \frac{\partial y}{\partial x}(x+\Delta x,t) - \frac{\partial y}{\partial x}(x,t) \right]$$ 

化为二阶空间导数并将上式除以 $$\Delta x：$$

$$\mu \frac{\partial^2 y}{\partial t^2} = T\, \frac{ \frac{\partial y}{\partial x}(x+\Delta x,t) - \frac{\partial y}{\partial x}(x,t) }{ \Delta x }$$ 

令 $$ \Delta x\to 0$$，右边变成二阶导数：

$$\mu \frac{\partial^2 y}{\partial t^2} = T\frac{\partial^2 y}{\partial x^2}$$ 

于是得到

$$\frac{\partial^2 y}{\partial t^2} = \frac{T}{\mu}\frac{\partial^2 y}{\partial x^2}$$ 

记

$$c^2=\frac{T}{\mu}$$ 

便是

## $$\frac{\partial^2 y}{\partial t^2} = c^2\frac{\partial^2 y}{\partial x^2}$$

$$\frac{\partial^2 y}{\partial t^2}$$ 是加速度 a

$$y(x)$$ 是当时间为常量，即静止时候的波形状

故 $$\frac{\partial^2 y}{\partial x^2}$$ 为空间函数的二阶导数即波在某一静止时间上的弯曲率 $$\kappa$$

即 $$a=c^2\kappa$$

**加速度 = 波速² × 曲率**

这个方程的通解是：

$$y(x,t) = f(x - ct) + g(x + ct)$$

$$f(x)$$ 是一个固定形状的函数，而 $$ct$$ 则说明这个函数在向着 x 轴的方向移动

这个方程说明了**弦的振动 = 两个沿相反方向传播的波的叠加**























