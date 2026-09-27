from pathlib import Path
import math
import csv

import numpy as np
import matplotlib.pyplot as plt


# ============================================================
# 1. 固定参数
# ============================================================

# bBpiano 当前音频采样率
FS = 44100.0

# 当前 Soundboard FDN 的 8 条延迟线长度
# 单位：sample
FDN_DELAYS = np.array(
    [37, 87, 181, 271, 359, 492, 687, 721],
    dtype=float,
)

# v2a 初版采用的平均结构损耗因子
#
# eta = alpha / (pi * f)
#
# 当前先使用确定性的平均目标，
# 暂时不加入真实音板中 mode-to-mode 的随机阻尼离散。
ETA0 = 0.023

# 当前旧版 Soundboard 的参考衰减时间：
#
# T60 ≈ 0.34 s
#
# 即振幅在 0.34 秒后下降到原来的 10^-3。
T60_BASELINE = 0.34

# 对应的连续时间振幅衰减率：
#
# A(t) = A0 * exp(-alpha * t)
#
# 当 t = T60 时：
#
# exp(-alpha * T60) = 10^-3
#
# 因此：
#
# alpha = ln(1000) / T60
ALPHA_FLOOR = math.log(1000.0) / T60_BASELINE

# 求低频阻尼 floor 与 eta = 0.023 主干相交的位置：
#
# alpha_floor = pi * eta0 * f
#
# 因此：
#
# f = alpha_floor / (pi * eta0)
F_CROSSOVER = ALPHA_FLOOR / (math.pi * ETA0)

# 1.2–1.8 kHz 附近用于描述辐射阻尼增强的目标衰减率
# 单位：s^-1
ALPHA_RADIATION = 130.0

#当前实验中，3 kHz 以上先固定 alpha，作为保守的工程外推。
F_CAP = 3000.0

ALPHA_CAP = math.pi * ETA0 * F_CAP


# ============================================================
# 2. 定义目标阻尼规律 alpha(f)
# ============================================================

def smoothstep(x):
    """
    三次 smoothstep：

        s(x) = 3x^2 - 2x^3

    性质：

        s(0) = 0
        s(1) = 1

    并且在两个端点处斜率都为 0。

    这里用它把两个不同的阻尼规律平滑连接起来，
    避免 alpha(f) 在频率轴上突然出现尖锐折点。
    """

    return 3.0 * x**2 - 2.0 * x**3


def alpha_target(frequency_hz):
    """
    定义 Soundboard Fancy / v2a 的第一版目标阻尼率 alpha(f)。

    输入：
        frequency_hz
        频率，单位 Hz

    输出：
        alpha
        连续时间振幅衰减率，单位 s^-1

    定义关系：

        A(t) = A0 * exp(-alpha * t)

    alpha 越大，振动衰减越快。

    注意：
    这仍然只是一个确定性的“平均阻尼目标”，
    当前还没有模拟真实音板不同模态之间的阻尼离散。
    """

    f = np.asarray(frequency_hz, dtype=float)

    alpha = np.empty_like(f)


    # --------------------------------------------------------
    # 区域 A：低频阻尼 floor
    # --------------------------------------------------------
    #
    # 在非常低的频率下，我们暂时不直接使用
    #
    #     alpha = pi * eta * f
    #
    # 因为这会使 alpha 随 f -> 0 而趋近于 0，
    # 从而得到非常长的模态寿命。
    #
    # Ege / Boutillon 的低阶模态还明显受到
    # rim、边界条件等附加损耗影响。
    #
    # 因此第一版先保守地保留旧模型的
    #
    #     T60 = 0.34 s
    #
    # 所对应的 alpha floor。
    # --------------------------------------------------------

    mask = f <= F_CROSSOVER

    alpha[mask] = ALPHA_FLOOR


    # --------------------------------------------------------
    # 区域 B：近似恒定结构损耗因子
    # --------------------------------------------------------
    #
    # 损耗因子定义：
    #
    #     eta = alpha / (pi * f)
    #
    # 因此：
    #
    #     alpha = pi * eta * f
    #
    # 当前采用：
    #
    #     eta = 0.023
    #
    # 即约 2.3%。
    # --------------------------------------------------------

    mask = (
        (f > F_CROSSOVER)
        & (f < 1200.0)
    )

    alpha[mask] = (
        math.pi
        * ETA0
        * f[mask]
    )


    # --------------------------------------------------------
    # 区域 C：1.2–1.5 kHz 辐射阻尼过渡区
    # --------------------------------------------------------
    #
    # 从 eta = 0.023 对应的结构阻尼主干，
    # 平滑过渡到：
    #
    #     alpha ≈ 130 s^-1
    #
    # 当前使用 smoothstep 做平滑混合。
    # --------------------------------------------------------

    mask = (
        (f >= 1200.0)
        & (f < 1500.0)
    )

    transition_f = f[mask]

    # 如果只按照 eta = 0.023，
    # 此处原本对应的 alpha。
    wood_alpha = (
        math.pi
        * ETA0
        * transition_f
    )

    # 将 1200–1500 Hz 映射到 [0, 1]
    x = (
        transition_f - 1200.0
    ) / 300.0

    s = smoothstep(x)

    # 从 wood_alpha 平滑过渡到 ALPHA_RADIATION
    alpha[mask] = (
        (1.0 - s) * wood_alpha
        + s * ALPHA_RADIATION
    )


    # --------------------------------------------------------
    # 区域 D：1.5–1.8 kHz 辐射阻尼平台
    # --------------------------------------------------------

    mask = (
        (f >= 1500.0)
        & (f < 1800.0)
    )

    alpha[mask] = ALPHA_RADIATION


    # --------------------------------------------------------
    # 区域 E：1.8–3 kHz
    #
    # 重新回到：
    #
    #     eta ≈ 0.023
    #
    # 的结构阻尼主干。
    #
    # 在 1800 Hz：
    #
    # pi * 0.023 * 1800 ≈ 130 s^-1
    #
    # 因此这里能够非常自然地重新接上。
    # --------------------------------------------------------

    mask = (
        (f >= 1800.0)
        & (f <= F_CAP)
    )

    alpha[mask] = (
        math.pi
        * ETA0
        * f[mask]
    )


    # --------------------------------------------------------
    # 区域 F：3 kHz 以上
    # --------------------------------------------------------
    #
    # 当前没有足够依据把 eta = 0.023
    # 直接外推到整个可听频段。
    #
    # 因此第一版暂时固定：
    #
    #     alpha = alpha(3000 Hz)
    #
    # 这是工程上的保守外推，
    # 不是对真实钢琴音板高频阻尼的测量结论。
    # --------------------------------------------------------

    mask = f > F_CAP

    alpha[mask] = ALPHA_CAP

    return alpha


# ============================================================
# 3. 由 alpha(f) 推导其他声学量
# ============================================================

def t60_from_alpha(alpha):
    """
    将连续时间衰减率 alpha 转换为 T60。

    定义：

        exp(-alpha * T60) = 10^-3

    因此：

        T60 = ln(1000) / alpha
    """

    return math.log(1000.0) / alpha


def eta_from_alpha(alpha, frequency_hz):
    """
    由 alpha 和频率计算模态损耗因子 eta：

        eta = alpha / (pi * f)
    """

    return alpha / (
        math.pi * frequency_hz
    )


# ============================================================
# 4. 将连续时间阻尼规律转换成 FDN 每条 branch 的目标衰减
# ============================================================

def branch_target_magnitude(
    frequency_hz,
    delay_samples,
):
    """
    计算某一条 FDN delay branch
    在一次 traversal 中应该具有的目标振幅响应。

    一条长度为 L 的 delay，
    对应的实际经过时间为：

        tau = L / Fs

    连续时间振幅衰减：

        A(t) = A0 * exp(-alpha * t)

    因此经过这条 branch 后：

        A_out / A_in
            = exp[-alpha(f) * L / Fs]

    所以目标 magnitude response：

        |H_i(f)|
            = exp[-alpha(f) * L_i / Fs]

    注意：

    delay 本身只改变 phase，
    这里计算的是与 delay 配套的 loss filter
    应该产生多少 attenuation。
    """

    alpha = alpha_target(
        frequency_hz
    )

    delay_seconds = (
        delay_samples / FS
    )

    magnitude = np.exp(
        -alpha * delay_seconds
    )

    return magnitude


def magnitude_to_db(magnitude):
    """
    将振幅比转换为 dB：

        dB = 20 log10(|H|)
    """

    return 20.0 * np.log10(
        magnitude
    )


# ============================================================
# 5. 建立输出目录和频率采样点
# ============================================================

OUTPUT_DIR = (
    Path(__file__).parent
    / "output"
)

OUTPUT_DIR.mkdir(
    exist_ok=True
)

# 使用对数频率采样。
#
# 这样低频区域不会因为线性采样而过于稀疏，
# 也更符合观察音频频率响应时的习惯。
frequencies = np.geomspace(
    20.0,
    FS / 2.0,
    2000,
)

# 计算整个频率轴上的目标阻尼数据
alpha = alpha_target(
    frequencies
)

t60 = t60_from_alpha(
    alpha
)

eta = eta_from_alpha(
    alpha,
    frequencies,
)


# ============================================================
# 6. 打印关键频率锚点
# ============================================================

# 这些点用于人工检查目标函数是否符合预期。
anchor_frequencies = np.array([
    100.0,
    200.0,
    F_CROSSOVER,
    300.0,
    500.0,
    1000.0,
    1200.0,
    1350.0,
    1500.0,
    1800.0,
    2000.0,
    2500.0,
    3000.0,
    5000.0,
])

anchor_alpha = alpha_target(
    anchor_frequencies
)

anchor_t60 = t60_from_alpha(
    anchor_alpha
)

anchor_eta = eta_from_alpha(
    anchor_alpha,
    anchor_frequencies,
)


print()

print(
    "Soundboard Fancy — v2a 阻尼目标"
)

print(
    "================================"
)

print(
    f"采样率 Fs              = {FS:.1f} Hz"
)

print(
    f"结构损耗因子 eta0       = {ETA0:.6f}"
)

print(
    f"低频 alpha floor       = {ALPHA_FLOOR:.6f} s^-1"
)

print(
    f"低频交点               = {F_CROSSOVER:.6f} Hz"
)

print(
    f"高频 alpha cap         = {ALPHA_CAP:.6f} s^-1"
)

print()

print(
    f"{'频率 (Hz)':>12}"
    f"{'alpha (1/s)':>16}"
    f"{'eta (%)':>14}"
    f"{'T60 (s)':>14}"
)

for f, a, e, t in zip(
    anchor_frequencies,
    anchor_alpha,
    anchor_eta,
    anchor_t60,
):
    print(
        f"{f:12.3f}"
        f"{a:16.6f}"
        f"{100.0 * e:14.4f}"
        f"{t:14.6f}"
    )


# ============================================================
# 7. 计算并导出 8 条 FDN branch 的目标响应
# ============================================================

branch_magnitudes = {}

for delay in FDN_DELAYS.astype(int):

    branch_magnitudes[delay] = (
        branch_target_magnitude(
            frequencies,
            delay,
        )
    )


csv_path = (
    OUTPUT_DIR
    / "branch_targets.csv"
)

with csv_path.open(
    "w",
    newline="",
) as file:

    writer = csv.writer(file)

    # CSV 前四列保存共同的物理目标
    header = [
        "frequency_hz",
        "alpha_per_second",
        "eta",
        "t60_seconds",
    ]

    # 后续每条 delay 保存：
    #
    # 1. 线性 magnitude
    # 2. dB attenuation
    for delay in FDN_DELAYS.astype(int):

        header += [
            f"delay_{delay}_magnitude",
            f"delay_{delay}_db",
        ]

    writer.writerow(
        header
    )

    for index, frequency in enumerate(
        frequencies
    ):

        row = [
            frequency,
            alpha[index],
            eta[index],
            t60[index],
        ]

        for delay in FDN_DELAYS.astype(int):

            magnitude = (
                branch_magnitudes[
                    delay
                ][index]
            )

            row += [
                magnitude,
                magnitude_to_db(
                    magnitude
                ),
            ]

        writer.writerow(
            row
        )


# ============================================================
# 8. 绘制 alpha(f)
# ============================================================

plt.figure(
    figsize=(9, 5)
)

plt.semilogx(
    frequencies,
    alpha,
)

plt.xlabel(
    "Frequency (Hz)"
)

plt.ylabel(
    "Damping rate alpha (1/s)"
)

plt.title(
    "Soundboard Fancy — v2a target alpha(f)"
)

plt.grid(
    True,
    which="both",
    alpha=0.3,
)

plt.tight_layout()

plt.savefig(
    OUTPUT_DIR
    / "target_alpha.png",
    dpi=180,
)

plt.close()


# ============================================================
# 9. 绘制 T60(f)
# ============================================================

plt.figure(
    figsize=(9, 5)
)

plt.semilogx(
    frequencies,
    t60,
)

plt.xlabel(
    "Frequency (Hz)"
)

plt.ylabel(
    "T60 (s)"
)

plt.title(
    "Soundboard Fancy — v2a target T60(f)"
)

plt.grid(
    True,
    which="both",
    alpha=0.3,
)

plt.tight_layout()

plt.savefig(
    OUTPUT_DIR
    / "target_t60.png",
    dpi=180,
)

plt.close()


# ============================================================
# 10. 绘制 8 条 FDN branch 的目标 attenuation
# ============================================================

plt.figure(
    figsize=(10, 6)
)

for delay in FDN_DELAYS.astype(int):

    magnitude = (
        branch_magnitudes[
            delay
        ]
    )

    plt.semilogx(
        frequencies,
        magnitude_to_db(
            magnitude
        ),
        label=f"L = {delay}",
    )

plt.xlabel(
    "Frequency (Hz)"
)

plt.ylabel(
    "Attenuation per traversal (dB)"
)

plt.title(
    "Soundboard Fancy — v2a FDN branch targets"
)

plt.grid(
    True,
    which="both",
    alpha=0.3,
)

plt.legend()

plt.tight_layout()

plt.savefig(
    OUTPUT_DIR
    / "branch_targets.png",
    dpi=180,
)

plt.close()


# ============================================================
# 11. 输出生成结果
# ============================================================

print()

print(
    f"已生成：{csv_path}"
)

print(
    "已生成："
    f"{OUTPUT_DIR / 'target_alpha.png'}"
)

print(
    "已生成："
    f"{OUTPUT_DIR / 'target_t60.png'}"
)

print(
    "已生成："
    f"{OUTPUT_DIR / 'branch_targets.png'}"
)
