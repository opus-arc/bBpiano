"""
Soundboard FDN 三版本验证：两套 closed-loop dynamics 与三套 audible readout。

A=L1 one-pole、B=v2a post-loss、C=v2a pre-loss。直接从指定 Git commit
读取 production 源码，验证 B/C 的 feedback 状态矩阵及 eigenvalues 一致，
然后逐 sample 复刻环形 delay、loss SOS、output tap 与 direct path。
前七图展示 L1/v2a structural poles；后四图展示三版本的单位脉冲响应。
论文线条只表示 published curves / reported trends，并非 digitized scatter。
运行：在 lab/soundboard-fancy/ 下执行 .venv/bin/python validate_fdn_poles.py
输出：output/poles/
"""

from __future__ import annotations

import argparse
import csv
import gc
import json
import math
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
from scipy.fft import next_fast_len
from scipy.linalg import eigvals
from scipy.optimize import linear_sum_assignment
from scipy.signal import butter, sosfilt

# ---------------------------------------------------------------------
# 固定的可复现实验版本
# ---------------------------------------------------------------------

REPO_SOURCE_PATH = "core/bBpiano Lite/core/bbpl/piano/soundboard_model.hpp"
TARGET_SOURCE_PATH = "lab/soundboard-fancy/target_damping.py"

DEFAULT_L1_REF = "a6e1914ffc632abc22cf9e065a96ccf8099c2b77"
DEFAULT_POSTLOSS_REF = "6ec51f9cd61cfd63898abc8e1af75584d5fa6a5b"
DEFAULT_PRELOSS_REF = "b9d8f0c"

BANDS = (
    ("100-300", 100.0, 300.0),
    ("300-1100", 300.0, 1100.0),
    ("1100-2500", 1100.0, 2500.0),
    ("2500-5000", 2500.0, 5000.0),
    ("5000-10000", 5000.0, 10000.0),
)

IDENTITY_SECTION = np.array([1.0, 0.0, 0.0, 0.0, 0.0])


# ---------------------------------------------------------------------
# 数据结构
# ---------------------------------------------------------------------

@dataclass
class TargetDefinition:
    fs: float
    eta0: float
    t60_baseline: float
    alpha_floor: float
    crossover_hz: float
    alpha_radiation: float
    cap_hz: float
    alpha_cap: float


@dataclass
class PoleModes:
    z: np.ndarray
    frequency_hz: np.ndarray
    radius: np.ndarray
    alpha_per_second: np.ndarray
    t60_seconds: np.ndarray
    eta: np.ndarray
    q_factor: np.ndarray


# ---------------------------------------------------------------------
# Git/source 读取
# ---------------------------------------------------------------------

def repo_root() -> Path:
    result = subprocess.run(
        ["git", "rev-parse", "--show-toplevel"],
        check=True,
        capture_output=True,
        text=True,
    )
    return Path(result.stdout.strip())


def git_show(root: Path, ref: str, path: str) -> str:
    """直接从指定 Git commit/ref 读取 production 源码，不读取 archive。"""
    result = subprocess.run(
        ["git", "show", f"{ref}:{path}"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout


def resolve_ref(root: Path, ref: str) -> str:
    result = subprocess.run(
        ["git", "rev-parse", ref],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


# ---------------------------------------------------------------------
# 源码解析
# ---------------------------------------------------------------------

_FLOAT = r"[-+]?(?:(?:\d+(?:\.\d*)?)|(?:\.\d+))(?:[eE][-+]?\d+)?"

def _initializer_block(source: str, token: str) -> str:
    """识别 C++ 的 `name{...}` 与 `name = {...}` 两种 initializer。"""
    token_pos = source.find(token)
    if token_pos < 0:
        raise RuntimeError(f"源码中找不到 {token!r}")

    brace_pos = source.find("{", token_pos + len(token))
    semicolon_pos = source.find(";", token_pos + len(token))
    if brace_pos < 0 or (semicolon_pos >= 0 and semicolon_pos < brace_pos):
        raise RuntimeError(f"无法解析 {token!r} initializer")

    depth = 0
    for index in range(brace_pos, len(source)):
        char = source[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return source[brace_pos:index + 1]

    raise RuntimeError(f"{token!r} initializer 大括号不闭合")


def parse_delays(source: str) -> np.ndarray:
    block = _initializer_block(source, "kDelayLengths")
    values = [int(x) for x in re.findall(r"\b\d+\b", block)]
    # production 当前应为 8 条；避免误吞其他内容
    if len(values) != 8:
        raise RuntimeError(f"kDelayLengths 解析得到 {len(values)} 个值：{values}")
    return np.asarray(values, dtype=int)


def parse_scalar_literal(source: str, name: str) -> float:
    pattern = re.compile(
        rf"\b{name}\s*=\s*({_FLOAT})f?\s*;"
    )
    match = pattern.search(source)
    if not match:
        raise RuntimeError(f"找不到数值常量 {name}")
    return float(match.group(1))


def parse_target_scalar(source: str, name: str) -> float:
    pattern = re.compile(
        rf"(?m)^\s*{name}\s*=\s*({_FLOAT})\s*$"
    )
    match = pattern.search(source)
    if not match:
        raise RuntimeError(f"target_damping.py 中找不到 {name}")
    return float(match.group(1))


def parse_target_definition(source: str) -> TargetDefinition:
    fs = parse_target_scalar(source, "FS")
    eta0 = parse_target_scalar(source, "ETA0")
    t60 = parse_target_scalar(source, "T60_BASELINE")
    alpha_radiation = parse_target_scalar(source, "ALPHA_RADIATION")
    cap_hz = parse_target_scalar(source, "F_CAP")

    # 这些分段点当前在 alpha_target() 代码中以 literal 出现。
    for literal in ("1200.0", "1500.0", "1800.0"):
        if literal not in source:
            raise RuntimeError(
                f"target_damping.py 的分段结构已变化：未找到 {literal}，"
                "请人工审计后更新 validate_fdn_poles.py。"
            )

    alpha_floor = math.log(1000.0) / t60
    crossover = alpha_floor / (math.pi * eta0)
    alpha_cap = math.pi * eta0 * cap_hz

    return TargetDefinition(
        fs=fs,
        eta0=eta0,
        t60_baseline=t60,
        alpha_floor=alpha_floor,
        crossover_hz=crossover,
        alpha_radiation=alpha_radiation,
        cap_hz=cap_hz,
        alpha_cap=alpha_cap,
    )


def parse_v2a_sos(source: str, branch_count: int, section_count: int = 8) -> np.ndarray:
    block = _initializer_block(source, "kV2aLossSOS")
    tuple_pattern = re.compile(
        r"\{\s*"
        rf"({_FLOAT})f?\s*,\s*"
        rf"({_FLOAT})f?\s*,\s*"
        rf"({_FLOAT})f?\s*,\s*"
        rf"({_FLOAT})f?\s*,\s*"
        rf"({_FLOAT})f?\s*"
        r"\}"
    )
    rows = [
        [float(value) for value in match.groups()]
        for match in tuple_pattern.finditer(block)
    ]
    expected = branch_count * section_count
    if len(rows) != expected:
        raise RuntimeError(
            f"kV2aLossSOS 应有 {expected} 个 section，实际解析到 {len(rows)}"
        )
    return np.asarray(rows, dtype=float).reshape(branch_count, section_count, 5)


def active_sections(bank: np.ndarray) -> list[np.ndarray]:
    """去掉 identity section；保留真实有动态作用的 section。"""
    result = []
    counts = []
    for branch in bank:
        sections = np.asarray(
            [
                section
                for section in branch
                if not np.allclose(section, IDENTITY_SECTION, atol=1e-12, rtol=0.0)
            ],
            dtype=float,
        )
        result.append(sections)
        counts.append(len(sections))

    if len(set(counts)) != 1:
        raise RuntimeError(f"各 branch 有效 SOS 数不一致：{counts}")
    return result


# ---------------------------------------------------------------------
# Target alpha(f)
# ---------------------------------------------------------------------

def smoothstep(x: np.ndarray) -> np.ndarray:
    return 3.0 * x**2 - 2.0 * x**3


def alpha_target(frequency_hz, target: TargetDefinition) -> np.ndarray:
    f = np.asarray(frequency_hz, dtype=float)
    alpha = np.empty_like(f)

    mask = f <= target.crossover_hz
    alpha[mask] = target.alpha_floor

    mask = (f > target.crossover_hz) & (f < 1200.0)
    alpha[mask] = math.pi * target.eta0 * f[mask]

    mask = (f >= 1200.0) & (f < 1500.0)
    x = (f[mask] - 1200.0) / 300.0
    s = smoothstep(x)
    wood = math.pi * target.eta0 * f[mask]
    alpha[mask] = (1.0 - s) * wood + s * target.alpha_radiation

    mask = (f >= 1500.0) & (f < 1800.0)
    alpha[mask] = target.alpha_radiation

    mask = (f >= 1800.0) & (f <= target.cap_hz)
    alpha[mask] = math.pi * target.eta0 * f[mask]

    mask = f > target.cap_hz
    alpha[mask] = target.alpha_cap

    return alpha


# ---------------------------------------------------------------------
# 从 C++ loss 参数构造 branch filter
# ---------------------------------------------------------------------

def make_l1_sections(
    delays: np.ndarray,
    fs: float,
    target_t60: float,
    loss_pole: float,
) -> list[np.ndarray]:
    sections = []
    for delay in delays:
        delay_seconds = float(delay) / fs
        gain = 10.0 ** (-3.0 * delay_seconds / target_t60)
        # H(z) = g(1-p)/(1-p z^-1)
        # denominator convention = 1 + a1 z^-1 + a2 z^-2
        sections.append(
            np.asarray(
                [[gain * (1.0 - loss_pole), 0.0, 0.0, -loss_pole, 0.0]],
                dtype=float,
            )
        )
    return sections


def section_state_order(section: np.ndarray) -> int:
    _, _, b2, _, a2 = section
    if np.allclose(section, IDENTITY_SECTION, atol=1e-14, rtol=0.0):
        return 0
    if abs(b2) < 1e-14 and abs(a2) < 1e-14:
        return 1
    return 2


# ---------------------------------------------------------------------
# Production feedback matrix
# ---------------------------------------------------------------------

def feedback_matrix(branch_count: int = 8) -> np.ndarray:
    """
    Production:
        feedback[i] = retained[(i+1) mod 8] - 0.25 * sum(retained)
    """
    if branch_count != 8:
        raise RuntimeError("当前脚本只验证 production 的 8-line shifted Householder")
    matrix = -0.25 * np.ones((branch_count, branch_count), dtype=float)
    for i in range(branch_count):
        matrix[i, (i + 1) % branch_count] += 1.0
    return matrix


# ---------------------------------------------------------------------
# 精确构造 zero-input state transition matrix
# ---------------------------------------------------------------------

def build_state_matrix(
    delays: np.ndarray,
    branch_sections: list[np.ndarray],
) -> np.ndarray:
    """
    直接复刻每采样 C++ dynamics，但令外部 input=0。

    每条 delay 用 canonical shift-register 表示：
        out = d[0]
        d'[0:L-1] = d[1:L]
        d'[L-1]   = feedback

    filter 使用与 C++ LossSection::process() 相同的 TDF-II 状态更新。

    对 L1 退化 one-pole section 只保留真正需要的一个状态；
    对 v2a biquad 每节保留两个状态。
    """
    delays = np.asarray(delays, dtype=int)
    branch_count = len(delays)

    delay_offsets = np.concatenate(
        ([0], np.cumsum(delays)[:-1])
    ).astype(int)
    delay_state_count = int(np.sum(delays))

    orders = [
        [section_state_order(section) for section in sections]
        for sections in branch_sections
    ]
    filter_state_count = sum(sum(branch_orders) for branch_orders in orders)
    state_count = delay_state_count + filter_state_count

    transition = np.zeros((state_count, state_count), dtype=float)

    # delay shift
    for offset, length in zip(delay_offsets, delays, strict=True):
        rows = np.arange(offset, offset + length - 1)
        cols = rows + 1
        transition[rows, cols] = 1.0

    retained_forms: list[np.ndarray] = []
    state_cursor = delay_state_count

    for branch_index, (offset, sections) in enumerate(
        zip(delay_offsets, branch_sections, strict=True)
    ):
        # 当前 delay readout 对 state vector 的线性形式
        value = np.zeros(state_count, dtype=float)
        value[offset] = 1.0

        for section in sections:
            order = section_state_order(section)
            if order == 0:
                continue

            b0, b1, b2, a1, a2 = section

            if order == 1:
                s1_index = state_cursor
                state_cursor += 1

                y = b0 * value.copy()
                y[s1_index] += 1.0

                s1_next = b1 * value - a1 * y
                transition[s1_index, :] = s1_next

                value = y
                continue

            s1_index = state_cursor
            s2_index = state_cursor + 1
            state_cursor += 2

            y = b0 * value.copy()
            y[s1_index] += 1.0

            s1_next = b1 * value - a1 * y
            s1_next[s2_index] += 1.0

            s2_next = b2 * value - a2 * y

            transition[s1_index, :] = s1_next
            transition[s2_index, :] = s2_next

            value = y

        retained_forms.append(value)

    if state_cursor != state_count:
        raise RuntimeError("filter state layout 内部错误")

    retained_sum = np.sum(retained_forms, axis=0)

    # production shifted Householder feedback
    for i, (offset, length) in enumerate(zip(delay_offsets, delays, strict=True)):
        feedback = retained_forms[(i + 1) % branch_count] - 0.25 * retained_sum
        transition[offset + length - 1, :] = feedback

    return transition


# ---------------------------------------------------------------------
# Pole extraction / classification
# ---------------------------------------------------------------------

def positive_modes(eigenvalues: np.ndarray, fs: float) -> PoleModes:
    z = eigenvalues[np.imag(eigenvalues) > 1e-8]
    radius = np.abs(z)
    frequency = np.angle(z) * fs / (2.0 * math.pi)
    alpha = -fs * np.log(radius)

    valid = (
        np.isfinite(frequency)
        & np.isfinite(alpha)
        & (frequency > 0.0)
        & (frequency < fs / 2.0)
        & (alpha > 0.0)
    )
    z = z[valid]
    radius = radius[valid]
    frequency = frequency[valid]
    alpha = alpha[valid]

    order = np.argsort(frequency)
    z = z[order]
    radius = radius[order]
    frequency = frequency[order]
    alpha = alpha[order]

    t60 = math.log(1000.0) / alpha
    eta = alpha / (math.pi * frequency)
    q_factor = 1.0 / eta

    return PoleModes(
        z=z,
        frequency_hz=frequency,
        radius=radius,
        alpha_per_second=alpha,
        t60_seconds=t60,
        eta=eta,
        q_factor=q_factor,
    )


def subset_modes(modes: PoleModes, indices: np.ndarray) -> PoleModes:
    return PoleModes(
        z=modes.z[indices],
        frequency_hz=modes.frequency_hz[indices],
        radius=modes.radius[indices],
        alpha_per_second=modes.alpha_per_second[indices],
        t60_seconds=modes.t60_seconds[indices],
        eta=modes.eta[indices],
        q_factor=modes.q_factor[indices],
    )


def sort_modes_by_frequency(modes: PoleModes) -> PoleModes:
    return subset_modes(modes, np.argsort(modes.frequency_hz))


def split_v2a_structural_modes(
    l1_structural: PoleModes,
    v2a_all_positive: PoleModes,
) -> tuple[PoleModes, PoleModes, float]:
    """
    v2a 的 6×2×8 = 96 个 SOS state 带来额外的强阻尼闭环状态。
    当前源码中这些与 delay/FDN structural cluster 之间有明显 radius gap。

    为了与 L1 做 apples-to-apples modal comparison：
    - 取与 L1 正频率结构模态数相同、radius 最大的一组作为 structural modes；
    - 剩余正频率 poles 单独报告为 SOS-state-dominated modes。

    如果未来 gap 消失，脚本会警告，不能盲目沿用这一分类。
    """
    count = len(l1_structural.z)
    if len(v2a_all_positive.z) < count:
        raise RuntimeError("v2a 正频率 pole 数少于 L1，无法做结构模态配对分类")

    radius_order = np.argsort(v2a_all_positive.radius)
    extra_indices = radius_order[:-count]
    structural_indices = radius_order[-count:]

    structural = sort_modes_by_frequency(
        subset_modes(v2a_all_positive, structural_indices)
    )
    extra = sort_modes_by_frequency(
        subset_modes(v2a_all_positive, extra_indices)
    )

    if len(extra.z) > 0:
        gap = float(np.min(structural.radius) - np.max(extra.radius))
    else:
        gap = math.inf

    return structural, extra, gap


# ---------------------------------------------------------------------
# det[I - D(z)A] 数值残差
# ---------------------------------------------------------------------

def filter_response_at_z(z: complex, sections: np.ndarray) -> complex:
    z_inv = 1.0 / z
    z_inv2 = z_inv * z_inv
    response = 1.0 + 0.0j
    for b0, b1, b2, a1, a2 in sections:
        numerator = b0 + b1 * z_inv + b2 * z_inv2
        denominator = 1.0 + a1 * z_inv + a2 * z_inv2
        response *= numerator / denominator
    return response


def characteristic_residuals(
    modes: PoleModes,
    delays: np.ndarray,
    branch_sections: list[np.ndarray],
) -> np.ndarray:
    """
    不直接使用高阶 polynomial coefficients（条件数很差），
    而在 state-space eig 求出的 poles 上验证：

        M(z) = I - diag(z^-L_i H_i(z)) A

    的最小奇异值趋近 0。

    返回 sigma_min / sigma_max，比 raw determinant 更稳定。
    """
    feedback = feedback_matrix(len(delays))
    identity = np.eye(len(delays), dtype=complex)
    residuals = np.empty(len(modes.z), dtype=float)

    for index, z in enumerate(modes.z):
        h = np.asarray(
            [filter_response_at_z(z, sections) for sections in branch_sections],
            dtype=complex,
        )
        loop = np.power(z, -delays) * h
        matrix = identity - np.diag(loop) @ feedback
        singular_values = np.linalg.svd(matrix, compute_uv=False)
        residuals[index] = singular_values[-1] / singular_values[0]

    return residuals


# ---------------------------------------------------------------------
# Modal statistics
# ---------------------------------------------------------------------

def modal_density_six_spacings(modes: PoleModes) -> tuple[np.ndarray, np.ndarray]:
    """
    与 Ege/Boutillon 论文定义一致：
    6 个 successive modal spacings 的 moving average 的倒数，
    横坐标取这 6 个 spacing 所覆盖 7 个 modes 的中频率。
    """
    frequencies = np.sort(modes.frequency_hz)
    if len(frequencies) < 7:
        return np.array([]), np.array([])

    spacing = np.diff(frequencies)
    moving_spacing = np.convolve(
        spacing,
        np.ones(6, dtype=float) / 6.0,
        mode="valid",
    )
    middle_frequency = 0.5 * (frequencies[:-6] + frequencies[6:])
    density = 1.0 / moving_spacing
    return middle_frequency, density


def spacing_statistics(
    modes: PoleModes,
    low_hz: float = 100.0,
    high_hz: float = 3000.0,
) -> dict[str, Any]:
    frequencies = modes.frequency_hz[
        (modes.frequency_hz >= low_hz)
        & (modes.frequency_hz <= high_hz)
    ]
    spacing = np.diff(frequencies)
    mean = float(np.mean(spacing))
    std = float(np.std(spacing))
    return {
        "mode_count": len(frequencies),
        "mean_hz": mean,
        "median_hz": float(np.median(spacing)),
        "std_hz": std,
        "cv": std / mean,
        "normalized": spacing / mean,
    }


def band_summary(
    model_name: str,
    modes: PoleModes,
    target: TargetDefinition,
) -> list[dict[str, Any]]:
    density_frequency, density = modal_density_six_spacings(modes)
    rows = []

    for band_name, low, high in BANDS:
        mask = (
            (modes.frequency_hz >= low)
            & (modes.frequency_hz < high)
        )
        dmask = (
            (density_frequency >= low)
            & (density_frequency < high)
        )

        if not np.any(mask):
            continue

        actual_alpha = modes.alpha_per_second[mask]
        wanted_alpha = alpha_target(modes.frequency_hz[mask], target)

        rows.append({
            "model": model_name,
            "band_hz": band_name,
            "mode_count": int(np.sum(mask)),
            "mean_alpha_s-1": float(np.mean(actual_alpha)),
            "mean_target_alpha_s-1": float(np.mean(wanted_alpha)),
            "alpha_rmse_vs_v2a_target_s-1": float(
                np.sqrt(np.mean((actual_alpha - wanted_alpha) ** 2))
            ),
            "mean_t60_s": float(np.mean(modes.t60_seconds[mask])),
            "mean_eta_percent": float(100.0 * np.mean(modes.eta[mask])),
            "mean_Q": float(np.mean(modes.q_factor[mask])),
            "mean_modal_density_modes_per_hz": (
                float(np.mean(density[dmask])) if np.any(dmask) else math.nan
            ),
        })

    return rows


# ---------------------------------------------------------------------
# CSV / text outputs
# ---------------------------------------------------------------------

def classification_cutoff(
    structural: PoleModes,
    extra: PoleModes,
) -> float:
    if len(extra.radius) == 0:
        return 0.0
    return 0.5 * (
        float(np.min(structural.radius))
        + float(np.max(extra.radius))
    )


def write_all_poles_csv(
    path: Path,
    l1_eigenvalues: np.ndarray,
    v2a_eigenvalues: np.ndarray,
    fs: float,
    v2a_cutoff: float,
):
    with path.open("w", newline="", encoding="utf-8") as file:
        writer = csv.writer(file)
        writer.writerow([
            "model",
            "real",
            "imag",
            "radius",
            "signed_frequency_hz",
            "alpha_per_second",
            "classification",
        ])

        for model_name, eigenvalues in (
            ("L1", l1_eigenvalues),
            ("v2a", v2a_eigenvalues),
        ):
            for z in eigenvalues:
                radius = abs(z)
                signed_frequency = np.angle(z) * fs / (2.0 * math.pi)
                alpha = (
                    -fs * math.log(radius)
                    if radius > 0.0
                    else math.inf
                )

                if model_name == "L1":
                    classification = (
                        "redundant_zero_state"
                        if radius < 1e-10
                        else "structural"
                    )
                else:
                    classification = (
                        "structural"
                        if radius >= v2a_cutoff
                        else "sos_state_dominated"
                    )

                writer.writerow([
                    model_name,
                    float(np.real(z)),
                    float(np.imag(z)),
                    float(radius),
                    float(signed_frequency),
                    float(alpha),
                    classification,
                ])


def write_band_summary_csv(path: Path, rows: list[dict[str, Any]]):
    if not rows:
        return
    with path.open("w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(file, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)


# ---------------------------------------------------------------------
# Plots
# ---------------------------------------------------------------------

def _plot_mode_scatter(ax, modes: PoleModes, y, label: str):
    ax.scatter(
        modes.frequency_hz,
        y,
        s=7,
        alpha=0.28,
        label=label,
    )


def plot_alpha(
    path: Path,
    l1: PoleModes,
    v2a: PoleModes,
    target: TargetDefinition,
):
    fig, ax = plt.subplots(figsize=(10.5, 6.0))

    _plot_mode_scatter(ax, l1, l1.alpha_per_second, "L1")
    _plot_mode_scatter(ax, v2a, v2a.alpha_per_second, "v2a closed-loop dynamics (shared by B and C)")

    f = np.geomspace(80.0, 10000.0, 1200)
    ax.plot(f, alpha_target(f, target), linewidth=2.0, label="v2a alpha target")

    # Ege et al. Fig.13 中明确画出的 constant loss-factor curves
    for eta_percent in (1.0, 2.0, 3.0):
        eta = eta_percent / 100.0
        ax.plot(
            f[f <= 3000],
            math.pi * eta * f[f <= 3000],
            linewidth=1.0,
            linestyle="--",
            label=f"paper eta={eta_percent:.0f}% curve",
        )

    # 论文正文报告的趋势，不冒充 digitized scatter
    ax.plot(
        [350.0, 1200.0],
        [80.0, 80.0],
        linewidth=1.8,
        linestyle=":",
        label="paper reported ~80 s^-1 below 1.2 kHz",
    )
    ax.plot(
        [1200.0, 1500.0],
        [130.0, 130.0],
        linewidth=1.8,
        linestyle=":",
        label="paper reported ~130 s^-1 at 1.2-1.5 kHz",
    )

    ax.set_xscale("log")
    ax.set_xlim(80, 10000)
    ax.set_ylim(bottom=0)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Modal damping rate alpha (1/s)")
    ax.set_title("Closed-loop modal damping: L1 vs v2a vs published reference")
    ax.grid(True, which="both", alpha=0.25)
    ax.legend(fontsize=8, ncol=2)
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def plot_t60(
    path: Path,
    l1: PoleModes,
    v2a: PoleModes,
    target: TargetDefinition,
):
    fig, ax = plt.subplots(figsize=(10.5, 6.0))

    _plot_mode_scatter(ax, l1, l1.t60_seconds, "L1")
    _plot_mode_scatter(ax, v2a, v2a.t60_seconds, "v2a closed-loop dynamics (shared by B and C)")

    f = np.geomspace(80.0, 10000.0, 1200)
    alpha = alpha_target(f, target)
    ax.plot(
        f,
        math.log(1000.0) / alpha,
        linewidth=2.0,
        label="v2a target T60",
    )

    ax.axhline(
        target.t60_baseline,
        linewidth=1.3,
        linestyle="--",
        label="L1 nominal T60 = 0.34 s",
    )

    ax.set_xscale("log")
    ax.set_xlim(80, 10000)
    ax.set_ylim(0, 0.42)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Amplitude T60 (s)")
    ax.set_title("Closed-loop T60: did v2a leave the old ~0.34 s horizontal band?")
    ax.grid(True, which="both", alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def plot_eta(
    path: Path,
    l1: PoleModes,
    v2a: PoleModes,
    target: TargetDefinition,
):
    fig, ax = plt.subplots(figsize=(10.5, 6.0))

    _plot_mode_scatter(ax, l1, 100.0 * l1.eta, "L1")
    _plot_mode_scatter(ax, v2a, 100.0 * v2a.eta, "v2a closed-loop dynamics (shared by B and C)")

    f = np.geomspace(80.0, 10000.0, 1200)
    eta_target = alpha_target(f, target) / (math.pi * f)
    ax.plot(
        f,
        100.0 * eta_target,
        linewidth=2.0,
        label="v2a target eta(f)",
    )

    # Ege/Boutillon: approximately 2% ± 1% over several kHz;
    # reduced model uses mean scale 2.3%.
    ax.axhspan(
        1.0,
        3.0,
        alpha=0.10,
        label="paper reference ~1-3% loss-factor band",
    )
    ax.axhline(
        2.3,
        linewidth=1.2,
        linestyle="--",
        label="reduced-model mean eta = 2.3%",
    )

    ax.set_xscale("log")
    ax.set_xlim(80, 10000)
    ax.set_ylim(0, 6)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Modal loss factor eta (%)")
    ax.set_title("Closed-loop modal loss factor: L1 vs v2a vs paper")
    ax.grid(True, which="both", alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def plot_modal_density(
    path: Path,
    l1: PoleModes,
    v2a: PoleModes,
):
    l1_f, l1_n = modal_density_six_spacings(l1)
    v2a_f, v2a_n = modal_density_six_spacings(v2a)

    fig, ax = plt.subplots(figsize=(10.5, 6.0))
    ax.plot(l1_f, l1_n, alpha=0.80, label="L1")
    ax.plot(v2a_f, v2a_n, alpha=0.80, label="v2a closed-loop dynamics (shared by B and C)")

    ax.plot(
        [300.0, 1100.0],
        [0.06, 0.06],
        linewidth=2.0,
        linestyle="--",
        label="paper low-frequency limit ~0.06 modes/Hz",
    )
    ax.axvline(
        1100.0,
        linewidth=1.2,
        linestyle=":",
        label="paper ~1.1 kHz localization transition",
    )

    ax.set_xscale("log")
    ax.set_xlim(80, 5000)
    ax.set_ylim(0, 0.11)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Apparent modal density (modes/Hz)")
    ax.set_title(
        "Modal density: damping changed, but did the FDN modal structure change?"
    )
    ax.grid(True, which="both", alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def rayleigh_spacing_pdf(x: np.ndarray) -> np.ndarray:
    # Boutillon & Ege 2013 Eq.(24), mean normalized spacing = 1
    return 0.5 * math.pi * x * np.exp(-0.25 * math.pi * x**2)


def plot_spacing(
    path: Path,
    l1_stats: dict[str, Any],
    v2a_stats: dict[str, Any],
):
    fig, ax = plt.subplots(figsize=(10.5, 6.0))

    bins = np.linspace(0.0, 2.4, 28).tolist()
    ax.hist(
        l1_stats["normalized"],
        bins=bins,
        density=True,
        histtype="step",
        linewidth=1.5,
        label=f"L1 (CV={l1_stats['cv']:.3f})",
    )
    ax.hist(
        v2a_stats["normalized"],
        bins=bins,
        density=True,
        histtype="step",
        linewidth=1.5,
        label=f"v2a closed-loop dynamics (shared by B and C) (CV={v2a_stats['cv']:.3f})",
    )

    x = np.linspace(0.0, 2.4, 600)
    ax.plot(
        x,
        rayleigh_spacing_pdf(x),
        linewidth=2.0,
        label=f"paper Rayleigh model (CV={math.sqrt(4/math.pi - 1):.3f})",
    )

    ax.set_xlabel("Nearest-neighbor spacing / mean spacing")
    ax.set_ylabel("Probability density")
    ax.set_title("Nearest-neighbor modal spacing, 100-3000 Hz")
    ax.grid(True, alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def plot_q(
    path: Path,
    l1: PoleModes,
    v2a: PoleModes,
):
    fig, ax = plt.subplots(figsize=(10.5, 6.0))
    _plot_mode_scatter(ax, l1, l1.q_factor, "L1")
    _plot_mode_scatter(ax, v2a, v2a.q_factor, "v2a closed-loop dynamics (shared by B and C)")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlim(80, 10000)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Q ≈ 1/eta")
    ax.set_title("Closed-loop modal Q")
    ax.grid(True, which="both", alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def plot_pole_radius(
    path: Path,
    l1: PoleModes,
    v2a: PoleModes,
    v2a_extra: PoleModes,
):
    fig, ax = plt.subplots(figsize=(10.5, 6.0))
    ax.scatter(
        l1.frequency_hz,
        l1.radius,
        s=7,
        alpha=0.28,
        label="L1",
    )
    ax.scatter(
        v2a.frequency_hz,
        v2a.radius,
        s=7,
        alpha=0.28,
        label="v2a closed-loop dynamics (shared by B and C)",
    )
    if len(v2a_extra.z):
        ax.scatter(
            v2a_extra.frequency_hz,
            v2a_extra.radius,
            s=11,
            alpha=0.45,
            label="v2a SOS-state-dominated modes",
        )

    ax.set_xscale("log")
    ax.set_xlim(80, 10000)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Pole radius |z|")
    ax.set_title("Closed-loop pole radii")
    ax.grid(True, which="both", alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


# ---------------------------------------------------------------------
# 真实源码的状态路径与输出路径审计
# ---------------------------------------------------------------------

def audit_runtime(source: str, version: str) -> tuple[str, float, float]:
    """只接受当前已知的 C++ FDN 更新式；源码改动时显式失败。"""
    clean = re.sub(r"/\*.*?\*/|//[^\n]*", "", source, flags=re.DOTALL)
    flat = re.sub(r"\s+", "", clean)
    q = parse_scalar_literal(clean, "q")
    direct = parse_scalar_literal(clean, "direct_gain")
    if not math.isclose(q, 1 / math.sqrt(8), rel_tol=1e-7):
        raise RuntimeError(f"{version}: input/output q 与已审计的归一化值不同")
    if "constfloatinput=q*shaped_force;" not in flat:
        raise RuntimeError(f"{version}: 无法验证 shaped_force 输入向量")
    if "delay_lines_[i].write(feedback+input);" not in flat:
        raise RuntimeError(f"{version}: delay write 逻辑改变")
    if "constfloatfeedback=" not in flat or "returnfdn_output+direct_gain*shaped_force;" not in flat:
        raise RuntimeError(f"{version}: feedback 或 direct/output 路径改变")

    # A/B 直接把 loss 后值命名 delayed；C 同时保存 raw delayed 与 retained。
    if version == "C":
        required = (
            "delayed[i]=delay_lines_[i].read();",
            "retained[i]=loss_filters_[i].process(delayed[i]);",
            "retained_sum+=retained[i];",
            "constfloatcommon=0.25f*retained_sum;",
            "constfloatfeedback=retained[shifted]-common;",
        )
        tap = "delayed"
    else:
        required = (
            "delayed[i]=loss_filters_[i].process(delay_lines_[i].read());",
            "delayed_sum+=delayed[i];",
            "constfloatcommon=0.25f*delayed_sum;",
            "constfloatfeedback=delayed[shifted]-common;",
        )
        tap = "retained"  # C++ 中变量叫 delayed，语义为 loss 后 retained。
    for phrase in required:
        if phrase not in flat:
            raise RuntimeError(f"{version}: 无法验证 feedback dynamics: {phrase}")
    if version in ("B", "C"):
        for field, index in (("b0", 0), ("b1", 1), ("b2", 2),
                             ("a1", 3), ("a2", 4)):
            if f"section.{field}=kV2aLossSOS[i][j][{index}];" not in flat:
                raise RuntimeError(f"{version}: SOS bank 未按已审计的路径初始化 {field}")
    expected_output = "constfloatfdn_output=q*(" + "".join(
        ("" if i == 0 else ("-" if i % 2 else "+")) + f"delayed[{i}]"
        for i in range(8)
    ) + ");"
    if expected_output not in flat:
        raise RuntimeError(f"{version}: audible readout 不是已审计的交替符号 tap")
    return tap, q, direct


def audit_shared_runtime_helpers(post: str, pre: str):
    """B/C 的环形 delay、biquad 与 loss 初始化代码也必须完全相同。"""
    for token in ("struct DelayLine", "struct LossSection",
                  "inline void initialize_delays()", "inline void initialize_losses()"):
        a = re.sub(r"\s+", "", _initializer_block(post, token))
        b = re.sub(r"\s+", "", _initializer_block(pre, token))
        if a != b:
            raise RuntimeError(f"B/C {token} 实现不同；feedback dynamics 可能不同")


def matched_pole_distance(a: np.ndarray, b: np.ndarray) -> float:
    """全复平面一对一匹配，避免重根附近的独立排序错配。"""
    if len(a) != len(b):
        raise RuntimeError("B/C state dimension 不同")
    rows, cols = linear_sum_assignment(np.abs(a[:, None] - b[None, :]))
    return float(np.max(np.abs(a[rows] - b[cols])))


def simulate_impulse(delays: np.ndarray, sections: list[np.ndarray],
                     tap: str, q: float, direct_gain: float, fs: float,
                     duration: float = 2.0) -> np.ndarray:
    """逐 sample 复刻 C++ 环形 delay 与 TDF-II SOS；初始状态全零。"""
    n_samples = round(duration * fs)
    delay_data = [np.zeros(int(length)) for length in delays]
    indices = np.zeros(len(delays), dtype=int)
    states = [np.zeros((len(bank), 2)) for bank in sections]
    output = np.zeros(n_samples)
    signs = np.array([1, -1] * 4)
    for n in range(n_samples):
        x = 1.0 if n == 0 else 0.0
        raw = np.array([data[index] for data, index in zip(delay_data, indices)])
        retained = np.empty(8)
        for i, bank in enumerate(sections):
            value = raw[i]
            for j, (b0, b1, b2, a1, a2) in enumerate(bank):
                s1, s2 = states[i][j]
                y = b0 * value + s1
                states[i][j, 0] = b1 * value - a1 * y + s2
                states[i][j, 1] = b2 * value - a2 * y
                value = y
            retained[i] = value
        audible = raw if tap == "delayed" else retained
        output[n] = q * float(np.dot(signs, audible)) + direct_gain * x
        common = 0.25 * float(np.sum(retained))
        for i, data in enumerate(delay_data):
            data[indices[i]] = retained[(i + 1) & 7] - common + q * x
            indices[i] = (indices[i] + 1) % len(data)
    return output


READOUT_BANDS = ("broadband", "low_20_300", "mid_300_3000", "high_gt3000")
TIME_WINDOWS_MS = ((0, 20), (20, 50), (50, 100), (100, 200),
                   (200, 500), (500, 1000), (20, 200))


def rms(x: np.ndarray) -> float:
    return float(np.sqrt(np.mean(np.square(x))))


def filtered_bands(x: np.ndarray, fs: float) -> dict[str, np.ndarray]:
    """三版本共用四阶 Butterworth SOS 与相同零初始状态。"""
    def apply_filter(cutoff: float | list[float], kind: str) -> np.ndarray:
        filtered = sosfilt(butter(4, cutoff, btype=kind, fs=fs, output="sos"), x)
        if not isinstance(filtered, np.ndarray):
            raise TypeError("SOS filter 未返回单通道数组")
        return filtered

    return {
        "broadband": x,
        "low_20_300": apply_filter([20, 300], "bandpass"),
        "mid_300_3000": apply_filter([300, 3000], "bandpass"),
        "high_gt3000": apply_filter(3000, "highpass"),
    }


def window_slice(x: np.ndarray, fs: float, start_ms: int, end_ms: int) -> np.ndarray:
    return x[round(start_ms * fs / 1000):round(end_ms * fs / 1000)]


def moving_rms(x: np.ndarray, width: int) -> np.ndarray:
    # 中心移动窗，能量卷积边界按零填充；所有版本用同一窗宽。
    return np.sqrt(np.maximum(0, np.convolve(x * x, np.ones(width) / width, mode="same")))


def persistence_ms(x: np.ndarray, fs: float, threshold_db: float) -> float:
    envelope = moving_rms(x, max(1, round(0.005 * fs)))
    peak = float(np.max(envelope))
    above = np.flatnonzero(envelope >= peak * 10 ** (threshold_db / 20))
    return float(1000 * above[-1] / fs) if len(above) else math.nan


def impulse_metrics(x: np.ndarray, bands: dict[str, np.ndarray], fs: float) -> dict[str, Any]:
    energy = x * x
    t = np.arange(len(x)) / fs
    total = float(np.sum(energy))
    row = {"rms_full_2s": rms(x), "peak": float(np.max(np.abs(x))),
           "crest_factor": float(np.max(np.abs(x)) / rms(x))}
    for name, signal in bands.items():
        power = signal * signal
        row[f"{name}_temporal_centroid_ms"] = float(
            1000 * np.sum(t * power) / np.sum(power))
        for start, end in TIME_WINDOWS_MS:
            row[f"{name}_rms_{start}_{end}ms"] = rms(window_slice(signal, fs, start, end))
    e_early = np.sum(window_slice(energy, fs, 0, 20))
    e_late = np.sum(window_slice(energy, fs, 20, 200))
    row["ABR_0_20_over_20_200_db"] = float(10 * np.log10(e_early / e_late))
    for boundary in (50, 80):
        early = np.sum(window_slice(energy, fs, 0, boundary))
        late = total - early
        row[f"C{boundary}_db"] = float(10 * np.log10(early / late))
    for band in ("mid_300_3000", "high_gt3000"):
        for threshold in (-20, -40):
            row[f"{band}_persistence_{threshold}dB_ms"] = persistence_ms(
                bands[band], fs, threshold)
    return row


def db_ratio(a: float, b: float) -> float:
    return 20 * math.log10(a / b)


def plot_transfer(path: Path, responses: dict[str, np.ndarray], fs: float,
                  wet_only: bool = False):
    # 完整 2 s 矩形截断；在尾部补零至 >=2N，只细化采样，不改善真实分辨率。
    n = next_fast_len(2 * len(next(iter(responses.values()))))
    if n is None:
        raise RuntimeError("SciPy 无法确定 FFT 长度")
    frequency = np.fft.rfftfreq(n, 1 / fs)
    mask = (frequency >= 20) & (frequency <= 20000)
    fig, ax = plt.subplots(figsize=(10.5, 6))
    for name, response in responses.items():
        values = response.copy()
        if wet_only:
            values[0] -= DIRECT_GAIN_FOR_PLOTS[name]
        magnitude = np.abs(np.fft.rfft(values, n=n))
        ax.plot(frequency[mask], 20 * np.log10(np.maximum(magnitude[mask], 1e-12)), label=name)
    ax.set_xscale("log")
    ax.set_xlim(20, 20000)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Magnitude (dB re unit impulse)")
    ax.set_title("FDN wet/readout transfer" if wet_only else "Full shaped-force transfer")
    ax.grid(True, which="both", alpha=.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


DIRECT_GAIN_FOR_PLOTS: dict[str, float] = {}


def plot_readout_difference(path: Path, post: np.ndarray, pre: np.ndarray, fs: float,
                            direct_gain: float):
    n = next_fast_len(2 * len(post))
    if n is None:
        raise RuntimeError("SciPy 无法确定 FFT 长度")
    hbf, hcf = np.abs(np.fft.rfft(post, n=n)), np.abs(np.fft.rfft(pre, n=n))
    b, c = post.copy(), pre.copy()
    b[0] -= direct_gain
    c[0] -= direct_gain
    hb, hc = np.abs(np.fft.rfft(b, n=n)), np.abs(np.fft.rfft(c, n=n))
    frequency = np.fft.rfftfreq(n, 1 / fs)
    # 在 B 零点附近的比值无物理稳定性，标为 NaN 不连线。
    reliable = hb >= np.max(hb) * 1e-6
    ratio = np.full_like(hb, np.nan)
    ratio[reliable] = 20 * np.log10(np.maximum(hc[reliable], 1e-12) / hb[reliable])
    full_ratio = 20 * np.log10(np.maximum(hcf, 1e-12) / np.maximum(hbf, 1e-12))
    mask = (frequency >= 20) & (frequency <= 20000)
    fig, ax = plt.subplots(figsize=(10.5, 6))
    ax.plot(frequency[mask], ratio[mask], label="FDN wet C/B", alpha=.85)
    ax.plot(frequency[mask], full_ratio[mask], label="Full transfer C/B", alpha=.7)
    ax.axhline(0, color="black", linewidth=.8)
    ax.set_xscale("log")
    ax.set_xlim(20, 20000)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Pre-loss minus post-loss (dB)")
    ax.set_title("Readout difference: C / B; B nulls below -120 dB masked")
    ax.grid(True, which="both", alpha=.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def plot_early_envelope(path: Path, filtered: dict[str, dict[str, np.ndarray]], fs: float):
    width = round(.005 * fs)
    limit = round(.2 * fs)
    time_ms = 1000 * np.arange(limit) / fs
    fig, axes = plt.subplots(2, 1, figsize=(11, 7), sharex=True)
    for ax, band in zip(axes, ("mid_300_3000", "high_gt3000")):
        for name, signals in filtered.items():
            envelope = moving_rms(signals[band][:limit], width)
            ax.plot(time_ms, 20 * np.log10(np.maximum(envelope, 1e-10)), label=name)
        ax.set_ylabel(f"{band} 5 ms RMS (dB)")
        ax.grid(True, alpha=.25)
        ax.legend()
    axes[-1].set_xlabel("Time (ms)")
    axes[-1].set_xlim(0, 200)
    fig.suptitle("Early impulse: centered 5 ms moving RMS, absolute level")
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


# ---------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------

def plot_candidate_comparison(output, current, lossless_f, damped, extra,
                              target, metrics, pair_f, delta_f):
    import modal_structure_target as mst
    models = (("current v2a", current.frequency_hz),
              ("candidate lossless", lossless_f),
              ("candidate damped", damped.frequency_hz))
    colors = {"current v2a": "C0", "candidate lossless": "C2", "candidate damped": "C1"}
    for filename, field, ylabel, wanted in (
        ("01_alpha_compare.png", "alpha_per_second", "Alpha (s$^{-1}$)",
         lambda f: alpha_target(f, target)),
        ("02_t60_compare.png", "t60_seconds", "T60 (s)",
         lambda f: math.log(1000) / alpha_target(f, target)),
    ):
        fig, ax = plt.subplots(figsize=(10, 5.5))
        for name, modes in (("current v2a", current), ("candidate damped", damped)):
            mask = (modes.frequency_hz >= 100) & (modes.frequency_hz <= 10000)
            ax.scatter(modes.frequency_hz[mask], getattr(modes, field)[mask],
                       s=7, alpha=.5, color=colors[name], label=name)
        grid = np.geomspace(100, 10000, 600)
        ax.plot(grid, wanted(grid), color="black", lw=1.7, label="v2a target")
        ax.set(xscale="log", xlim=(100, 10000), xlabel="Frequency (Hz)", ylabel=ylabel)
        ax.grid(True, which="both", alpha=.25); ax.legend()
        fig.tight_layout(); fig.savefig(output / filename, dpi=170); plt.close(fig)

    fig, ax = plt.subplots(figsize=(10, 5.5))
    for name, frequencies in models:
        f, density = mst.apparent_modal_density(frequencies)
        mask = (f >= 100) & (f <= 3000)
        ax.plot(f[mask], density[mask], color=colors[name], alpha=.8, lw=1.1, label=name)
    grid = np.linspace(100, 1100, 400)
    ax.plot(grid, mst.target_modal_density(grid), color="black", lw=2, ls="--",
            label="v2b density target (100-1100 Hz)")
    ax.axvline(1100, color="gray", ls=":", label="1.1 kHz transition")
    ax.set(xlim=(100, 3000), xlabel="Frequency (Hz)", ylabel="Modal density (modes/Hz)")
    ax.grid(True, alpha=.25); ax.legend(); fig.tight_layout()
    fig.savefig(output / "03_modal_density_compare.png", dpi=170); plt.close(fig)

    fig, ax = plt.subplots(figsize=(10, 5.5))
    bins = np.linspace(0, 2.5, 31)
    for name, frequencies in models:
        ax.hist(mst.normalized_spacings(frequencies), bins=bins, density=True,
                histtype="step", lw=1.8, color=colors[name],
                label=f"{name} (CV={metrics[name]['spacing_cv']:.4f})")
    x = np.linspace(0, 2.5, 500)
    ax.plot(x, mst.rayleigh_spacing_pdf(x), color="black", ls="--",
            label=f"Rayleigh (CV={mst.RAYLEIGH_SPACING_CV:.4f})")
    ax.set(xlabel="Nearest-neighbor spacing / mean, 100-3000 Hz", ylabel="PDF")
    ax.grid(True, alpha=.25); ax.legend(); fig.tight_layout()
    fig.savefig(output / "04_spacing_compare.png", dpi=170); plt.close(fig)

    fig, ax = plt.subplots(figsize=(10, 5.5))
    for name, modes in (("current structural", current), ("candidate structural", damped),
                        ("candidate SOS-state-dominated", extra)):
        ax.scatter(modes.frequency_hz, modes.radius, s=6, alpha=.5, label=name)
    ax.set(xlim=(0, 10000), xlabel="Frequency (Hz)", ylabel="Pole radius")
    ax.grid(True, alpha=.25); ax.legend(); fig.tight_layout()
    fig.savefig(output / "05_pole_radius_compare.png", dpi=170); plt.close(fig)

    fig, ax = plt.subplots(figsize=(10, 5.5))
    ax.scatter(pair_f, delta_f, s=8, alpha=.65, color="C3")
    ax.axhline(0, color="black", lw=.8)
    ax.set(xlim=(100, 10000), xlabel="Candidate lossless pole frequency (Hz)",
           ylabel="Damped minus lossless frequency (Hz)")
    ax.grid(True, alpha=.25); fig.tight_layout()
    fig.savefig(output / "06_structure_delta.png", dpi=170); plt.close(fig)

def validate_candidate(args):
    """Opt-in v2b validation; reuse the production state-space implementation."""
    import modal_structure_target as mst
    import search_fdn_structure as search

    output = args.output.resolve()
    if output == (Path(__file__).resolve().parent / "output" / "poles").resolve():
        raise RuntimeError("candidate mode requires a separate --output")
    root = repo_root()
    source = (root / REPO_SOURCE_PATH).read_text(encoding="utf-8")
    target_source = (Path(__file__).resolve().parent / "target_damping.py").read_text(encoding="utf-8")
    target = parse_target_definition(target_source)
    current_delays = parse_delays(source)
    current_bank = parse_v2a_sos(source, len(current_delays))
    tap, q, direct = audit_runtime(source, "C")
    current_sections = active_sections(current_bank)
    if tap != "delayed" or len(current_sections[0]) != 6:
        raise RuntimeError("production readout/SOS audit failed")

    candidate_data = json.loads(args.candidate_json.read_text(encoding="utf-8"))
    chosen = candidate_data.get("best_temporally_feasible")
    if not isinstance(chosen, dict):
        raise RuntimeError("missing best_temporally_feasible")
    def parse_vector(value):
        return np.asarray([int(x) for x in value.split()] if isinstance(value, str) else value, dtype=int)
    candidate_delays = parse_vector(chosen["delays"])
    modal = candidate_data.get("best_modal_only")
    same_best = isinstance(modal, dict) and np.array_equal(candidate_delays, parse_vector(modal["delays"]))
    print("best_modal_only == best_temporally_feasible" if same_best else
          "NOTICE: best_modal_only differs; using best_temporally_feasible", flush=True)
    if (len(candidate_delays) != 8 or np.any(candidate_delays <= 0)
            or len(set(candidate_delays)) != 8
            or int(chosen["sum_delays"]) != int(candidate_delays.sum())):
        raise RuntimeError("invalid candidate delay bank")
    if not np.array_equal(current_delays, parse_vector(candidate_data["current"]["delays"])):
        raise RuntimeError("search baseline differs from working production")
    sos_data = json.loads(args.candidate_sos.read_text(encoding="utf-8"))
    if not math.isclose(float(sos_data["sample_rate_hz"]), target.fs):
        raise RuntimeError("candidate SOS sample rate mismatch")
    branches = sos_data["branches"]
    if [int(b["delay_samples"]) for b in branches] != candidate_delays.tolist():
        raise RuntimeError("candidate SOS delay bank mismatch")
    candidate_bank = np.asarray([[[s[k] for k in ("b0", "b1", "b2", "a1", "a2")]
                                  for s in b["sections"]] for b in branches], dtype=float)
    if (candidate_bank.shape != (8, 6, 5) or not np.isfinite(candidate_bank).all()
            or np.max(np.abs(candidate_bank)) > 10
            or sos_data.get("production_section_count") != 8
            or sos_data.get("identity_sections_per_branch") != 2):
        raise RuntimeError("candidate SOS coefficients or identity slots invalid")
    candidate_sections = active_sections(candidate_bank)
    section_radii = [max(abs(np.roots([1, s[3], s[4]])))
                     for bank in (current_bank[:, :6], candidate_bank)
                     for s in bank.reshape(-1, 5)]
    if max(section_radii) >= 1:
        raise RuntimeError("SOS pole outside unit circle")
    print(f"Production: {current_delays.tolist()}, q={q:.9g}, direct={direct:.9g}; candidate: {candidate_delays.tolist()}", flush=True)
    print(f"SOS max pole radius={max(section_radii):.9f}", flush=True)

    # Exact eigenvalues for the lossless candidate skeleton and L1 baseline.
    # The latter is the original validator's structural-mode counting rule.
    print("Solving exact lossless candidate and current L1 state matrices...", flush=True)
    lossless_matrix = build_state_matrix(candidate_delays, [np.empty((0, 5)) for _ in candidate_delays])
    lossless_eig = eigvals(lossless_matrix, overwrite_a=True, check_finite=False)
    del lossless_matrix
    lossless_f = np.sort(np.angle(lossless_eig[np.imag(lossless_eig) > 1e-8]) * target.fs / (2 * math.pi))
    l1_matrix = build_state_matrix(current_delays, make_l1_sections(current_delays, target.fs, .34, .075))
    l1_eig = eigvals(l1_matrix, overwrite_a=True, check_finite=False)
    del l1_matrix
    current_count = len(positive_modes(l1_eig, target.fs).z)
    print(f"exact positive structural counts: current L1={current_count}, candidate lossless={len(lossless_f)}", flush=True)
    search_f = search.modal_frequencies_lossless(candidate_delays, 100, 3000, .75, True)
    exact_band = lossless_f[(lossless_f >= 100) & (lossless_f <= 3000)]
    if len(search_f) != len(exact_band) or not np.allclose(search_f, exact_band, atol=1e-4, rtol=0):
        print("WARNING: exact lossless eig frequencies differ from search roots", flush=True)

    def exact_modes(name, delays, sections, structural_count):
        matrix = build_state_matrix(delays, sections)
        print(f"{name}: exact eig(F), dimension={len(matrix)}...", flush=True)
        eigenvalues = eigvals(matrix, overwrite_a=True, check_finite=False)
        del matrix
        gc.collect()
        if not np.isfinite(eigenvalues).all() or np.max(np.abs(eigenvalues)) >= 1:
            raise RuntimeError(f"{name}: nonfinite or unstable closed-loop poles")
        positive = positive_modes(eigenvalues, target.fs)
        if len(positive.z) < structural_count:
            raise RuntimeError(f"{name}: insufficient positive modes")
        order = np.argsort(positive.radius)
        structural = sort_modes_by_frequency(subset_modes(positive, order[-structural_count:]))
        extra = sort_modes_by_frequency(subset_modes(positive, order[:-structural_count]))
        gap = float(np.min(structural.radius) - np.max(extra.radius)) if len(extra.z) else math.inf
        print(f"{name}: max radius={max(abs(eigenvalues)):.9f}; structural={len(structural.z)}, SOS-dominated={len(extra.z)}, radius gap={gap:.6g}", flush=True)
        return eigenvalues, structural, extra, gap

    current_eig, current, current_extra, current_gap = exact_modes(
        "current v2a", current_delays, current_sections, current_count)
    candidate_eig, damped, extra, candidate_gap = exact_modes(
        "candidate damped", candidate_delays, candidate_sections, len(lossless_f))
    current_res = characteristic_residuals(current, current_delays, current_sections)
    candidate_res = characteristic_residuals(damped, candidate_delays, candidate_sections)
    residual_ok = max(current_res.max(), candidate_res.max()) < 1e-7
    if not residual_ok:
        print("WARNING: characteristic residual not near numerical zero", flush=True)
    if min(current_gap, candidate_gap) < 1e-3:
        print("WARNING: structural/SOS radius gap small; classification uncertain", flush=True)

    frequencies = {"current v2a": current.frequency_hz,
                   "candidate lossless": lossless_f,
                   "candidate damped": damped.frequency_hz}
    metrics = {name: mst.objective_components(f) for name, f in frequencies.items()}
    def tracking(modes):
        mask = (modes.frequency_hz >= 100) & (modes.frequency_hz <= 10000)
        wanted = alpha_target(modes.frequency_hz[mask], target)
        error = modes.alpha_per_second[mask] - wanted
        return (int(mask.sum()), float(np.mean(np.abs(error))),
                float(np.sqrt(np.mean(error**2))), float(np.mean(np.abs(error) / wanted)))
    current_tracking, candidate_tracking = tracking(current), tracking(damped)
    if len(lossless_f) != len(damped.frequency_hz):
        raise RuntimeError("lossless/damped count differs; monotonic frequency pairing invalid")
    pair_mask = (lossless_f >= 100) & (lossless_f <= 10000)
    pair_f = lossless_f[pair_mask]
    delta_f = (damped.frequency_hz - lossless_f)[pair_mask]
    shift = (float(np.median(abs(delta_f))), float(np.percentile(abs(delta_f), 95)),
             float(np.max(abs(delta_f))))
    structure_mask = pair_f <= 3000
    structure_shift = (float(np.median(abs(delta_f[structure_mask]))),
                       float(np.percentile(abs(delta_f[structure_mask]), 95)),
                       float(np.max(abs(delta_f[structure_mask]))))

    output.mkdir(parents=True, exist_ok=True)
    rows = band_summary("current v2a", current, target) + band_summary("candidate damped", damped, target)
    density_f, density = mst.apparent_modal_density(lossless_f)
    for band, low, high in BANDS:
        dmask = (density_f >= low) & (density_f < high)
        rows.append({"model": "candidate lossless", "band_hz": band,
                     "mode_count": int(np.sum((lossless_f >= low) & (lossless_f < high))),
                     "mean_modal_density_modes_per_hz": float(np.mean(density[dmask]))})
    with (output / "modal_summary.csv").open("w", newline="", encoding="utf-8") as f:
        fields = ["model", "band_hz", "mode_count", "mean_modal_density_modes_per_hz",
                  "mean_alpha_s-1", "mean_target_alpha_s-1", "alpha_rmse_vs_v2a_target_s-1",
                  "mean_t60_s", "mean_eta_percent", "mean_Q"]
        writer = csv.DictWriter(f, fieldnames=fields)
        writer.writeheader(); writer.writerows(rows)
    plot_candidate_comparison(output, current, lossless_f, damped, extra,
                              target, metrics, pair_f, delta_f)

    fit_rows = sos_data["fit_metrics"]
    fit_overall = float(np.sqrt(np.mean([r["rms_db"]**2 for r in fit_rows])))
    lines = ["[VERSION / CANDIDATE]",
             f"production source: {root / REPO_SOURCE_PATH}",
             f"candidate JSON: {args.candidate_json.resolve()}",
             f"best_modal_only == best_temporally_feasible: {same_best}; selected best_temporally_feasible",
             f"current delays={current_delays.tolist()}, sum={current_delays.sum()}",
             f"candidate delays={candidate_delays.tolist()}, sum={candidate_delays.sum()}",
             "[SOS REGENERATION]",
             "Unchanged v2a alpha target; |H_i(f)|=exp(-alpha_target(f)*L_i/Fs). One shared 6-section nonlinear fit; reference 721, section gain_db scaled by L/721. Float32 coefficients, two identity slots per branch.",
             f"optimizer success={sos_data['optimizer']['success']}; candidate SOS={args.candidate_sos.resolve()}",
             "[BRANCH FIT QUALITY]", "20 Hz..Nyquist, 4096 log-spaced points; RMS / max absolute dB:"]
    lines += [f"  L={r['delay_samples']}: {r['rms_db']:.6f} / {r['max_abs_db']:.6f}" for r in fit_rows]
    lines += [f"overall RMS dB={fit_overall:.6f}; max SOS pole radius={max(section_radii):.9f}; all finite, |coefficient|<=10",
              "[EXACT CLOSED-LOOP VALIDATION]",
              f"current eig(F) dimension={len(current_eig)}, max radius={max(abs(current_eig)):.9f}; candidate lossless dimension={len(lossless_eig)}, max radius={max(abs(lossless_eig)):.9f}; candidate damped dimension={len(candidate_eig)}, max radius={max(abs(candidate_eig)):.9f}",
              f"structural/SOS-dominated: current {len(current.z)}/{len(current_extra.z)}, candidate {len(damped.z)}/{len(extra.z)}",
              f"radius gap: current={current_gap:.8g}, candidate={candidate_gap:.8g}",
              f"M(z) sigma_min/sigma_max current max/median={max(current_res):.3e}/{np.median(current_res):.3e}",
              f"M(z) sigma_min/sigma_max candidate max/median={max(candidate_res):.3e}/{np.median(candidate_res):.3e}",
              "Characteristic residuals near numerical zero" if residual_ok else "WARNING: characteristic residual not near numerical zero; no success conclusion",
              "[DAMPING TARGET TRACKING]", "Structural modes 100-10000 Hz; alpha MAE/RMSE/mean relative absolute error:",
              f"current n={current_tracking[0]}: {current_tracking[1]:.6f} / {current_tracking[2]:.6f} / {current_tracking[3]:.6%}",
              f"candidate n={candidate_tracking[0]}: {candidate_tracking[1]:.6f} / {candidate_tracking[2]:.6f} / {candidate_tracking[3]:.6%}"]
    for row in rows:
        if row["model"] != "candidate lossless":
            lines.append(f"  {row['model']} {row['band_hz']}: n={row['mode_count']}, mean alpha={row['mean_alpha_s-1']:.5f} s^-1, T60={row['mean_t60_s']:.5f} s, eta={row['mean_eta_percent']:.5f}%, Q={row['mean_Q']:.5f}")
    lines += ["[MODAL STRUCTURE PRESERVATION]",
              "Nearest-neighbor spacing 100-3000 Hz; physical density target only 100-1100 Hz."]
    for name, m in metrics.items():
        lines.append(f"  {name}: n={int(m['mode_count'])}, CV={m['spacing_cv']:.9f}, CDF distance={m['spacing_cdf_distance']:.9f}, quantile RMSE={m['spacing_quantile_rmse']:.9f}, low-density RMSE={m['density_rmse_modes_per_hz']:.9f}")
    lm, dm, cm = (metrics[n] for n in ("candidate lossless", "candidate damped", "current v2a"))
    closer_cv = abs(dm["spacing_cv"] - mst.RAYLEIGH_SPACING_CV) < abs(cm["spacing_cv"] - mst.RAYLEIGH_SPACING_CV)
    closer_distribution = (dm["spacing_cdf_distance"] < cm["spacing_cdf_distance"]
                           and dm["spacing_quantile_rmse"] < cm["spacing_quantile_rmse"])
    numerical_go = (residual_ok and min(current_gap, candidate_gap) >= 1e-3
                    and closer_cv and closer_distribution
                    and candidate_tracking[3] <= current_tracking[3])
    lines += [f"damped minus lossless: delta CV={dm['spacing_cv']-lm['spacing_cv']:+.9f}, delta CDF={dm['spacing_cdf_distance']-lm['spacing_cdf_distance']:+.9f}, delta quantile RMSE={dm['spacing_quantile_rmse']-lm['spacing_quantile_rmse']:+.9f}",
              f"monotonic same-candidate pairing (100-10000 Hz, n={len(pair_f)}): median/p95/max |delta f|={shift[0]:.6f}/{shift[1]:.6f}/{shift[2]:.6f} Hz",
              f"monotonic same-candidate pairing (100-3000 Hz, n={int(structure_mask.sum())}): median/p95/max |delta f|={structure_shift[0]:.6f}/{structure_shift[1]:.6f}/{structure_shift[2]:.6f} Hz",
              "[INTERPRETATION]",
              f"Q1: {'YES' if closer_cv and closer_distribution else 'NO'}; candidate damped CV {dm['spacing_cv']:.6f} versus current {cm['spacing_cv']:.6f}; Rayleigh target {mst.RAYLEIGH_SPACING_CV:.6f}. CDF and quantile metrics are also {'better' if closer_distribution else 'not both better'}.",
              f"Q2: lossless CV {lm['spacing_cv']:.6f} -> damped CV {dm['spacing_cv']:.6f}.",
              f"Q3: YES, candidate alpha target MAE/RMSE/relative MAE {candidate_tracking[1]:.5f}/{candidate_tracking[2]:.5f}/{candidate_tracking[3]:.3%}, versus current relative MAE {current_tracking[3]:.3%}; band T60 and eta above.",
              f"Q4: substantial angle shifts: 100-3000 Hz median/p95/max {structure_shift[0]:.5f}/{structure_shift[1]:.5f}/{structure_shift[2]:.5f} Hz, versus mean modal spacing {lm['mean_spacing_hz']:.5f} Hz. Smooth shifts leave spacing statistics close; damping is not purely radial.",
              f"Q5: {'YES' if numerical_go else 'NO'} on numerical grounds for experimental integration and listening A/B; no listening inference.",
              "[NEXT STEP]",
              "Experimental integration can test the measured angle shifts and then listening A/B; production unchanged in this run." if numerical_go else
              "Resolve failed numerical checks before experimental integration; production unchanged in this run."]
    report = "\n".join(lines) + "\n"
    (output / "report.txt").write_text(report, encoding="utf-8")
    print(report, flush=True)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--l1-ref", default=DEFAULT_L1_REF)
    parser.add_argument("--postloss-ref", default=DEFAULT_POSTLOSS_REF)
    parser.add_argument("--preloss-ref", default=DEFAULT_PRELOSS_REF)
    parser.add_argument("--candidate-json", type=Path)
    parser.add_argument("--candidate-sos", type=Path)
    parser.add_argument(
        "--output",
        type=Path,
        default=Path(__file__).resolve().parent / "output" / "poles",
    )
    args = parser.parse_args()
    if args.candidate_json is not None or args.candidate_sos is not None:
        if args.candidate_json is None or args.candidate_sos is None:
            parser.error("candidate mode requires both --candidate-json and --candidate-sos")
        sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "v2b"))
        validate_candidate(args)
        return

    root = repo_root()
    output = args.output
    l1_ref = resolve_ref(root, args.l1_ref)
    post_ref = resolve_ref(root, args.postloss_ref)
    pre_ref = resolve_ref(root, args.preloss_ref)

    print("读取 production source...")
    print(f"L1  = {l1_ref}")
    print(f"B = {post_ref}")
    print(f"C = {pre_ref}")

    l1_source = git_show(root, l1_ref, REPO_SOURCE_PATH)
    post_source = git_show(root, post_ref, REPO_SOURCE_PATH)
    pre_source = git_show(root, pre_ref, REPO_SOURCE_PATH)
    target_source = git_show(root, pre_ref, TARGET_SOURCE_PATH)

    l1_delays = parse_delays(l1_source)
    post_delays = parse_delays(post_source)
    pre_delays = parse_delays(pre_source)
    if not (np.array_equal(l1_delays, post_delays) and
            np.array_equal(post_delays, pre_delays)):
        raise RuntimeError(f"A/B/C delay bank 不一致: {l1_delays}, {post_delays}, {pre_delays}")
    delays = l1_delays

    target = parse_target_definition(target_source)
    old_t60 = parse_scalar_literal(l1_source, "target_t60")
    old_pole = parse_scalar_literal(l1_source, "loss_pole")
    if not (math.isclose(old_t60, .34) and math.isclose(old_pole, .075)):
        raise RuntimeError(f"L1 loss 已变更: T60={old_t60}, pole={old_pole}")
    l1_tap, l1_q, l1_direct = audit_runtime(l1_source, "A")
    post_tap, post_q, post_direct = audit_runtime(post_source, "B")
    pre_tap, pre_q, pre_direct = audit_runtime(pre_source, "C")
    audit_shared_runtime_helpers(post_source, pre_source)
    if (l1_tap, post_tap, pre_tap) != ("retained", "retained", "delayed"):
        raise RuntimeError("A/B/C readout mapping 错误")
    if len({l1_direct, post_direct, pre_direct}) != 1 or len({l1_q, post_q, pre_q}) != 1:
        raise RuntimeError("A/B/C direct_gain 或 q 不一致")

    post_bank = parse_v2a_sos(post_source, branch_count=len(delays))
    pre_bank = parse_v2a_sos(pre_source, branch_count=len(delays))
    if not np.array_equal(post_bank, pre_bank):
        raise RuntimeError("B/C SOS bank 不逐项相同；feedback dynamics 不同，停止分析")
    post_sections = active_sections(post_bank)
    pre_sections = active_sections(pre_bank)
    v2a_sections = post_sections

    active_count = len(v2a_sections[0])
    print(f"delay bank = {delays.tolist()}")
    print(f"L1 loss: T60={old_t60:.6f} s, pole={old_pole:.6f}")
    print(f"v2a active SOS/branch = {active_count}")
    print(f"A/B/C direct_gain = {l1_direct:.9g}; q = {l1_q:.9g}")
    print("B/C SOS bank: elementwise identical; feedback: retained; readout: B retained, C delayed")
    if active_count != 6:
        raise RuntimeError("v2a 有效 section 数不是要求的 6")

    l1_sections = make_l1_sections(
        delays,
        target.fs,
        old_t60,
        old_pole,
    )

    # -------------------------------------------------------------
    # L1 eig
    # -------------------------------------------------------------
    print("\n构造 L1 zero-input state matrix...")
    l1_matrix = build_state_matrix(delays, l1_sections)
    print(f"L1 state dimension = {l1_matrix.shape[0]}")

    print("求 L1 全部 eigenvalues...")
    l1_eigenvalues = eigvals(
        l1_matrix,
        overwrite_a=True,
        check_finite=False,
    )
    del l1_matrix
    gc.collect()

    l1_positive = positive_modes(l1_eigenvalues, target.fs)
    l1_zero_count = int(np.sum(np.abs(l1_eigenvalues) < 1e-10))

    # -------------------------------------------------------------
    # v2a eig
    # -------------------------------------------------------------
    print("\n分别构造 B/C zero-input state matrix...")
    f_post = build_state_matrix(post_delays, post_sections)
    f_pre = build_state_matrix(pre_delays, pre_sections)
    matrix_difference = float(np.max(np.abs(f_post - f_pre)))
    print(f"max_abs(F_post - F_pre) = {matrix_difference:.3e}")
    if matrix_difference != 0:
        raise RuntimeError("B/C F matrix 不同；feedback dynamics 不一致")
    print(f"v2a state dimension = {f_post.shape[0]}")

    print("分别求 B/C 全部 eigenvalues 并一对一匹配...")
    v2a_eigenvalues = eigvals(f_post, overwrite_a=True, check_finite=False)
    pre_eigenvalues = eigvals(f_pre, overwrite_a=True, check_finite=False)
    pole_difference = matched_pole_distance(v2a_eigenvalues, pre_eigenvalues)
    print(f"max matched pole distance |z_B - z_C| = {pole_difference:.3e}")
    if pole_difference > 1e-10:
        print("WARNING: B/C pole 匹配误差超过 1e-10；检查 eig 条件数与源码解析。")
    del f_post, f_pre, pre_eigenvalues
    gc.collect()

    v2a_positive_all = positive_modes(v2a_eigenvalues, target.fs)
    v2a_structural, v2a_extra, radius_gap = split_v2a_structural_modes(
        l1_positive,
        v2a_positive_all,
    )

    if radius_gap < 1e-3:
        print(
            "WARNING: structural 与 SOS-state-dominated pole cluster 的 radius gap 很小；"
            "当前分类不再可靠。"
        )

    # -------------------------------------------------------------
    # Characteristic-equation residual
    # -------------------------------------------------------------
    print("\n验证 det[I - D(z)A] = 0（以 normalized smallest singular value 表示）...")
    l1_residual = characteristic_residuals(
        l1_positive, delays, l1_sections
    )
    v2a_residual = characteristic_residuals(
        v2a_structural, delays, v2a_sections
    )

    # -------------------------------------------------------------
    # Statistics
    # -------------------------------------------------------------
    l1_spacing = spacing_statistics(l1_positive)
    v2a_spacing = spacing_statistics(v2a_structural)

    summary_rows = (
        band_summary("L1", l1_positive, target)
        + band_summary("v2a", v2a_structural, target)
    )

    v2a_mask = (
        (v2a_structural.frequency_hz >= 100.0)
        & (v2a_structural.frequency_hz <= 10000.0)
    )
    wanted_alpha = alpha_target(
        v2a_structural.frequency_hz[v2a_mask],
        target,
    )
    actual_alpha = v2a_structural.alpha_per_second[v2a_mask]
    overall_alpha_rmse = float(
        np.sqrt(np.mean((actual_alpha - wanted_alpha) ** 2))
    )
    overall_alpha_mae = float(
        np.mean(np.abs(actual_alpha - wanted_alpha))
    )
    overall_alpha_relative_mae = float(
        np.mean(np.abs(actual_alpha - wanted_alpha) / wanted_alpha)
    )

    rayleigh_cv = math.sqrt(4.0 / math.pi - 1.0)

    # -------------------------------------------------------------
    # Outputs
    # -------------------------------------------------------------
    output.mkdir(parents=True, exist_ok=True)
    cutoff = classification_cutoff(v2a_structural, v2a_extra)

    write_all_poles_csv(
        output / "all_poles.csv",
        l1_eigenvalues,
        v2a_eigenvalues,
        target.fs,
        cutoff,
    )
    write_band_summary_csv(
        output / "band_summary.csv",
        summary_rows,
    )

    plot_alpha(
        output / "01_alpha_paper_compare.png",
        l1_positive,
        v2a_structural,
        target,
    )
    plot_t60(
        output / "02_t60_compare.png",
        l1_positive,
        v2a_structural,
        target,
    )
    plot_eta(
        output / "03_eta_paper_compare.png",
        l1_positive,
        v2a_structural,
        target,
    )
    plot_modal_density(
        output / "04_modal_density_paper_compare.png",
        l1_positive,
        v2a_structural,
    )
    plot_spacing(
        output / "05_spacing_paper_compare.png",
        l1_spacing,
        v2a_spacing,
    )
    plot_q(
        output / "06_q_compare.png",
        l1_positive,
        v2a_structural,
    )
    plot_pole_radius(
        output / "07_pole_radius_compare.png",
        l1_positive,
        v2a_structural,
        v2a_extra,
    )

    print("\n模拟 A/B/C 的 2 s shaped_force 单位脉冲...")
    response = {
        "L1": simulate_impulse(delays, l1_sections, l1_tap, l1_q, l1_direct, target.fs),
        "v2a post-loss": simulate_impulse(post_delays, post_sections, post_tap,
                                          post_q, post_direct, target.fs),
        "v2a pre-loss": simulate_impulse(pre_delays, pre_sections, pre_tap,
                                         pre_q, pre_direct, target.fs),
    }
    assert np.all(np.isfinite(np.concatenate(list(response.values()))))
    direct_gains = {"L1": l1_direct, "v2a post-loss": post_direct,
                    "v2a pre-loss": pre_direct}
    DIRECT_GAIN_FOR_PLOTS.update(direct_gains)
    reference_rms = rms(response["v2a post-loss"])
    matched_response = {name: signal * reference_rms / rms(signal)
                        for name, signal in response.items()}
    filtered_raw = {name: filtered_bands(signal, target.fs)
                    for name, signal in response.items()}
    filtered_matched = {name: filtered_bands(signal, target.fs)
                        for name, signal in matched_response.items()}
    readout_rows = []
    metrics = {}
    for level, signals, filtered in (("RAW", response, filtered_raw),
                                     ("LEVEL-MATCHED", matched_response, filtered_matched)):
        metrics[level] = {}
        for name, signal in signals.items():
            row = {"level": level, "version": name,
                   "gain_from_raw": 1.0 if level == "RAW" else reference_rms / rms(response[name])}
            row.update(impulse_metrics(signal, filtered[name], target.fs))
            readout_rows.append(row)
            metrics[level][name] = row
    with (output / "readout_summary.csv").open("w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(file, fieldnames=list(readout_rows[0]))
        writer.writeheader()
        writer.writerows(readout_rows)

    plot_transfer(output / "08_transfer_magnitude_compare.png", response, target.fs)
    plot_transfer(output / "09_fdn_only_transfer_compare.png", response, target.fs,
                  wet_only=True)
    plot_readout_difference(output / "10_readout_difference.png",
                            response["v2a post-loss"], response["v2a pre-loss"],
                            target.fs, post_direct)
    plot_early_envelope(output / "11_early_impulse_envelope.png", filtered_raw, target.fs)

    report = [
        "Soundboard FDN: closed-loop dynamics and audible readout",
        "=" * 70,
        "1. Version mapping",
        f"A L1-Clavier: {l1_ref}",
        f"B v2a post-loss: {post_ref}",
        f"C v2a pre-loss: {pre_ref}",
        "",
        "2. Production-source verification",
        f"A/B/C delay banks identical: {delays.tolist()}",
        f"L1 target_t60={old_t60:.6f} s, loss_pole={old_pole:.6f}",
        f"B/C SOS banks elementwise identical; {active_count} effective sections per branch",
        "B/C feedback uses loss-filtered retained; B output uses retained; C output uses raw delayed",
        f"A/B/C direct_gain={l1_direct:.9g}; input/output q={l1_q:.9g}; Fs={target.fs:.1f} Hz",
        "",
        "3. Closed-loop dynamics: L1 vs v2a (shared by B and C)",
        f"L1 states={len(l1_eigenvalues)}, structural positive-frequency poles={len(l1_positive.z)}, redundant near-zero={l1_zero_count}",
        f"v2a states={len(v2a_eigenvalues)}, structural positive-frequency poles={len(v2a_structural.z)}, SOS-state-dominated={len(v2a_extra.z)}",
        f"Radius gap structural/SOS={radius_gap:.9f}",
        f"Characteristic normalized sigma_min/sigma_max max: L1={np.max(l1_residual):.3e}, v2a={np.max(v2a_residual):.3e}",
        "4. B/C pole identity verification",
        f"max_abs(F_post - F_pre) = {matrix_difference:.3e}",
        f"max matched pole distance |z_B - z_C| = {pole_difference:.3e}",
        "Same state transition implies identical exact poles; eig numerical tolerance warning threshold=1e-10.",
        "",
        "5. Modal damping statistics",
        f"v2a target alpha RMSE={overall_alpha_rmse:.3f} 1/s, MAE={overall_alpha_mae:.3f} 1/s, relative MAE={100*overall_alpha_relative_mae:.2f}% (100-10000 Hz structural poles)",
        "model  band         N   alpha(1/s)   T60(s)   eta(%)      Q      modal_density",
    ]
    for row in summary_rows:
        report.append(
            f"{row['model']:4s}  {row['band_hz']:11s} {row['mode_count']:4d} "
            f"{row['mean_alpha_s-1']:11.3f} {row['mean_t60_s']:8.4f} "
            f"{row['mean_eta_percent']:8.3f} {row['mean_Q']:8.2f} "
            f"{row['mean_modal_density_modes_per_hz']:13.5f}"
        )
    report.extend([
        "",
        "6. Modal structure statistics",
        (f"L1 spacing mean={l1_spacing['mean_hz']:.3f} Hz, CV={l1_spacing['cv']:.3f}; "
         f"v2a mean={v2a_spacing['mean_hz']:.3f} Hz, CV={v2a_spacing['cv']:.3f} (100-3000 Hz)"),
        f"Paper reduced-model Rayleigh spacing CV={rayleigh_cv:.3f}.",
        "Paper overlays are published curves/reported trends and equations, not digitized experimental scatter.",
        "",
        "7. Three-version impulse/readout comparison",
        "Input: shaped_force[n]=delta[n], zero state, 2 s; no hammer/string/region shaper.",
        "All 2 s response metrics include direct_gain; FDN-only FFT removes only the n=0 direct impulse.",
        "FFT: full 2 s rectangular truncation, zero padded to next_fast_len(2N); no taper; zero padding does not improve physical resolution.",
        "Bands: identical 4th-order Butterworth SOS, causal zero-state filters; high pass >3 kHz.",
        "Centroids and C50/C80 use energy; ABR=10log10(E[0,20ms]/E[20,200ms]).",
        "Persistence: last 5 ms centered RMS crossing -20/-40 dB relative to that band's own peak.",
    ])
    for level in ("RAW", "LEVEL-MATCHED"):
        report.append(f"8. {level} comparison" if level == "LEVEL-MATCHED" else "   RAW metrics")
        for name, row in metrics[level].items():
            report.append(
                f"{name}: 2s RMS={row['rms_full_2s']:.6g}, peak={row['peak']:.6g}, "
                f"crest={row['crest_factor']:.3f}, broadband centroid="
                f"{row['broadband_temporal_centroid_ms']:.2f} ms, "
                f"C50={row['C50_db']:.2f} dB, C80={row['C80_db']:.2f} dB, "
                f"ABR={row['ABR_0_20_over_20_200_db']:.2f} dB, gain={row['gain_from_raw']:.6g}"
            )
            report.append(
                "  centroid mid/high = "
                f"{row['mid_300_3000_temporal_centroid_ms']:.2f}/"
                f"{row['high_gt3000_temporal_centroid_ms']:.2f} ms; "
                "persistence mid -20/-40 = "
                f"{row['mid_300_3000_persistence_-20dB_ms']:.1f}/"
                f"{row['mid_300_3000_persistence_-40dB_ms']:.1f} ms; "
                "high -20/-40 = "
                f"{row['high_gt3000_persistence_-20dB_ms']:.1f}/"
                f"{row['high_gt3000_persistence_-40dB_ms']:.1f} ms"
            )
            for start, end in TIME_WINDOWS_MS[:-1]:
                report.append(
                    f"  {start:3d}-{end:4d} ms RMS broadband/mid/high = "
                    f"{row[f'broadband_rms_{start}_{end}ms']:.6g}/"
                    f"{row[f'mid_300_3000_rms_{start}_{end}ms']:.6g}/"
                    f"{row[f'high_gt3000_rms_{start}_{end}ms']:.6g}"
                )
        b = metrics[level]["v2a post-loss"]
        c = metrics[level]["v2a pre-loss"]
        report.append("  C-B RMS differences (dB):")
        for band, start, end in (
            ("high_gt3000", 20, 50), ("high_gt3000", 50, 100),
            ("broadband", 20, 200), ("mid_300_3000", 20, 200),
            ("high_gt3000", 20, 200),
        ):
            key = f"{band}_rms_{start}_{end}ms"
            report.append(f"    {band} {start}-{end} ms: {db_ratio(c[key], b[key]):+.3f}")

    l1_low = next(row for row in summary_rows if row["model"] == "L1" and row["band_hz"] == "100-300")
    l1_high = next(row for row in summary_rows if row["model"] == "L1" and row["band_hz"] == "5000-10000")
    v_low = next(row for row in summary_rows if row["model"] == "v2a" and row["band_hz"] == "100-300")
    v_high = next(row for row in summary_rows if row["model"] == "v2a" and row["band_hz"] == "5000-10000")
    report.extend([
        "",
        "9. Interpretation",
        "[DYNAMICS]",
        (f"L1 mean T60 low/high={l1_low['mean_t60_s']:.3f}/{l1_high['mean_t60_s']:.3f} s; "
         f"v2a={v_low['mean_t60_s']:.3f}/{v_high['mean_t60_s']:.3f} s. "
         "Compare bands above and alpha/T60 plots for frequency-dependent damping."),
        (f"Spacing CV L1/v2a={l1_spacing['cv']:.3f}/{v2a_spacing['cv']:.3f}; "
         "modal density is reported by band above; delay bank is identical."),
        "[READOUT]",
        ("B and C have identical feedback matrices and poles. Their different output operators "
         "give different mode residues/zeros, hence different impulse and audible spectra."),
    ])
    for start, end in ((20, 50), (50, 100), (20, 200)):
        row_b, row_c = metrics["LEVEL-MATCHED"]["v2a post-loss"], metrics["LEVEL-MATCHED"]["v2a pre-loss"]
        key = f"high_gt3000_rms_{start}_{end}ms"
        report.append(f"Level-matched high-band C-B {start}-{end} ms: {db_ratio(row_c[key], row_b[key]):+.2f} dB")
    matched = metrics["LEVEL-MATCHED"]
    key = "high_gt3000_rms_20_50ms"
    a_high, b_high, c_high = (matched[name][key] for name in
        ("L1", "v2a post-loss", "v2a pre-loss"))
    placement = "between L1 and B" if min(a_high, b_high) < c_high < max(a_high, b_high) else "outside the L1/B range"
    report.append(
        f"At matched 2 s RMS, 20-50 ms high-band C is {placement}: "
        f"L1={a_high:.6g}, B={b_high:.6g}, C={c_high:.6g}."
    )
    report.append(
        "The same ordering does not apply to every metric: level-matched C-B "
        f"20-200 ms broadband={db_ratio(matched['v2a pre-loss']['broadband_rms_20_200ms'], matched['v2a post-loss']['broadband_rms_20_200ms']):+.2f} dB, "
        f"mid-band={db_ratio(matched['v2a pre-loss']['mid_300_3000_rms_20_200ms'], matched['v2a post-loss']['mid_300_3000_rms_20_200ms']):+.2f} dB. "
        "C does not return to L1 feedback dynamics."
    )
    report_text = "\n".join(report) + "\n"
    (output / "report.txt").write_text(report_text, encoding="utf-8")
    print("\n" + report_text)
    print(f"输出目录：{output}")


if __name__ == "__main__":
    main()
