"""
bBpiano Soundboard Fancy — search_fdn_structure.py
==================================================

用途
----
v2b modal-structure 搜索器：只搜索 8 条 FDN delay lengths，
冻结当前 shifted-Householder feedback topology。Stage A 优化 modal
statistics 并加入轻量 delay-geometry 工程 guardrail；Stage B 只对前 100
个候选做 lossless impulse / temporal diffusion 可行性检查。

它回答一个非常具体的问题：

    单靠重新选择 8 个 delay lengths，
    能不能让当前过度规则的 modal spacing 更接近
    modal_structure_target.py 定义的统计目标？

这个脚本不改 production，不改 damping，不改 readout，也不改 A matrix。

为什么搜索时先忽略 v2a damping？
----------------------------
当前实验已经确认：

1. v2a damping 主要改变 closed-loop pole radius；
2. L1 → v2a 的 pole angles / modal density / spacing 变化很小；
3. modal structure 的第一版搜索目标是 pole angles。

因此本脚本在搜索阶段使用 lossless / unattenuated FDN 的 closed-loop
characteristic equation 来得到 modal frequencies。

这样每个候选不需要对 2800+ 维 state matrix 反复做 eig。

对当前 production 的 shifted-Householder：

    feedback[i]
        = delayed[(i + 1) % 8]
        - 0.25 * sum(delayed)

其 characteristic equation：

    det[I - D(z) A] = 0

在 lossless 情况：

    H_i(z) = 1
    z = exp(jω)

时，可以化成一个实的 trigonometric polynomial：

    G(ω)
      = 2 cos(ω T/2)
        + 1/4 Σ_{m=1..7} Σ_{i=0..7}
          cos(ω (T/2 - S_{i,m}))

其中：

    T = Σ_i L_i

    S_{i,m}
        = 从 branch i 开始
          连续 m 条 cyclic delays 的长度和

因此可以在频率轴上快速找 G(ω)=0，
得到 lossless closed-loop structural modal frequencies。

重要限制
--------
- 这不是最终 production design。
- 当前搜索只改变 delay lengths。
- feedback matrix A 仍然是原来的 shifted Householder。
- 当前搜索不试图复刻 >1.1 kHz 的 position-dependent localization。
- 找到候选后必须重新：
    1. 根据新 delay lengths 生成 v2a damping coefficients；
    2. 用 validate_fdn_poles.py 做 exact damped closed-loop 验证；
    3. render / A-B 试听。

运行
----
在 lab/soundboard-fancy/v2b/ 下：

    ../.venv/bin/python search_fdn_structure.py

更充分搜索：

    ../.venv/bin/python search_fdn_structure.py \
        --global-candidates 5000 \
        --local-candidates 3000 \
        --temporal-shortlist 100

输出
----
    output/structure_search/
        report.txt
        top_candidates.csv
        best_candidate.json
        01_modal_density_compare.png
        02_spacing_compare.png
        03_delay_sets.png
        04_echo_density_compare.png
        05_search_tradeoff.png

依赖
----
    numpy
    scipy
    matplotlib
    modal_structure_target.py
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import re
import subprocess
from dataclasses import dataclass
from pathlib import Path

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import OptimizeResult, brentq, minimize_scalar

import modal_structure_target as target  # pyright: ignore[reportImplicitRelativeImport]

# ============================================================================
# 1. 默认工程约束
# ============================================================================

SOURCE_PATH = (
    "core/bBpiano Lite/core/bbpl/piano/soundboard_model.hpp"
)

BRANCH_COUNT = 8

# 第一版只在与当前工程同量级的 delay 范围搜索。
DEFAULT_MIN_DELAY = 25
DEFAULT_MAX_DELAY = 780

# 防止多条 delay 几乎堆在一起。
# 这是工程约束，不是物理定律。
DEFAULT_MIN_GAP = 20

# 搜索阶段 root grid。
SEARCH_ROOT_STEP_HZ = 0.75

# 最终 shortlist 用更细的 root grid 重算。
FINAL_ROOT_STEP_HZ = 0.25

TOP_K = 20
ELITE_COUNT = 24
TEMPORAL_SHORTLIST = 100
NED_GAUSSIAN_REFERENCE = 0.31731
NED_HOP_MS = 1.0

# 用户给出的上一轮聚集候选只作为独立对照，不进入新搜索或排序。
DEFAULT_HISTORICAL_DELAYS = (189, 209, 248, 339, 359, 379, 416, 507)


# ============================================================================
# 2. optimizer score 的归一尺度
#
# 注意：
# 这些是 search policy，
# 不属于 modal_structure_target.py 中的物理 target。
# ============================================================================

DENSITY_SCALE = 0.010
CV_ERROR_SCALE = 0.10
CDF_DISTANCE_SCALE = 0.10
QUANTILE_RMSE_SCALE = 0.15

WEIGHT_DENSITY = 1.0
WEIGHT_CV = 1.0
WEIGHT_CDF = 1.0
WEIGHT_QUANTILE = 1.0

# 仅对越过宽松阈值的 geometry 候选施加轻量、单侧的工程惩罚。
WEIGHT_SPREAD_GUARDRAIL = 1.0
WEIGHT_PROXIMITY_GUARDRAIL = 1.0


# ============================================================================
# 3. 数据结构
# ============================================================================

@dataclass
class Candidate:
    delays: np.ndarray
    score: float
    modal_score: float
    metrics: dict[str, float]
    tag: str = "search"
    temporal: TemporalMetrics | None = None


@dataclass(frozen=True)
class GeometryReference:
    sigma: float
    cv_floor_ratio: float
    proximity_max_ratio: float
    current_cv: float
    current_proximity: float


@dataclass
class TemporalMetrics:
    time_ms: np.ndarray
    ned: np.ndarray
    kurtosis: np.ndarray
    median_ned_20_80: float
    mean_ned_20_80: float
    time_to_ned075_ms: float
    kurtosis_error_20_80: float
    passed: bool | None = None
    failure_reasons: tuple[str, ...] = ()


@dataclass(frozen=True)
class TemporalGuardrails:
    duration_ms: float
    window_ms: float
    ned_floor_ratio: float
    ned_time_max_ratio: float
    kurtosis_max_ratio: float


# ============================================================================
# 4. Git / production source
# ============================================================================

def repo_root() -> Path:
    result = subprocess.run(
        ["git", "rev-parse", "--show-toplevel"],
        check=True,
        capture_output=True,
        text=True,
    )
    return Path(result.stdout.strip())


def git_show(
    root: Path,
    ref: str,
    path: str,
) -> str:
    result = subprocess.run(
        ["git", "show", f"{ref}:{path}"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout


def resolve_ref(
    root: Path,
    ref: str,
) -> str:
    result = subprocess.run(
        ["git", "rev-parse", ref],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


def initializer_block(
    source: str,
    token: str,
) -> str:
    token_pos = source.find(token)

    if token_pos < 0:
        raise RuntimeError(
            f"源码中找不到 {token!r}"
        )

    # C++ 同时允许 kDelayLengths{...} 与 kDelayLengths = {...}。
    brace_pos = source.find("{", token_pos + len(token))
    semicolon_pos = source.find(";", token_pos + len(token))

    if brace_pos < 0 or (semicolon_pos >= 0 and semicolon_pos < brace_pos):
        raise RuntimeError(
            f"无法解析 {token!r}"
        )

    depth = 0

    for index in range(brace_pos, len(source)):
        char = source[index]

        if char == "{":
            depth += 1

        elif char == "}":
            depth -= 1

            if depth == 0:
                return source[brace_pos:index + 1]

    raise RuntimeError(
        f"{token!r} initializer 大括号不闭合"
    )


def parse_delays(
    source: str,
) -> np.ndarray:
    block = initializer_block(
        source,
        "kDelayLengths",
    )

    values = [
        int(x)
        for x in re.findall(
            r"\b\d+\b",
            block,
        )
    ]

    if len(values) != BRANCH_COUNT:
        raise RuntimeError(
            "kDelayLengths 应有 "
            f"{BRANCH_COUNT} 个值，"
            f"实际得到 {values}"
        )

    return np.asarray(
        values,
        dtype=int,
    )


def verify_lossless_runtime_topology(source: str) -> None:
    """源码若偏离本脚本复刻的拓扑，拒绝给出误导性的 diffusion 结果。"""
    code = re.sub(r"/\*.*?\*/|//[^\n]*", "", source, flags=re.DOTALL)
    code = re.sub(r"\s+", "", code)
    required = (
        "delayed[i]=delay_lines_[i].read();",
        "retained[i]=loss_filters_[i].process(delayed[i]);",
        "retained_sum+=retained[i];",
        "constexprfloatq=0.3535533905932738f;",
        "0.25f*retained_sum",
        "(i+1)&7",
        "retained[shifted]-common",
        "delay_lines_[i].write(feedback+input);",
        "q*shaped_force",
        ("q*(delayed[0]-delayed[1]+delayed[2]-delayed[3]"
         "+delayed[4]-delayed[5]+delayed[6]-delayed[7])"),
    )
    missing = [fragment for fragment in required if fragment not in code]
    if missing:
        raise RuntimeError("Git source FDN topology 与 lossless simulator 不一致: "
                           + ", ".join(missing))


# ============================================================================
# 5. lossless shifted-Householder characteristic equation
# ============================================================================

def cyclic_segment_sums(
    delays: np.ndarray,
) -> np.ndarray:
    """
    返回所有：

        m = 1 ... N-1
        i = 0 ... N-1

    的 cyclic consecutive delay sums：

        S_{i,m}

    N=8 时共 56 个。
    """

    delays = np.asarray(
        delays,
        dtype=int,
    )

    n = len(delays)

    sums = []

    for width in range(1, n):

        for start in range(n):

            value = sum(
                int(
                    delays[
                        (start + offset) % n
                    ]
                )
                for offset in range(width)
            )

            sums.append(value)

    return np.asarray(
        sums,
        dtype=float,
    )


def characteristic_parameters(
    delays: np.ndarray,
) -> tuple[float, np.ndarray]:

    total = float(
        np.sum(delays)
    )

    segment_sums = (
        cyclic_segment_sums(delays)
    )

    offsets = (
        total / 2.0
        - segment_sums
    )

    return total, offsets


def characteristic_real(
    frequency_hz: np.ndarray | float,
    total_delay: float,
    offsets: np.ndarray,
) -> np.ndarray:
    """
    lossless closed-loop characteristic：

        G(ω)

    root(G)=0 对应单位圆上的 structural poles。
    """

    frequency = np.asarray(
        frequency_hz,
        dtype=float,
    )

    omega = (
        2.0
        * math.pi
        * frequency
        / target.FS
    )

    flat = omega.reshape(-1)

    values = (
        2.0
        * np.cos(
            flat * total_delay / 2.0
        )
        +
        0.25
        * np.sum(
            np.cos(
                np.outer(
                    flat,
                    offsets,
                )
            ),
            axis=1,
        )
    )

    return values.reshape(
        omega.shape
    )


def modal_frequencies_lossless(
    delays: np.ndarray,
    low_hz: float = target.ANALYSIS_LOW_HZ,
    high_hz: float = target.ANALYSIS_HIGH_HZ,
    grid_step_hz: float = SEARCH_ROOT_STEP_HZ,
    detect_tangent_roots: bool = False,
) -> np.ndarray:
    """
    求 analysis band 内全部 lossless structural modal frequencies。

    主路径：
        sign change + Brent root finding

    final shortlist：
        可额外检查 tangent / near-double roots。
    """

    delays = np.asarray(
        delays,
        dtype=int,
    )

    total_delay, offsets = (
        characteristic_parameters(delays)
    )

    grid = np.arange(
        low_hz,
        high_hz + grid_step_hz,
        grid_step_hz,
        dtype=float,
    )

    values = characteristic_real(
        grid,
        total_delay,
        offsets,
    )

    def scalar_function(
        frequency: float,
    ) -> float:

        omega = (
            2.0
            * math.pi
            * frequency
            / target.FS
        )

        return float(
            2.0
            * math.cos(
                omega
                * total_delay
                / 2.0
            )
            +
            0.25
            * np.sum(
                np.cos(
                    omega
                    * offsets
                )
            )
        )

    roots: list[float] = []

    # ------------------------------------------------------------
    # simple roots
    # ------------------------------------------------------------

    sign_change = np.flatnonzero(
        values[:-1]
        * values[1:]
        < 0.0
    )

    for index in sign_change:

        root = brentq(
            scalar_function,
            float(grid[index]),
            float(grid[index + 1]),
            xtol=1e-11,
            rtol=np.float64(1e-13),
            maxiter=50,
        )

        if isinstance(root, tuple):
            root = root[0]
        roots.append(float(root))

    # ------------------------------------------------------------
    # optional tangent root detection
    # ------------------------------------------------------------

    if detect_tangent_roots:

        absolute = np.abs(values)

        local_min = (
            np.flatnonzero(
                (absolute[1:-1] < absolute[:-2])
                &
                (absolute[1:-1] < absolute[2:])
            )
            + 1
        )

        for index in local_min:

            if absolute[index] > 2e-2:
                continue

            left = float(
                grid[index - 1]
            )

            right = float(
                grid[index + 1]
            )

            result = minimize_scalar(
                lambda f: abs(
                    scalar_function(
                        float(f)
                    )
                ),
                bounds=(
                    left,
                    right,
                ),
                method="bounded",
                options={
                    "xatol": 1e-10,
                },
            )

            if not isinstance(result, OptimizeResult):
                raise TypeError("minimize_scalar 未返回优化结果")

            if result.fun < 1e-8:

                roots.append(
                    float(result.x)
                )

    if not roots:

        return np.array(
            [],
            dtype=float,
        )

    sorted_roots = np.sort(
        np.asarray(
            roots,
            dtype=float,
        )
    )

    # 数值重复 root 合并
    unique = [float(sorted_roots[0])]

    for value in sorted_roots[1:]:

        if (
            abs(
                value
                - unique[-1]
            )
            > 1e-5
        ):
            unique.append(float(value))

    return np.asarray(
        unique,
        dtype=float,
    )


# ============================================================================
# 6. target total delay
# ============================================================================

def target_total_delay_samples() -> int:
    """
    dense lossless FDN 中：

        mean modal density
        ≈ ΣL / Fs

    target density：

        0.06 modes/Hz

    所以：

        ΣL ≈ 0.06 * 44100
           ≈ 2646 samples
    """

    return round(
            target.DEFAULT_TARGET
            .low_freq_modal_density
            * target.FS
    )


# ============================================================================
# 7. candidate constraint
# ============================================================================

def valid_delay_vector(
    delays: np.ndarray,
    min_delay: int,
    max_delay: int,
    min_gap: int,
    target_sum: int,
) -> bool:

    delays = np.asarray(
        delays,
        dtype=int,
    )

    if len(delays) != BRANCH_COUNT:
        return False

    if not np.all(
        np.diff(delays)
        >= min_gap
    ):
        return False

    if delays[0] < min_delay:
        return False

    if delays[-1] > max_delay:
        return False

    return int(np.sum(delays)) == target_sum


def repair_delay_vector(
    raw: np.ndarray,
    rng: np.random.Generator,
    min_delay: int,
    max_delay: int,
    min_gap: int,
    target_sum: int,
) -> np.ndarray | None:
    """
    将 floating candidate 投影回：

    - 8 个递增 integer delays
    - 固定 ΣL
    - minimum gap
    - min/max bounds
    """

    raw = np.sort(
        np.asarray(
            raw,
            dtype=float,
        )
    )

    if len(raw) != BRANCH_COUNT:
        return None

    if not np.all(
        np.isfinite(raw)
    ):
        return None

    if np.sum(raw) <= 0.0:
        return None

    # ------------------------------------------------------------
    # scale to target total
    # ------------------------------------------------------------

    scaled = (
        raw
        * (
            target_sum
            / np.sum(raw)
        )
    )

    delays = np.rint(
        scaled
    ).astype(int)

    # ------------------------------------------------------------
    # enforce minimum delay / gap
    # ------------------------------------------------------------

    delays[0] = max(
        delays[0],
        min_delay,
    )

    for index in range(
        1,
        BRANCH_COUNT,
    ):

        delays[index] = max(
            delays[index],
            delays[index - 1]
            + min_gap,
        )

    if delays[-1] > max_delay:
        return None

    # ------------------------------------------------------------
    # fix exact total sum
    # ------------------------------------------------------------

    difference = int(
        target_sum
        - np.sum(delays)
    )

    for _ in range(20_000):

        if difference == 0:
            break

        # --------------------------------------------------------
        # sum too small → increment one legal delay
        # --------------------------------------------------------

        if difference > 0:

            valid = [
                index
                for index
                in range(BRANCH_COUNT)
                if (
                    delays[index]
                    < max_delay
                )
                and (
                    index
                    == BRANCH_COUNT - 1
                    or
                    delays[index] + 1
                    <=
                    delays[index + 1]
                    - min_gap
                )
            ]

            if not valid:
                return None

            index = int(
                rng.choice(valid)
            )

            delays[index] += 1

            difference -= 1

        # --------------------------------------------------------
        # sum too large → decrement one legal delay
        # --------------------------------------------------------

        else:

            valid = [
                index
                for index
                in range(BRANCH_COUNT)
                if (
                    delays[index]
                    > min_delay
                )
                and (
                    index == 0
                    or
                    delays[index] - 1
                    >=
                    delays[index - 1]
                    + min_gap
                )
            ]

            if not valid:
                return None

            index = int(
                rng.choice(valid)
            )

            delays[index] -= 1

            difference += 1

    if difference != 0:
        return None

    if not valid_delay_vector(
        delays,
        min_delay,
        max_delay,
        min_gap,
        target_sum,
    ):
        return None

    return delays


def scaled_current_candidate(
    current: np.ndarray,
    rng: np.random.Generator,
    min_delay: int,
    max_delay: int,
    min_gap: int,
    target_sum: int,
) -> np.ndarray:

    repaired = repair_delay_vector(
        current.astype(float),
        rng,
        min_delay,
        max_delay,
        min_gap,
        target_sum,
    )

    if repaired is None:

        raise RuntimeError(
            "无法把 current delay bank "
            "投影到 target total delay"
        )

    return repaired


# ============================================================================
# 8. metrics / score
# ============================================================================

def score_from_metrics(
    metrics: dict[str, float],
) -> float:
    """
    这个 score 只是搜索器内部的 objective。

    不是：
        - perceptual score
        - sound quality score
        - physical validity score
    """

    density = (
        metrics[
            "density_rmse_modes_per_hz"
        ]
        / DENSITY_SCALE
    )

    cv = (
        metrics[
            "spacing_cv_error"
        ]
        / CV_ERROR_SCALE
    )

    cdf = (
        metrics[
            "spacing_cdf_distance"
        ]
        / CDF_DISTANCE_SCALE
    )

    quantile = (
        metrics[
            "spacing_quantile_rmse"
        ]
        / QUANTILE_RMSE_SCALE
    )

    return float(
        WEIGHT_DENSITY
        * density**2
        +
        WEIGHT_CV
        * cv**2
        +
        WEIGHT_CDF
        * cdf**2
        +
        WEIGHT_QUANTILE
        * quantile**2
    )


def delay_geometry(delays: np.ndarray, sigma: float) -> dict[str, float]:
    """连续 delay 几何诊断；不把当前 production 当作物理目标。"""
    values = np.asarray(delays, dtype=float)
    distances = np.abs(values[:, None] - values[None, :])
    pairs = distances[np.triu_indices(BRANCH_COUNT, 1)]
    return {
        "delay_cv": float(np.std(values) / np.mean(values)),
        "delay_span": float(np.max(values) - np.min(values)),
        "delay_ratio": float(np.max(values) / np.min(values)),
        "proximity_penalty": float(np.mean(np.exp(-(pairs / sigma) ** 2))),
    }


def geometry_reference(
    current: np.ndarray, sigma: float, cv_floor_ratio: float,
    proximity_max_ratio: float,
) -> GeometryReference:
    metrics = delay_geometry(current, sigma)
    return GeometryReference(
        sigma, cv_floor_ratio, proximity_max_ratio,
        metrics["delay_cv"], metrics["proximity_penalty"],
    )


def evaluate_candidate(
    delays: np.ndarray,
    geometry: GeometryReference,
    root_step_hz: float = SEARCH_ROOT_STEP_HZ,
    detect_tangent_roots: bool = False,
    tag: str = "search",
) -> Candidate:

    modes = modal_frequencies_lossless(
        delays,
        grid_step_hz=root_step_hz,
        detect_tangent_roots=(
            detect_tangent_roots
        ),
    )

    metrics = (
        target.objective_components(
            modes
        )
    )

    modal_score = score_from_metrics(metrics)
    metrics.update(delay_geometry(delays, geometry.sigma))
    cv_floor = geometry.cv_floor_ratio * geometry.current_cv
    prox_ceiling = geometry.proximity_max_ratio * geometry.current_proximity
    metrics["spread_guardrail"] = max(
        0.0, (cv_floor - metrics["delay_cv"]) / max(cv_floor, 1e-12)
    ) ** 2
    metrics["proximity_guardrail"] = max(
        0.0,
        (metrics["proximity_penalty"] - prox_ceiling)
        / max(prox_ceiling, 1e-12),
    ) ** 2
    score = (modal_score
             + WEIGHT_SPREAD_GUARDRAIL * metrics["spread_guardrail"]
             + WEIGHT_PROXIMITY_GUARDRAIL * metrics["proximity_guardrail"])

    return Candidate(
        delays=np.asarray(
            delays,
            dtype=int,
        ),
        score=score,
        modal_score=modal_score,
        metrics=metrics,
        tag=tag,
    )


# ============================================================================
# 9. search
# ============================================================================

def candidate_key(
    delays: np.ndarray,
) -> tuple[int, ...]:

    return tuple(
        int(value)
        for value in delays
    )


def run_search(
    current_delays: np.ndarray,
    geometry: GeometryReference,
    temporal_shortlist: int,
    seed: int,
    global_candidates: int,
    local_candidates: int,
    min_delay: int,
    max_delay: int,
    min_gap: int,
) -> tuple[
    list[Candidate],
    Candidate,
    Candidate,
]:

    rng = np.random.default_rng(
        seed
    )

    target_sum = (
        target_total_delay_samples()
    )

    # ------------------------------------------------------------
    # current production baseline
    # ------------------------------------------------------------

    current_eval = evaluate_candidate(
        current_delays,
        geometry,
        root_step_hz=FINAL_ROOT_STEP_HZ,
        detect_tangent_roots=True,
        tag="current-production",
    )

    # ------------------------------------------------------------
    # current shape scaled to density target total
    # ------------------------------------------------------------

    scaled = scaled_current_candidate(
        current_delays,
        rng,
        min_delay,
        max_delay,
        min_gap,
        target_sum,
    )

    scaled_eval = evaluate_candidate(
        scaled,
        geometry,
        root_step_hz=FINAL_ROOT_STEP_HZ,
        detect_tangent_roots=True,
        tag="scaled-current",
    )

    seen = {
        candidate_key(
            current_delays
        ),
        candidate_key(
            scaled
        ),
    }

    pool: list[Candidate] = [
        scaled_eval
    ]

    # ========================================================================
    # Phase A
    #
    # global-ish random search
    # ========================================================================

    base = scaled.astype(float)

    accepted = 0
    attempts = 0

    max_attempts = (
        global_candidates
        * 20
    )

    while (
        accepted
        < global_candidates
        and
        attempts
        < max_attempts
    ):

        attempts += 1

        # --------------------------------------------------------
        # 大部分 candidate：
        # 从 current geometry 附近做 multiplicative perturbation
        # --------------------------------------------------------

        if rng.random() < 0.72:

            sigma = float(
                rng.uniform(
                    0.08,
                    0.55,
                )
            )

            raw = (
                base
                * np.exp(
                    rng.normal(
                        0.0,
                        sigma,
                        size=BRANCH_COUNT,
                    )
                )
            )

        # --------------------------------------------------------
        # 少部分 candidate：
        # broad random exploration
        # --------------------------------------------------------

        else:

            raw = np.sort(
                rng.uniform(
                    min_delay,
                    max_delay,
                    size=BRANCH_COUNT,
                )
            )

        delays = repair_delay_vector(
            raw,
            rng,
            min_delay,
            max_delay,
            min_gap,
            target_sum,
        )

        if delays is None:
            continue

        key = candidate_key(
            delays
        )

        if key in seen:
            continue

        seen.add(key)

        candidate = evaluate_candidate(
            delays, geometry
        )

        pool.append(
            candidate
        )

        accepted += 1

        if accepted % 250 == 0:

            current_best = min(
                pool,
                key=lambda item: item.score,
            )

            print(
                f"[global "
                f"{accepted:5d}/"
                f"{global_candidates}] "
                f"best="
                f"{current_best.score:.4f} "
                f"CV="
                f"{current_best.metrics['spacing_cv']:.4f} "
                f"delays="
                f"{current_best.delays.tolist()}"
            )

    # ========================================================================
    # Phase B
    #
    # elite local refinement
    # ========================================================================

    pool.sort(
        key=lambda item: item.score
    )

    elites = pool[
        :ELITE_COUNT
    ]

    accepted = 0
    attempts = 0

    max_attempts = (
        local_candidates
        * 20
    )

    while (
        accepted
        < local_candidates
        and
        attempts
        < max_attempts
    ):

        attempts += 1

        parent = elites[
            int(
                rng.integers(
                    len(elites)
                )
            )
        ]

        sigma_samples = float(
            rng.uniform(
                2.0,
                40.0,
            )
        )

        raw = (
            parent.delays.astype(float)
            +
            rng.normal(
                0.0,
                sigma_samples,
                size=BRANCH_COUNT,
            )
        )

        delays = repair_delay_vector(
            raw,
            rng,
            min_delay,
            max_delay,
            min_gap,
            target_sum,
        )

        if delays is None:
            continue

        key = candidate_key(
            delays
        )

        if key in seen:
            continue

        seen.add(key)

        candidate = evaluate_candidate(
            delays, geometry
        )

        pool.append(
            candidate
        )

        accepted += 1

        # --------------------------------------------------------
        # update elites
        # --------------------------------------------------------

        combined = (
            elites
            + [candidate]
        )

        combined.sort(
            key=lambda item: item.score
        )

        elites = combined[
            :ELITE_COUNT
        ]

        if accepted % 250 == 0:

            current_best = elites[0]

            print(
                f"[local  "
                f"{accepted:5d}/"
                f"{local_candidates}] "
                f"best="
                f"{current_best.score:.4f} "
                f"CV="
                f"{current_best.metrics['spacing_cv']:.4f} "
                f"delays="
                f"{current_best.delays.tolist()}"
            )

    # ========================================================================
    # Phase C
    #
    # fine-grid recomputation
    # ========================================================================

    pool.sort(
        key=lambda item: item.score
    )

    shortlist = pool[
        :max(
            TOP_K * 4,
            temporal_shortlist + TOP_K,
            60,
        )
    ]

    refined: list[Candidate] = []

    for candidate in shortlist:

        refined_candidate = (
            evaluate_candidate(
                candidate.delays,
                geometry,
                root_step_hz=(
                    FINAL_ROOT_STEP_HZ
                ),
                detect_tangent_roots=True,
                tag=candidate.tag,
            )
        )

        refined.append(
            refined_candidate
        )

    refined.sort(
        key=lambda item: item.score
    )

    return (
        refined,
        current_eval,
        scaled_eval,
    )


def simulate_lossless_impulse(delays: np.ndarray, duration_ms: float) -> np.ndarray:
    """按 production read→feedback→write 顺序执行；仅关掉 loss/direct/shaper。"""
    count = round(duration_ms * 0.001 * target.FS)
    rings = [np.zeros(int(length), dtype=float) for length in delays]
    positions = np.zeros(BRANCH_COUNT, dtype=int)
    output = np.zeros(count, dtype=float)
    q = 1.0 / math.sqrt(BRANCH_COUNT)
    signs = np.array([1.0, -1.0] * (BRANCH_COUNT // 2))
    for n in range(count):
        delayed = np.array([rings[i][positions[i]] for i in range(BRANCH_COUNT)])
        output[n] = q * float(np.dot(signs, delayed))
        common = 0.25 * float(np.sum(delayed))
        injection = q if n == 0 else 0.0
        for i in range(BRANCH_COUNT):
            rings[i][positions[i]] = delayed[(i + 1) & 7] - common + injection
            positions[i] += 1
            if positions[i] == len(rings[i]):
                positions[i] = 0
    return output


def temporal_metrics(delays: np.ndarray, guardrails: TemporalGuardrails) -> TemporalMetrics:
    impulse = simulate_lossless_impulse(delays, guardrails.duration_ms)
    fs = target.FS
    width = max(2, round(guardrails.window_ms * 0.001 * fs))
    hop = max(1, round(NED_HOP_MS * 0.001 * fs))
    starts = np.arange(0, max(0, len(impulse) - width + 1), hop)
    times = (starts + width / 2) * 1000.0 / fs
    ned = np.full(len(starts), np.nan)
    kurtosis = np.full(len(starts), np.nan)
    for k, start in enumerate(starts):
        window = impulse[start:start + width]
        sigma = float(np.std(window))
        if sigma <= 1e-15:
            continue
        ned[k] = np.count_nonzero(np.abs(window) > sigma) / len(window) / NED_GAUSSIAN_REFERENCE
        centered = window - np.mean(window)
        kurtosis[k] = float(np.mean(centered ** 4) / sigma ** 4)
    summary = (times >= 20.0) & (times <= 80.0)
    if not np.any(np.isfinite(ned[summary])):
        raise RuntimeError("20–80 ms NED 窗口没有有效数据")
    # 10 ms 之后再寻找连续三个窗口达标，避免最早期 deterministic arrivals。
    valid = (times >= 10.0) & (ned >= 0.75)
    run = np.convolve(valid.astype(int), np.ones(3, dtype=int), mode="valid")
    reached = np.flatnonzero(run == 3)
    reached_ms = float(times[reached[0]]) if len(reached) else math.nan
    return TemporalMetrics(
        times, ned, kurtosis,
        float(np.nanmedian(ned[summary])), float(np.nanmean(ned[summary])),
        reached_ms, float(np.nanmedian(np.abs(kurtosis[summary] - 3.0))),
    )


def check_temporal(candidate: TemporalMetrics, current: TemporalMetrics,
                   rules: TemporalGuardrails) -> None:
    """只作可行性判定；不把 temporal 数值混入 modal score。"""
    reasons = []
    if candidate.median_ned_20_80 < rules.ned_floor_ratio * current.median_ned_20_80:
        reasons.append("median NED below floor")
    if math.isfinite(current.time_to_ned075_ms) and (
        not math.isfinite(candidate.time_to_ned075_ms)
        or candidate.time_to_ned075_ms > rules.ned_time_max_ratio * current.time_to_ned075_ms
    ):
        reasons.append("NED 0.75 reached too late or not reached")
    if candidate.kurtosis_error_20_80 > (
        rules.kurtosis_max_ratio * max(current.kurtosis_error_20_80, 1e-9)
    ):
        reasons.append("kurtosis error above ceiling")
    candidate.passed = not reasons
    candidate.failure_reasons = tuple(reasons)


# ============================================================================
# 10. CSV / JSON output
# ============================================================================

def candidate_row(
    rank: int,
    candidate: Candidate,
) -> dict[str, object]:

    return {
        "rank": rank,
        "tag": candidate.tag,
        "score": candidate.score,
        "modal_score": candidate.modal_score,
        "delays": " ".join(
            str(int(x))
            for x
            in candidate.delays
        ),
        "sum_delays": int(
            np.sum(
                candidate.delays
            )
        ),
        "mode_count_100_3000": (
            candidate.metrics[
                "mode_count"
            ]
        ),
        "mean_spacing_hz": (
            candidate.metrics[
                "mean_spacing_hz"
            ]
        ),
        "spacing_cv": (
            candidate.metrics[
                "spacing_cv"
            ]
        ),
        "target_spacing_cv": (
            candidate.metrics[
                "target_spacing_cv"
            ]
        ),
        "density_rmse_modes_per_hz": (
            candidate.metrics[
                "density_rmse_modes_per_hz"
            ]
        ),
        "spacing_cv_error": (
            candidate.metrics[
                "spacing_cv_error"
            ]
        ),
        "spacing_cdf_distance": (
            candidate.metrics[
                "spacing_cdf_distance"
            ]
        ),
        "spacing_quantile_rmse": (
            candidate.metrics[
                "spacing_quantile_rmse"
            ]
        ),
        "delay_cv": candidate.metrics["delay_cv"],
        "delay_span": candidate.metrics["delay_span"],
        "delay_ratio": candidate.metrics["delay_ratio"],
        "proximity_penalty": candidate.metrics["proximity_penalty"],
        "spread_guardrail": candidate.metrics["spread_guardrail"],
        "proximity_guardrail": candidate.metrics["proximity_guardrail"],
        "median_ned_20_80": (candidate.temporal.median_ned_20_80
                             if candidate.temporal else ""),
        "mean_ned_20_80": (candidate.temporal.mean_ned_20_80
                           if candidate.temporal else ""),
        "time_to_ned075_ms": (
            (candidate.temporal.time_to_ned075_ms
             if math.isfinite(candidate.temporal.time_to_ned075_ms) else None)
            if candidate.temporal else ""
        ),
        "kurtosis_error_20_80": (
            candidate.temporal.kurtosis_error_20_80 if candidate.temporal else ""
        ),
        "temporal_pass": (candidate.temporal.passed if candidate.temporal else ""),
    }


def write_top_candidates(
    path: Path,
    candidates: list[Candidate],
):

    rows = [
        candidate_row(
            rank,
            candidate,
        )
        for rank, candidate
        in enumerate(
            candidates,
            start=1,
        )
    ]

    with path.open(
        "w",
        newline="",
        encoding="utf-8",
    ) as file:

        writer = csv.DictWriter(
            file,
            fieldnames=list(
                rows[0].keys()
            ),
        )

        writer.writeheader()
        writer.writerows(rows)


def write_best_json(
    path: Path,
    best_modal: Candidate,
    best_feasible: Candidate | None,
    current: Candidate,
    scaled: Candidate,
    historical: Candidate,
    source_ref: str,
    geometry: GeometryReference,
    temporal_rules: TemporalGuardrails,
    temporal_shortlist: int,
):

    payload = {
        "format_version": 2,

        "source_ref": source_ref,

        "search_scope": (
            "delay_lengths_only"
        ),

        "feedback_matrix": (
            "production shifted-Householder unchanged"
        ),

        "damping": (
            "not included in search objective; "
            "regenerate for new delays before production"
        ),

        "best_modal_only": candidate_row(
            1,
            best_modal,
        ),

        "best_temporally_feasible": (
            candidate_row(1, best_feasible) if best_feasible else None
        ),

        "current": candidate_row(
            0,
            current,
        ),

        "scaled_current": candidate_row(
            0,
            scaled,
        ),
        "historical_cluster_control": candidate_row(0, historical),
        "guardrail_parameters": {
            "proximity_sigma": geometry.sigma,
            "delay_cv_floor_ratio": geometry.cv_floor_ratio,
            "proximity_max_ratio": geometry.proximity_max_ratio,
            "current_delay_cv": geometry.current_cv,
            "current_proximity": geometry.current_proximity,
            "temporal_shortlist": temporal_shortlist,
            "diffusion_duration_ms": temporal_rules.duration_ms,
            "ned_window_ms": temporal_rules.window_ms,
            "ned_hop_ms": NED_HOP_MS,
            "ned_floor_ratio": temporal_rules.ned_floor_ratio,
            "ned_time_max_ratio": temporal_rules.ned_time_max_ratio,
            "kurtosis_max_ratio": temporal_rules.kurtosis_max_ratio,
        },

        "notes": [
            (
                "best candidate is a modal-structure "
                "proposal, not a production-ready soundboard"
            ),
            (
                "new delays require a newly generated "
                "v2a SOS coefficient bank because "
                "damping gain scales with traversal length"
            ),
            (
                "run exact damped validation and listening "
                "tests before production integration"
            ),
        ],
    }

    path.write_text(
        json.dumps(
            payload,
            indent=2,
            ensure_ascii=False,
            allow_nan=False,
        ),
        encoding="utf-8",
    )


# ============================================================================
# 11. Plot helper
# ============================================================================

def modal_density_for_candidate(
    candidate: Candidate,
) -> tuple[
    np.ndarray,
    np.ndarray,
]:

    modes = (
        modal_frequencies_lossless(
            candidate.delays,
            grid_step_hz=(
                FINAL_ROOT_STEP_HZ
            ),
            detect_tangent_roots=True,
        )
    )

    return (
        target.apparent_modal_density(
            modes
        )
    )


def normalized_spacing_for_candidate(
    candidate: Candidate,
) -> np.ndarray:

    modes = (
        modal_frequencies_lossless(
            candidate.delays,
            grid_step_hz=(
                FINAL_ROOT_STEP_HZ
            ),
            detect_tangent_roots=True,
        )
    )

    return (
        target.normalized_spacings(
            modes
        )
    )


# ============================================================================
# 12. Plot modal density
# ============================================================================

def plot_density_compare(
    path: Path,
    current: Candidate,
    best: Candidate,
):

    current_f, current_n = (
        modal_density_for_candidate(
            current
        )
    )

    best_f, best_n = (
        modal_density_for_candidate(
            best
        )
    )

    fig, ax = plt.subplots(
        figsize=(11, 6)
    )

    ax.plot(
        current_f,
        current_n,
        linewidth=1.4,
        alpha=0.80,
        label=(
            "current "
            f"(ΣL="
            f"{int(np.sum(current.delays))})"
        ),
    )

    ax.plot(
        best_f,
        best_n,
        linewidth=1.4,
        alpha=0.85,
        label=(
            "best delay candidate "
            f"(ΣL="
            f"{int(np.sum(best.delays))})"
        ),
    )

    frequency = np.geomspace(
        target.ANALYSIS_LOW_HZ,
        target.LOCALIZATION_TRANSITION_HZ,
        400,
    )

    wanted = (
        target.target_modal_density(
            frequency
        )
    )

    ax.plot(
        frequency,
        wanted,
        linewidth=2.0,
        linestyle="--",
        label=(
            "defined low-frequency target"
        ),
    )

    ax.axvline(
        target.LOCALIZATION_TRANSITION_HZ,
        linestyle=":",
        linewidth=1.2,
        label=(
            "~1.1 kHz localization transition"
        ),
    )

    ax.set_xscale("log")

    ax.set_xlim(
        target.ANALYSIS_LOW_HZ,
        target.ANALYSIS_HIGH_HZ,
    )

    ax.set_ylim(
        0.0,
        0.11,
    )

    ax.set_xlabel(
        "Frequency (Hz)"
    )

    ax.set_ylabel(
        "Apparent modal density (modes/Hz)"
    )

    ax.set_title(
        "Modal density: current vs delay-search candidate"
    )

    ax.grid(
        True,
        which="both",
        alpha=0.25,
    )

    ax.legend()

    fig.tight_layout()

    fig.savefig(
        path,
        dpi=180,
    )

    plt.close(fig)


# ============================================================================
# 13. Plot spacing
# ============================================================================

def plot_spacing_compare(
    path: Path,
    current: Candidate,
    best: Candidate,
):

    current_x = (
        normalized_spacing_for_candidate(
            current
        )
    )

    best_x = (
        normalized_spacing_for_candidate(
            best
        )
    )

    bins = np.linspace(
        0.0,
        2.5,
        28,
    ).tolist()

    fig, ax = plt.subplots(
        figsize=(11, 6)
    )

    ax.hist(
        current_x,
        bins=bins,
        density=True,
        histtype="step",
        linewidth=1.6,
        label=(
            "current "
            f"(CV="
            f"{current.metrics['spacing_cv']:.3f})"
        ),
    )

    ax.hist(
        best_x,
        bins=bins,
        density=True,
        histtype="step",
        linewidth=1.6,
        label=(
            "best candidate "
            f"(CV="
            f"{best.metrics['spacing_cv']:.3f})"
        ),
    )

    x = np.linspace(
        0.0,
        2.5,
        800,
    )

    ax.plot(
        x,
        target.rayleigh_spacing_pdf(
            x
        ),
        linewidth=2.0,
        label=(
            "Rayleigh target "
            f"(CV="
            f"{target.RAYLEIGH_SPACING_CV:.3f})"
        ),
    )

    ax.set_xlabel(
        "Nearest-neighbor spacing / mean spacing"
    )

    ax.set_ylabel(
        "Probability density"
    )

    ax.set_title(
        "Normalized modal-spacing distribution, 100–3000 Hz"
    )

    ax.grid(
        True,
        alpha=0.25,
    )

    ax.legend()

    fig.tight_layout()

    fig.savefig(
        path,
        dpi=180,
    )

    plt.close(fig)


# ============================================================================
# 14. Plot delay sets
# ============================================================================

def plot_delay_sets(
    path: Path,
    current: Candidate,
    scaled: Candidate,
    best: Candidate,
):

    index = np.arange(
        1,
        BRANCH_COUNT + 1,
    )

    fig, ax = plt.subplots(
        figsize=(10, 5.8)
    )

    ax.plot(
        index,
        current.delays,
        marker="o",
        label="current production",
    )

    ax.plot(
        index,
        scaled.delays,
        marker="o",
        label=(
            "current shape scaled "
            "to target ΣL"
        ),
    )

    ax.plot(
        index,
        best.delays,
        marker="o",
        label=(
            "best searched candidate"
        ),
    )

    ax.set_xticks(index)

    ax.set_xlabel(
        "Delay branch index"
    )

    ax.set_ylabel(
        "Delay length (samples)"
    )

    ax.set_title(
        "FDN delay-bank comparison"
    )

    ax.grid(
        True,
        alpha=0.25,
    )

    ax.legend()

    fig.tight_layout()

    fig.savefig(
        path,
        dpi=180,
    )

    plt.close(fig)


# ============================================================================
# 15. Report
# ============================================================================

def plot_echo_density(path: Path, current: Candidate, best_modal: Candidate,
                      best_feasible: Candidate | None, historical: Candidate) -> None:
    fig, ax = plt.subplots(figsize=(11, 6))
    plotted: set[tuple[int, ...]] = set()
    for label, candidate in (
        ("current production", current),
        ("best modal-only", best_modal),
        ("best temporally feasible", best_feasible),
        ("historical cluster control", historical),
    ):
        if candidate is None or candidate.temporal is None:
            continue
        key = candidate_key(candidate.delays)
        if key in plotted:
            continue
        plotted.add(key)
        ax.plot(candidate.temporal.time_ms, candidate.temporal.ned, label=label)
    ax.axhline(0.75, color="black", ls="--", lw=1, label="NED = 0.75")
    ax.axhline(1.0, color="gray", ls=":", lw=1, label="Gaussian NED = 1")
    ax.set(xlim=(0, 150), xlabel="Time (ms)", ylabel="Normalized echo density",
           title="Lossless FDN wet impulse, 10 ms window / 1 ms hop")
    ax.grid(alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)


def plot_search_tradeoff(path: Path, candidates: list[Candidate], current: Candidate,
                         best_modal: Candidate, best_feasible: Candidate | None) -> None:
    fig, ax = plt.subplots(figsize=(10, 6))
    groups = (
        ("Stage A only", lambda c: c.temporal is None, "#bbbbbb"),
        ("temporal fail", lambda c: c.temporal is not None and c.temporal.passed is False, "#d95f02"),
        ("temporal pass", lambda c: c.temporal is not None and c.temporal.passed is True, "#1b9e77"),
    )
    for label, predicate, color in groups:
        group = [c for c in candidates if predicate(c)]
        if group:
            ax.scatter([c.metrics["spacing_cv"] for c in group],
                       [c.metrics["spacing_cdf_distance"] for c in group],
                       s=28, color=color, alpha=0.65, label=label)
    for label, candidate, marker, color in (
        ("current", current, "*", "black"),
        ("best modal-only", best_modal, "D", "#7570b3"),
        ("best feasible", best_feasible, "P", "#1b9e77"),
    ):
        if candidate is None:
            continue
        ax.scatter(candidate.metrics["spacing_cv"],
                   candidate.metrics["spacing_cdf_distance"],
                   marker=marker, s=130, color=color, edgecolors="black", label=label,
                   zorder=5)
    ax.axvline(target.RAYLEIGH_SPACING_CV, color="black", ls=":", lw=1,
               label="Rayleigh CV target")
    ax.set(xlabel="Spacing CV (higher approaches Rayleigh target)",
           ylabel="Spacing CDF distance (lower is better)",
           title="Modal-spacing tradeoff and temporal feasibility")
    ax.grid(alpha=0.25)
    ax.legend()
    fig.tight_layout()
    fig.savefig(path, dpi=180)
    plt.close(fig)

def format_metrics(
    candidate: Candidate,
) -> str:
    m = candidate.metrics
    t = candidate.temporal
    lines = [
        f"delays={candidate.delays.tolist()}  sum(L)={int(np.sum(candidate.delays))}",
        f"search_score={candidate.score:.6f}  modal_score={candidate.modal_score:.6f}",
        (f"density_RMSE={m['density_rmse_modes_per_hz']:.6f}  "
        f"spacing_CV={m['spacing_cv']:.6f}  "
        f"CDF_distance={m['spacing_cdf_distance']:.6f}  "
        f"quantile_RMSE={m['spacing_quantile_rmse']:.6f}"),
        (f"delay_CV={m['delay_cv']:.6f}  span={m['delay_span']:.0f}  "
        f"max/min={m['delay_ratio']:.3f}  proximity={m['proximity_penalty']:.6f}"),
        (f"spread_penalty={m['spread_guardrail']:.6f}  "
        f"proximity_penalty_one_sided={m['proximity_guardrail']:.6f}"),
    ]
    if t is not None:
        reached = (f"{t.time_to_ned075_ms:.2f} ms"
                   if math.isfinite(t.time_to_ned075_ms) else "not reached")
        lines.append(
            f"median_NED_20_80={t.median_ned_20_80:.6f}  "
            f"mean_NED_20_80={t.mean_ned_20_80:.6f}  "
            f"time_to_NED075={reached}  "
            f"kurtosis_error_20_80={t.kurtosis_error_20_80:.6f}  "
            f"temporal_pass={t.passed}"
        )
        if t.failure_reasons:
            lines.append("failure_reasons=" + "; ".join(t.failure_reasons))
    return "\n".join(lines)


def make_report(
    source_ref: str,
    current: Candidate,
    scaled: Candidate,
    best_modal: Candidate,
    best_feasible: Candidate | None,
    historical: Candidate,
    candidates: list[Candidate],
    geometry: GeometryReference,
    temporal_rules: TemporalGuardrails,
    temporal_shortlist: int,
    min_delay: int,
    max_delay: int,
    min_gap: int,
    global_candidates: int,
    local_candidates: int,
) -> str:
    tested = [c for c in candidates if c.temporal is not None]
    passed = [c for c in tested if c.temporal is not None and c.temporal.passed]
    best_cv = max((c.metrics["spacing_cv"] for c in passed), default=math.nan)
    historical_t = historical.temporal
    current_t = current.temporal
    def better(metric: str, higher: bool = False) -> str:
        if best_feasible is None:
            return "not available"
        a = best_feasible.metrics[metric]
        b = current.metrics[metric]
        return ("yes" if (a > b if higher else a < b) else "no") + f" ({a:.6f} vs {b:.6f})"
    lines = [
        "bBpiano v2b modal-structure search with temporal guardrails",
        "=" * 68, f"source ref: {source_ref}", "",
        "[SEARCH SCOPE]",
        "Eight delay lengths only; fixed shifted-Householder topology.",
        "Lossless structural search; damping, readout and direct path excluded.",
        (f"Stage A: {global_candidates} global + {local_candidates} local candidates; "
        f"Stage B: top {temporal_shortlist} by modal/geometry score."),
        "", "[MODAL TARGET]",
        (f"density={target.LOW_FREQ_MODAL_DENSITY_MODES_PER_HZ:.3f} modes/Hz; "
        f"sum(L)={target_total_delay_samples()}; Rayleigh CV={target.RAYLEIGH_SPACING_CV:.6f}."),
        "Original four modal objective terms, target curves and weights unchanged.",
        "", "[ENGINEERING GUARDRAILS — not piano-physics targets]",
        f"min_delay={min_delay}; max_delay={max_delay}; min_gap={min_gap}",
        (f"pair proximity sigma={geometry.sigma:g}; "
        f"CV floor={geometry.cv_floor_ratio:g} × current; "
        f"proximity ceiling={geometry.proximity_max_ratio:g} × current."),
        "One-sided spread/proximity penalties add to Stage A search score.",
        "Best modal-only means Stage A modal+geometry score before temporal filtering.",
        "", "[CURRENT PRODUCTION]", format_metrics(current),
        "", "[SCALED CURRENT]", format_metrics(scaled),
        "", "[BEST MODAL-ONLY CANDIDATE]", format_metrics(best_modal),
        "", "[HISTORICAL CLUSTER CONTROL — user-supplied, excluded from ranking]",
        format_metrics(historical),
        "", "[TEMPORAL DIFFUSION BASELINE]",
        "Lossless zero-state delta input; 8 runtime ring buffers; no damping/direct/shaper.",
        (f"duration={temporal_rules.duration_ms:g} ms; NED window={temporal_rules.window_ms:g} ms; "
        f"hop={NED_HOP_MS:g} ms; NED=P(|x|>window std)/{NED_GAUSSIAN_REFERENCE}."),
        "NED is not clipped; 20–80 ms summary; 3 consecutive windows for time to 0.75.",
        format_metrics(current),
        "", "[BEST TEMPORALLY FEASIBLE CANDIDATE]",
        format_metrics(best_feasible) if best_feasible else
        "NO TEMPORALLY FEASIBLE DELAY-ONLY CANDIDATE FOUND\n"
        "best_temporally_feasible_candidate = None",
        "", "[GUARDRAIL RESULTS]",
        f"tested={len(tested)}; passed={len(passed)}; failed={len(tested)-len(passed)}.",
        (f"median NED floor={temporal_rules.ned_floor_ratio:g} × current; "
        f"time ceiling={temporal_rules.ned_time_max_ratio:g} × current if current reaches 0.75; "
        f"kurtosis ceiling={temporal_rules.kurtosis_max_ratio:g} × current (epsilon 1e-9)."),
        f"maximum temporal-feasible spacing CV in shortlist={best_cv:.6f}.",
        "", "[INTERPRETATION]",
        (f"Historical cluster temporal guardrail pass: "
        f"{historical_t.passed if historical_t else 'not tested'}; "
        f"failures={historical_t.failure_reasons if historical_t else 'not tested'}."),
        (f"Historical minus current median NED="
         f"{historical_t.median_ned_20_80 - current_t.median_ned_20_80:+.6f}; "
         f"time to NED 0.75="
         f"{historical_t.time_to_ned075_ms - current_t.time_to_ned075_ms:+.2f} ms; "
         f"kurtosis error="
         f"{historical_t.kurtosis_error_20_80 - current_t.kurtosis_error_20_80:+.6f}."
         if historical_t and current_t else "Historical temporal comparison unavailable."),
        f"Best feasible spacing CV improves current: {better('spacing_cv', True)}",
        f"Best feasible CDF distance improves current: {better('spacing_cdf_distance')}",
        f"Best feasible quantile RMSE improves current: {better('spacing_quantile_rmse')}",
        "A lower modal/geometry score is not an audio-quality score.",
    ]
    if best_feasible is not None and best_modal.temporal is not None and (
        not best_modal.temporal.passed
    ) and best_modal.metrics["spacing_cv"] >= 0.44 and best_feasible.metrics["spacing_cv"] < 0.44:
        lines.append(
            "Evidence suggests delay-only optimization is reaching an architectural "
            "limit under the fixed shifted-Householder feedback matrix."
        )
    else:
        lines.append(
            "Diminishing returns / architectural limit: not established by this shortlist."
        )
    lines += [
        "", "[NEXT STEP]",
        "Run the requested full search before selecting a delay candidate for integration.",
        "No damping SOS, production delay bank, or MIDI rendering is changed here.",
    ]
    return "\n".join(lines) + "\n"


# ============================================================================
# 16. Main
# ============================================================================

def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--source-ref",
        default="HEAD",
        help=(
            "从哪个 Git ref 读取当前 "
            "production delay bank；默认 HEAD"
        ),
    )

    parser.add_argument(
        "--seed",
        type=int,
        default=20260928,
    )

    parser.add_argument(
        "--global-candidates",
        type=int,
        default=7500,
    )

    parser.add_argument(
        "--local-candidates",
        type=int,
        default=6500,
    )

    parser.add_argument(
        "--min-delay",
        type=int,
        default=DEFAULT_MIN_DELAY,
    )

    parser.add_argument(
        "--max-delay",
        type=int,
        default=DEFAULT_MAX_DELAY,
    )

    parser.add_argument(
        "--min-gap",
        type=int,
        default=DEFAULT_MIN_GAP,
    )

    parser.add_argument("--proximity-sigma", type=float, default=50.0)
    parser.add_argument("--delay-cv-floor-ratio", type=float, default=0.70)
    parser.add_argument("--proximity-max-ratio", type=float, default=2.0)
    parser.add_argument("--temporal-shortlist", type=int, default=TEMPORAL_SHORTLIST)
    parser.add_argument("--diffusion-duration-ms", type=float, default=150.0)
    parser.add_argument("--ned-window-ms", type=float, default=10.0)
    parser.add_argument("--ned-floor-ratio", type=float, default=0.90)
    parser.add_argument("--ned-time-max-ratio", type=float, default=1.15)
    parser.add_argument("--kurtosis-max-ratio", type=float, default=1.25)

    parser.add_argument(
        "--output",
        type=Path,
        default=(
            Path(__file__).resolve().parent
            / "output"
            / "structure_search"
        ),
    )

    args = parser.parse_args()

    for name in ("proximity_sigma", "delay_cv_floor_ratio", "proximity_max_ratio",
                 "diffusion_duration_ms", "ned_window_ms", "ned_floor_ratio",
                 "ned_time_max_ratio", "kurtosis_max_ratio"):
        if not math.isfinite(getattr(args, name)) or getattr(args, name) <= 0:
            parser.error(f"--{name.replace('_', '-')} must be positive and finite")
    if args.temporal_shortlist < 1 or args.global_candidates < 0 or args.local_candidates < 0:
        parser.error("candidate counts and temporal shortlist must be nonnegative/positive")
    if args.diffusion_duration_ms < 100.0 + args.ned_window_ms / 2:
        parser.error("duration must cover 100 ms plus half of the NED window")

    root = repo_root()

    source_ref = resolve_ref(
        root,
        args.source_ref,
    )

    source = git_show(
        root,
        source_ref,
        SOURCE_PATH,
    )
    verify_lossless_runtime_topology(source)

    current_delays = parse_delays(
        source
    )
    geometry = geometry_reference(
        current_delays, args.proximity_sigma, args.delay_cv_floor_ratio,
        args.proximity_max_ratio,
    )
    temporal_rules = TemporalGuardrails(
        args.diffusion_duration_ms, args.ned_window_ms, args.ned_floor_ratio,
        args.ned_time_max_ratio, args.kurtosis_max_ratio,
    )

    print(
        "bBpiano v2b "
        "modal-structure delay search"
    )

    print(
        "=" * 60
    )

    print(
        f"source ref      : "
        f"{source_ref}"
    )

    print(
        f"current delays  : "
        f"{current_delays.tolist()}"
    )

    print(
        f"current sum(L)  : "
        f"{int(np.sum(current_delays))}"
    )

    print(
        f"target sum(L)   : "
        f"{target_total_delay_samples()}"
    )

    print(
        f"Rayleigh CV     : "
        f"{target.RAYLEIGH_SPACING_CV:.6f}"
    )

    print()

    (
        refined,
        current_eval,
        scaled_eval,
    ) = run_search(
        current_delays=current_delays,
        geometry=geometry,
        temporal_shortlist=args.temporal_shortlist,
        seed=args.seed,
        global_candidates=(
            args.global_candidates
        ),
        local_candidates=(
            args.local_candidates
        ),
        min_delay=args.min_delay,
        max_delay=args.max_delay,
        min_gap=args.min_gap,
    )

    if not refined:

        raise RuntimeError(
            "搜索没有产生有效候选"
        )

    # 只对 fine-grid Stage A 前 N 名做时域检查；原 modal 排名不改。
    current_eval.temporal = temporal_metrics(current_eval.delays, temporal_rules)
    current_eval.temporal.passed = True
    for candidate in refined[:args.temporal_shortlist]:
        candidate.temporal = temporal_metrics(candidate.delays, temporal_rules)
        check_temporal(candidate.temporal, current_eval.temporal, temporal_rules)
    best_modal = refined[0]
    best_feasible = next(
        (c for c in refined if c.temporal is not None and c.temporal.passed), None
    )
    historical = evaluate_candidate(
        np.asarray(DEFAULT_HISTORICAL_DELAYS), geometry,
        root_step_hz=FINAL_ROOT_STEP_HZ, detect_tangent_roots=True,
        tag="historical-cluster-control",
    )
    historical.temporal = temporal_metrics(historical.delays, temporal_rules)
    check_temporal(historical.temporal, current_eval.temporal, temporal_rules)

    args.output.mkdir(
        parents=True,
        exist_ok=True,
    )

    write_top_candidates(
        args.output
        / "top_candidates.csv",
        refined,
    )

    write_best_json(
        args.output
        / "best_candidate.json",
        best_modal,
        best_feasible,
        current_eval,
        scaled_eval,
        historical,
        source_ref,
        geometry,
        temporal_rules,
        args.temporal_shortlist,
    )

    plot_density_compare(
        args.output
        / "01_modal_density_compare.png",
        current_eval,
        best_modal,
    )

    plot_spacing_compare(
        args.output
        / "02_spacing_compare.png",
        current_eval,
        best_modal,
    )

    plot_delay_sets(
        args.output
        / "03_delay_sets.png",
        current_eval,
        scaled_eval,
        best_modal,
    )

    plot_echo_density(
        args.output / "04_echo_density_compare.png",
        current_eval, best_modal, best_feasible, historical,
    )
    plot_search_tradeoff(
        args.output / "05_search_tradeoff.png",
        refined, current_eval, best_modal, best_feasible,
    )

    report = make_report(
        source_ref=source_ref,
        current=current_eval,
        scaled=scaled_eval,
        best_modal=best_modal,
        best_feasible=best_feasible,
        historical=historical,
        candidates=refined,
        geometry=geometry,
        temporal_rules=temporal_rules,
        temporal_shortlist=args.temporal_shortlist,
        min_delay=args.min_delay,
        max_delay=args.max_delay,
        min_gap=args.min_gap,
        global_candidates=(
            args.global_candidates
        ),
        local_candidates=(
            args.local_candidates
        ),
    )

    (
        args.output
        / "report.txt"
    ).write_text(
        report,
        encoding="utf-8",
    )

    print(
        "\n"
        + report
    )

    print(
        f"输出目录："
        f"{args.output}"
    )


if __name__ == "__main__":
    main()
