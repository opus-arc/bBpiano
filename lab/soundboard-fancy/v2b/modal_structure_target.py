#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
bBpiano Soundboard Fancy — modal_structure_target.py
====================================================

用途
----
这个文件只定义“我们希望 Soundboard 的 modal structure 长成什么样”。
它不修改 production，不搜索 delay，不优化 feedback matrix，也不负责 damping。

它与 target_damping.py 的关系是：

    target_damping.py
        └─ 定义 pole radius / decay：
           alpha(f), T60(f), eta(f)

    modal_structure_target.py
        └─ 定义 pole angle / modal statistics：
           modal density n(f)
           nearest-neighbor spacing distribution
           spacing CV
           localization transition

后续 search_fdn_structure.py 应该：
    candidate delays / feedback matrix
            ↓
        exact poles
            ↓
        modal frequencies f_k
            ↓
        本文件提供 target / metrics / objective

当前版本刻意“保守”：
1. 低频 global-plate regime（约 100–1100 Hz）使用文献支持较明确的
   apparent modal-density reference ≈ 0.06 modes/Hz。
2. 约 1.1 kHz 以上进入 rib-induced localization / waveguide regime 后，
   apparent modal density 与观察位置有关，因此本文件“不伪造一条高频 target 曲线”。
   高频 density 默认不进入 objective；等我们把位置相关 mobility / waveguide
   文献目标整理清楚后，再补 v2。
3. nearest-neighbor spacing 采用 Boutillon/Ege reduced model 中的
   normalized Rayleigh spacing law：
       p(x) = (pi/2) x exp(-pi x^2 / 4), x >= 0
   其中 x = Δf / mean(Δf)，其理论 CV = sqrt(4/pi - 1) ≈ 0.5227。

参考文献
--------
[1] Ege, K., Boutillon, X., Rébillat, M.
    Vibroacoustics of the piano soundboard:
    Nonlinearity and modal properties in the low- and mid-frequency ranges.
    Journal of Sound and Vibration, 332(5), 1288–1305, 2013.
    DOI: 10.1016/j.jsv.2012.10.012

[2] Boutillon, X., Ege, K.
    Vibroacoustics of the piano soundboard:
    Reduced models, mobility synthesis, and acoustical radiation regime.
    Journal of Sound and Vibration, 332(18), 4261–4279, 2013.
    DOI: 10.1016/j.jsv.2013.03.015

重要
----
这里的 target 是“统计目标”，不是逐个指定每一个真实 mode 的频率。
第一版 v2b 的目标不是复刻一块具体钢琴音板的 FEM/实验模态表，而是先让
8-delay FDN 摆脱目前过度规则的 modal spacing，同时保持合理 modal density。
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import math

import numpy as np


# ============================================================================
# 1. 基本配置
# ============================================================================

FS = 44_100.0

# 我们当前只对这个频带做第一版 modal-structure calibration。
# 低于 100 Hz 的 mode 数太少，统计量不稳定；
# 高于 3 kHz 后当前单一 global FDN 本身就未必是正确架构。
ANALYSIS_LOW_HZ = 100.0
ANALYSIS_HIGH_HZ = 3_000.0

# 文献中约 1.1 kHz 附近开始进入 ribs 导致的 localization / waveguide regime。
LOCALIZATION_TRANSITION_HZ = 1_100.0

# 低频 global-plate regime 的 apparent modal density reference。
LOW_FREQ_MODAL_DENSITY_MODES_PER_HZ = 0.06

# 为避免 optimizer 在 1.1 kHz 边界处出现硬切换，
# density objective 的权重在 transition 附近做一个小的平滑衰减。
TRANSITION_FADE_WIDTH_HZ = 150.0

# modal density 的估计方法保持和论文/我们当前 validate_fdn_poles.py 一致：
# 6 个 successive modal spacings 的 moving average 的倒数。
DENSITY_SPACING_WINDOW = 6

# normalized Rayleigh spacing 的理论 CV。
RAYLEIGH_SPACING_CV = math.sqrt(4.0 / math.pi - 1.0)


@dataclass(frozen=True)
class ModalStructureTarget:
    analysis_low_hz: float = ANALYSIS_LOW_HZ
    analysis_high_hz: float = ANALYSIS_HIGH_HZ
    localization_transition_hz: float = LOCALIZATION_TRANSITION_HZ
    transition_fade_width_hz: float = TRANSITION_FADE_WIDTH_HZ
    low_freq_modal_density: float = LOW_FREQ_MODAL_DENSITY_MODES_PER_HZ
    density_spacing_window: int = DENSITY_SPACING_WINDOW
    rayleigh_spacing_cv: float = RAYLEIGH_SPACING_CV


DEFAULT_TARGET = ModalStructureTarget()


# ============================================================================
# 2. Modal-density target
# ============================================================================

def density_target_weight(
    frequency_hz: np.ndarray | float,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> np.ndarray:
    """
    返回 modal-density target 的可信权重。

    100 Hz 以下：
        0，不纳入第一版统计优化。

    100 Hz 到 transition-fade：
        1，使用 n(f) ≈ 0.06 modes/Hz。

    transition 附近：
        smoothstep 从 1 降到 0。

    transition 以上：
        0。
        原因不是“高频 density 不重要”，而是它在 localized regime 中具有
        位置依赖性；当前没有足够依据用一条全局 n(f) 曲线硬约束。
    """
    f = np.asarray(frequency_hz, dtype=float)
    weight = np.zeros_like(f)

    start = target.analysis_low_hz
    transition = target.localization_transition_hz
    fade = target.transition_fade_width_hz
    fade_start = max(start, transition - fade)

    full = (f >= start) & (f <= fade_start)
    weight[full] = 1.0

    taper = (f > fade_start) & (f < transition)
    if np.any(taper):
        x = (f[taper] - fade_start) / (transition - fade_start)
        smooth = 3.0 * x**2 - 2.0 * x**3
        weight[taper] = 1.0 - smooth

    return weight


def target_modal_density(
    frequency_hz: np.ndarray | float,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> np.ndarray:
    """
    第一版 target modal density。

    在可信的低频 global-plate regime：
        n_target(f) = 0.06 modes/Hz

    transition 以上返回 NaN，明确表示“当前版本不定义 target”，
    防止后续 optimizer 误以为 0.06 应该从低频一路维持到 Nyquist。
    """
    f = np.asarray(frequency_hz, dtype=float)
    result = np.full_like(f, np.nan)

    valid = (
        (f >= target.analysis_low_hz)
        & (f < target.localization_transition_hz)
    )
    result[valid] = target.low_freq_modal_density
    return result


# ============================================================================
# 3. Nearest-neighbor spacing target
# ============================================================================

def rayleigh_spacing_pdf(x: np.ndarray | float) -> np.ndarray:
    """
    Unit-mean normalized Rayleigh modal-spacing PDF：

        p(x) = (pi/2) x exp(-pi x^2 / 4),  x >= 0

    x = Δf / mean(Δf)
    """
    x = np.asarray(x, dtype=float)
    result = np.zeros_like(x)
    valid = x >= 0.0
    xv = x[valid]
    result[valid] = 0.5 * math.pi * xv * np.exp(-0.25 * math.pi * xv**2)
    return result


def rayleigh_spacing_cdf(x: np.ndarray | float) -> np.ndarray:
    """
    与 rayleigh_spacing_pdf 对应的 CDF：

        F(x) = 1 - exp(-pi x^2 / 4), x >= 0
    """
    x = np.asarray(x, dtype=float)
    result = np.zeros_like(x)
    valid = x >= 0.0
    xv = x[valid]
    result[valid] = 1.0 - np.exp(-0.25 * math.pi * xv**2)
    return result


def rayleigh_spacing_quantile(p: np.ndarray | float) -> np.ndarray:
    """
    Rayleigh target quantile function：

        Q(p) = sqrt(-4 ln(1-p) / pi)
    """
    p = np.asarray(p, dtype=float)
    if np.any((p < 0.0) | (p >= 1.0)):
        raise ValueError("p 必须满足 0 <= p < 1")
    return np.sqrt(-4.0 * np.log1p(-p) / math.pi)


# ============================================================================
# 4. 从候选 modal frequencies 提取统计量
# ============================================================================

def select_analysis_frequencies(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> np.ndarray:
    f = np.sort(np.asarray(modal_frequencies_hz, dtype=float))
    mask = (
        np.isfinite(f)
        & (f >= target.analysis_low_hz)
        & (f <= target.analysis_high_hz)
    )
    return f[mask]


def nearest_neighbor_spacings(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> np.ndarray:
    """
    返回 analysis band 内相邻 modal frequencies 的 spacing（Hz）。
    """
    f = select_analysis_frequencies(modal_frequencies_hz, target)
    if len(f) < 2:
        return np.array([], dtype=float)
    return np.diff(f)


def normalized_spacings(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> np.ndarray:
    """
    Δf / mean(Δf)
    """
    spacing = nearest_neighbor_spacings(modal_frequencies_hz, target)
    if len(spacing) == 0:
        return spacing

    mean_spacing = float(np.mean(spacing))
    if not np.isfinite(mean_spacing) or mean_spacing <= 0.0:
        return np.array([], dtype=float)

    return spacing / mean_spacing


def spacing_cv(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> float:
    """
    CV = std(Δf) / mean(Δf)
    """
    spacing = nearest_neighbor_spacings(modal_frequencies_hz, target)
    if len(spacing) < 2:
        return math.nan

    mean_spacing = float(np.mean(spacing))
    if mean_spacing <= 0.0:
        return math.nan

    return float(np.std(spacing) / mean_spacing)


def apparent_modal_density(
    modal_frequencies_hz: np.ndarray,
    spacing_window: int = DENSITY_SPACING_WINDOW,
) -> tuple[np.ndarray, np.ndarray]:
    """
    使用 6 successive spacings moving-average 的倒数估计 apparent modal density。

    对排序后的频率 f_k：

        Δf_k = f_{k+1} - f_k

        <Δf>_6(k) = mean(Δf_k ... Δf_{k+5})

        n(k) = 1 / <Δf>_6(k)

    横坐标用该窗口覆盖的 7 个 mode 的首尾中点。
    """
    f = np.sort(np.asarray(modal_frequencies_hz, dtype=float))
    f = f[np.isfinite(f)]

    if len(f) < spacing_window + 1:
        return np.array([], dtype=float), np.array([], dtype=float)

    spacing = np.diff(f)
    kernel = np.ones(spacing_window, dtype=float) / spacing_window
    mean_spacing = np.convolve(spacing, kernel, mode="valid")

    center_frequency = 0.5 * (
        f[:-spacing_window] + f[spacing_window:]
    )

    density = np.full_like(mean_spacing, np.nan)
    valid = mean_spacing > 0.0
    density[valid] = 1.0 / mean_spacing[valid]

    return center_frequency, density


# ============================================================================
# 5. Objective metrics
# ============================================================================

def density_rmse(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> float:
    """
    低频可信区间的 weighted modal-density RMSE。

    注意：
    transition 以上权重为 0，所以不会错误地惩罚高频 localized regime。
    """
    center, density = apparent_modal_density(
        modal_frequencies_hz,
        target.density_spacing_window,
    )
    if len(center) == 0:
        return math.inf

    wanted = target_modal_density(center, target)
    weight = density_target_weight(center, target)

    valid = (
        np.isfinite(density)
        & np.isfinite(wanted)
        & (weight > 0.0)
    )
    if not np.any(valid):
        return math.inf

    error = density[valid] - wanted[valid]
    w = weight[valid]

    return float(np.sqrt(np.sum(w * error**2) / np.sum(w)))


def spacing_cv_error(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> float:
    """
    第一版很便宜的 spacing-statistic objective：

        |CV_FDN - CV_Rayleigh|

    它只约束分布宽度，不足以完全约束 distribution shape；
    后续还应结合 spacing_cdf_distance()。
    """
    cv = spacing_cv(modal_frequencies_hz, target)
    if not np.isfinite(cv):
        return math.inf
    return abs(cv - target.rayleigh_spacing_cv)


def spacing_cdf_distance(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> float:
    """
    normalized spacing 的经验 CDF 与 Rayleigh target CDF 的 KS-like 距离。

    不依赖 scipy，便于 search loop 高频调用。
    """
    x = np.sort(normalized_spacings(modal_frequencies_hz, target))
    if len(x) < 2:
        return math.inf

    empirical = np.arange(1, len(x) + 1, dtype=float) / len(x)
    desired = rayleigh_spacing_cdf(x)
    return float(np.max(np.abs(empirical - desired)))


def spacing_quantile_rmse(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> float:
    """
    用 quantile matching 衡量整个 spacing distribution，而不仅仅看 CV。

    与 histogram loss 相比，它不依赖 bin 选择，后续优化更稳定。
    """
    x = np.sort(normalized_spacings(modal_frequencies_hz, target))
    if len(x) < 2:
        return math.inf

    p = (np.arange(len(x), dtype=float) + 0.5) / len(x)
    desired = rayleigh_spacing_quantile(p)
    return float(np.sqrt(np.mean((x - desired) ** 2)))


def objective_components(
    modal_frequencies_hz: np.ndarray,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> dict[str, float]:
    """
    给 search_fdn_structure.py 用的原始 objective components。

    这里故意不直接加权成一个总 score。
    权重属于 optimizer/search policy，而不是物理 target 本身。

    建议后续至少同时观察：
        density_rmse
        spacing_cv_error
        spacing_cdf_distance
        spacing_quantile_rmse

    不要只优化 CV。
    """
    f = select_analysis_frequencies(modal_frequencies_hz, target)
    spacing = nearest_neighbor_spacings(modal_frequencies_hz, target)

    return {
        "mode_count": float(len(f)),
        "mean_spacing_hz": (
            float(np.mean(spacing)) if len(spacing) else math.nan
        ),
        "spacing_cv": spacing_cv(modal_frequencies_hz, target),
        "target_spacing_cv": target.rayleigh_spacing_cv,
        "density_rmse_modes_per_hz": density_rmse(
            modal_frequencies_hz, target
        ),
        "spacing_cv_error": spacing_cv_error(
            modal_frequencies_hz, target
        ),
        "spacing_cdf_distance": spacing_cdf_distance(
            modal_frequencies_hz, target
        ),
        "spacing_quantile_rmse": spacing_quantile_rmse(
            modal_frequencies_hz, target
        ),
    }


# ============================================================================
# 6. Preview / self-check
# ============================================================================

def print_summary(
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> None:
    print("Soundboard modal-structure target v1")
    print("=" * 60)
    print(
        f"analysis band               : "
        f"{target.analysis_low_hz:.0f}–{target.analysis_high_hz:.0f} Hz"
    )
    print(
        f"localization transition     : "
        f"~{target.localization_transition_hz:.0f} Hz"
    )
    print(
        f"low-frequency modal density : "
        f"{target.low_freq_modal_density:.3f} modes/Hz"
    )
    print(
        f"density window              : "
        f"{target.density_spacing_window} successive spacings"
    )
    print(
        f"Rayleigh spacing CV         : "
        f"{target.rayleigh_spacing_cv:.6f}"
    )
    print("")
    print(
        "NOTE: >1.1 kHz modal density is intentionally undefined in v1; "
        "do not extrapolate 0.06 modes/Hz into the localized regime."
    )


def make_preview_plot(
    output_dir: Path,
    target: ModalStructureTarget = DEFAULT_TARGET,
) -> None:
    """
    只画 target 本身，不需要 candidate FDN。
    """
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    output_dir.mkdir(parents=True, exist_ok=True)

    # Figure 1: density target +可信权重
    frequency = np.geomspace(
        max(50.0, target.analysis_low_hz / 2.0),
        target.analysis_high_hz,
        800,
    )
    density = target_modal_density(frequency, target)
    weight = density_target_weight(frequency, target)

    fig, ax = plt.subplots(figsize=(10.5, 5.8))
    ax.plot(
        frequency,
        density,
        linewidth=2.0,
        label="defined modal-density target",
    )
    ax.axvline(
        target.localization_transition_hz,
        linestyle="--",
        linewidth=1.2,
        label="~1.1 kHz localization transition",
    )
    ax.fill_between(
        frequency,
        0.0,
        target.low_freq_modal_density * weight,
        alpha=0.15,
        label="density-objective confidence weight",
    )
    ax.set_xscale("log")
    ax.set_xlim(frequency[0], frequency[-1])
    ax.set_ylim(0.0, 0.08)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Apparent modal density (modes/Hz)")
    ax.set_title("Soundboard modal-density target v1")
    ax.grid(True, which="both", alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(output_dir / "modal_density_target.png", dpi=180)
    plt.close(fig)

    # Figure 2: spacing PDF
    x = np.linspace(0.0, 2.5, 800)
    fig, ax = plt.subplots(figsize=(10.5, 5.8))
    ax.plot(
        x,
        rayleigh_spacing_pdf(x),
        linewidth=2.0,
        label=(
            "unit-mean Rayleigh spacing target "
            f"(CV={target.rayleigh_spacing_cv:.3f})"
        ),
    )
    ax.set_xlabel("Nearest-neighbor spacing / mean spacing")
    ax.set_ylabel("Probability density")
    ax.set_title("Normalized modal-spacing target")
    ax.grid(True, alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(output_dir / "spacing_target.png", dpi=180)
    plt.close(fig)


if __name__ == "__main__":
    print_summary()

    # 直接运行本文件时只生成两张 target preview，
    # 不分析 production，不搜索任何参数。
    output = (
        Path(__file__).resolve().parent
        / "output"
        / "modal_structure_target"
    )
    make_preview_plot(output)
    print(f"\npreview written to: {output}")
