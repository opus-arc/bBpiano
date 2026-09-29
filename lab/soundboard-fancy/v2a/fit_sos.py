"""将 v2a branch target 拟合为共享形状、随 delay 缩放的参数式 SOS bank。

默认运行：.venv/bin/python fit_sos.py；候选模式需指定 --delay-candidate-json 和 --output。
默认只写 output/fit/；不会改动 production C++。target_damping 在 import 时
会自行重写其原有的 target CSV/图片，这是上游模块目前的顶层副作用。
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import target_damping as target
from scipy.optimize import least_squares

OUTPUT_DIR = Path(__file__).resolve().parent / "output" / "fit"
REFERENCE_DELAY = int(max(target.FDN_DELAYS))
PRODUCTION_SECTION_LIMIT = 8  # 审计自 soundboard_model.hpp 的 kLossSectionCount
SECTION_KINDS = ("low_shelf", "peak", "peak", "peak", "peak", "high_shelf")
SECTION_NAMES = ("bass_floor", "low_mid", "radiation", "upper_mid", "cap_edge", "treble_cap")
FIT_FREQUENCIES = np.geomspace(20.0, target.FS / 2.0, 384)
PLOT_FREQUENCIES = np.geomspace(20.0, target.FS / 2.0, 4096)

# 这些是工程门槛，不声称完整 FDN 的模态或听感已合格。
RMS_WARN_DB = 1.0
MAX_WARN_DB = 3.0
MAX_POLE_RADIUS = 0.9995
GAIN_TOLERANCE_DB = 1e-3


def normalize(b0, b1, b2, a0, a1, a2):
    """输出 [b0,b1,b2,a1,a2]；分母是 1+a1 z^-1+a2 z^-2。"""
    terms = np.broadcast_arrays(b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0)
    return np.stack(terms, axis=-1)


def design_biquad(kind: str, fc: float, shape: float, gain_db, fs: float):
    """RBJ Audio EQ Cookbook；shape 对 peak 是 Q，对 shelf 是 S。

    与 C++ 转置 DF-II 完全同号：y=b0*x+s1，s1=b1*x-a1*y+s2，
    s2=b2*x-a2*y。gain_db 可为 8 条 branch 的向量。
    """
    gain = np.asarray(gain_db, dtype=float)
    a = np.power(10.0, gain / 40.0)
    omega = 2.0 * np.pi * fc / fs
    c, s = np.cos(omega), np.sin(omega)

    if kind == "peak":
        alpha = s / (2.0 * shape)
        return normalize(
            1.0 + alpha * a, -2.0 * c, 1.0 - alpha * a,
            1.0 + alpha / a, -2.0 * c, 1.0 - alpha / a,
        )

    if kind not in ("low_shelf", "high_shelf"):
        raise ValueError(f"未知 biquad 类型：{kind}")
    alpha = s / 2.0 * np.sqrt((a + 1.0 / a) * (1.0 / shape - 1.0) + 2.0)
    root = 2.0 * np.sqrt(a) * alpha
    ap, am = a + 1.0, a - 1.0
    if kind == "low_shelf":
        return normalize(
            a * (ap - am * c + root), 2.0 * a * (am - ap * c),
            a * (ap - am * c - root), ap + am * c + root,
            -2.0 * (am + ap * c), ap + am * c - root,
        )
    return normalize(
        a * (ap + am * c + root), -2.0 * a * (am + ap * c),
        a * (ap + am * c - root), ap - am * c + root,
        2.0 * (am - ap * c), ap - am * c - root,
    )


def response_db(coefficients: np.ndarray, frequencies: np.ndarray, fs: float):
    """coefficients: [branch, section, 5]；以 log magnitude 累加避免下溢。"""
    z = np.exp(-2j * np.pi * frequencies / fs)[None, None, :]
    c = coefficients[:, :, :, None]
    numerator = c[:, :, 0] + c[:, :, 1] * z + c[:, :, 2] * z * z
    denominator = 1.0 + c[:, :, 3] * z + c[:, :, 4] * z * z
    return np.sum(20.0 * np.log10(np.abs(numerator / denominator)), axis=1)


def target_db(frequencies: np.ndarray, delays: np.ndarray):
    """物理规律只从 target_damping 读取，不复制 alpha 公式。"""
    return np.stack([
        target.magnitude_to_db(target.branch_target_magnitude(frequencies, int(delay)))
        for delay in delays
    ])


def unpack_parameters(parameters: np.ndarray):
    matrix = parameters.reshape(len(SECTION_KINDS), 3)
    return np.exp(matrix[:, 0]), matrix[:, 1], matrix[:, 2]


def make_coefficients(parameters: np.ndarray, delays: np.ndarray):
    centers, shapes, reference_gains = unpack_parameters(parameters)
    scale = delays / REFERENCE_DELAY
    sections = [
        design_biquad(kind, fc, shape, reference_gain * scale, target.FS)
        for kind, fc, shape, reference_gain in zip(
            SECTION_KINDS, centers, shapes, reference_gains, strict=True
        )
    ]
    return np.stack(sections, axis=1)


def parameter_bounds():
    # 各 peak 固定在不同宽频带，避免 section 对调及针对 target 折点的窄带过拟合。
    fc_bounds = ((60, 700), (300, 900), (700, 1500),
                 (1200, 2600), (2200, 4500), (700, 5000))
    shape_bounds = ((0.25, 1.0), (0.4, 2.0), (0.4, 2.0),
                    (0.4, 2.0), (0.4, 2.0), (0.25, 1.0))
    gain_bounds = ((-12, 0), (-18, 0), (-18, 0),
                   (-18, 0), (-18, 0), (-40, 0))
    lower, upper = [], []
    for (f0, f1), (s0, s1), (g0, g1) in zip(
        fc_bounds, shape_bounds, gain_bounds, strict=True
    ):
        lower.extend((np.log(f0), s0, g0))
        upper.extend((np.log(f1), s1, g1))
    return np.array(lower), np.array(upper)


def objective(parameters: np.ndarray, frequencies: np.ndarray,
              desired_db: np.ndarray, delays: np.ndarray):
    fitted = response_db(make_coefficients(parameters, delays), frequencies, target.FS)
    error = fitted - desired_db
    # 对数采样使每倍频程有近似相同点数。短 branch 的误差按 sqrt(Lref/L)
    # 提权；长 branch 仍因绝对 dB 误差较大而有充分权重。
    branch_weight = np.sqrt(REFERENCE_DELAY / delays)[:, None]
    point_residual = (error * branch_weight).ravel()

    # 每 1/6 octave 的局部 RMS 防止一段频率误差被全频平均掩盖。
    log_bins = np.floor(6.0 * np.log2(frequencies / frequencies[0])).astype(int)
    local = []
    for bin_index in np.unique(log_bins):
        segment = error[:, log_bins == bin_index] * branch_weight
        local.extend(0.7 * np.sqrt(np.mean(segment**2, axis=1)))
    return np.concatenate((point_residual, local))


def fit_parameters(delays: np.ndarray):
    initial = np.array([
        (240, 0.6, -3.0), (550, 0.7, -2.0),
        (1100, 0.8, -2.0), (1900, 0.8, -2.0),
        (3000, 0.7, -2.0), (2200, 0.7, -24.0),
    ], dtype=float)
    initial[:, 0] = np.log(initial[:, 0])
    lower, upper = parameter_bounds()
    desired = target_db(FIT_FREQUENCIES, delays)
    result = least_squares(
        objective, initial.ravel(), bounds=(lower, upper),
        args=(FIT_FREQUENCIES, desired, delays),
        loss="soft_l1", f_scale=0.75, max_nfev=450,
        x_scale="jac", ftol=1e-8, xtol=1e-8, gtol=1e-8,
    )
    if not result.success:
        print(f"WARNING: optimizer 未收敛：{result.message}")
    return result


def pole_radii(coefficients: np.ndarray):
    # z^2+a1*z+a2=0；用复数平方根涵盖实极点与共轭极点。
    discriminant = np.lib.scimath.sqrt(coefficients[..., 3] ** 2 - 4 * coefficients[..., 4])
    return np.maximum(
        np.abs((-coefficients[..., 3] + discriminant) / 2),
        np.abs((-coefficients[..., 3] - discriminant) / 2),
    )


def compute_metrics(coefficients: np.ndarray, delays: np.ndarray):
    frequencies = PLOT_FREQUENCIES
    wanted = target_db(frequencies, delays)
    actual = response_db(coefficients, frequencies, target.FS)
    error = actual - wanted
    bands = (("low_20_300", 20, 300), ("mid_300_3000", 300, 3000),
             ("high_3000_nyquist", 3000, target.FS / 2))
    rows = []
    for index, delay in enumerate(delays):
        band_metrics = {}
        for name, low, high in bands:
            mask = (frequencies >= low) & (frequencies <= high)
            band_metrics[name] = {
                "rms_db": float(np.sqrt(np.mean(error[index, mask] ** 2))),
                "max_abs_db": float(np.max(np.abs(error[index, mask]))),
            }
        rows.append({
            "delay_samples": int(delay),
            "rms_db": float(np.sqrt(np.mean(error[index] ** 2))),
            "max_abs_db": float(np.max(np.abs(error[index]))),
            "max_gain_db": float(np.max(actual[index])),
            "bands": band_metrics,
        })
    return frequencies, wanted, actual, rows


def plot_curves(frequencies, desired, fitted, delays):
    for name, values, ylabel in (
        ("target_vs_fitted.png", (desired, fitted), "Attenuation per traversal (dB)"),
        ("error_vs_frequency.png", (fitted - desired,), "Fitted minus target (dB)"),
    ):
        fig, ax = plt.subplots(figsize=(11, 6))
        for index, delay in enumerate(delays):
            color = f"C{index}"
            if len(values) == 2:
                ax.semilogx(frequencies, values[0][index], color=color, alpha=0.45)
                ax.semilogx(frequencies, values[1][index], color=color, linestyle="--",
                            label=f"L={delay}")
            else:
                ax.semilogx(frequencies, values[0][index], color=color, label=f"L={delay}")
        ax.set(xlabel="Frequency (Hz)", ylabel=ylabel)
        ax.grid(True, which="both", alpha=0.3)
        ax.legend(ncol=2)
        fig.tight_layout()
        fig.savefig(OUTPUT_DIR / name, dpi=180)
        plt.close(fig)


def cpp_initializer(coefficients: np.ndarray, delays: np.ndarray):
    def c_float(value: float):
        literal = f"{float(value):.9g}"
        if "e" not in literal and "." not in literal:
            literal += ".0"
        return literal + "f"

    lines = [
        "// 由 fit_sos.py 生成；仅适用 Fs=44100 Hz。",
        "// 手动放进 initialize_losses()，替换当前旧 loss 初始化；不要和旧逻辑叠加。",
        "// 系数顺序：b0,b1,b2,a1,a2；后两节为 identity。",
        "static constexpr float kV2aLossSOS[kFDNSize][kLossSectionCount][5] = {",
    ]
    for delay, branch in zip(delays, coefficients, strict=True):
        lines.append(f"    {{ // L={int(delay)}")
        for section in list(branch) + [np.array([1, 0, 0, 0, 0], dtype=float)] * (
            PRODUCTION_SECTION_LIMIT - len(branch)
        ):
            formatted = ", ".join(c_float(value) for value in section)
            lines.append(f"        {{{formatted}}},")
        lines.append("    },")
    lines += [
        "};",
        "for (std::size_t i = 0; i < kFDNSize; ++i) {",
        "    for (std::size_t j = 0; j < kLossSectionCount; ++j) {",
        "        auto& s = loss_filters_[i].sections[j];",
        "        s.b0 = kV2aLossSOS[i][j][0];",
        "        s.b1 = kV2aLossSOS[i][j][1];",
        "        s.b2 = kV2aLossSOS[i][j][2];",
        "        s.a1 = kV2aLossSOS[i][j][3];",
        "        s.a2 = kV2aLossSOS[i][j][4];",
        "        s.s1 = 0.0f; s.s2 = 0.0f;",
        "    }",
        "}",
    ]
    return "\n".join(lines) + "\n"


def export(parameters: np.ndarray, coefficients: np.ndarray,
           delays: np.ndarray, optimizer_result, *, candidate_mode: bool = False,
           candidate_source: str | None = None):
    centers, shapes, reference_gains = unpack_parameters(parameters)
    frequencies, desired, fitted, metrics = compute_metrics(coefficients, delays)
    section_radius = pole_radii(coefficients)
    full_grid = np.concatenate(([0.0], frequencies))
    global_max_gain = float(np.max(response_db(coefficients, full_grid, target.FS)))
    max_radius = float(np.max(section_radius))
    if not np.isfinite(coefficients).all() or max_radius >= MAX_POLE_RADIUS:
        raise RuntimeError(f"拒绝导出：SOS 稳定余量不足，最大 pole radius={max_radius}")
    if global_max_gain > GAIN_TOLERANCE_DB:
        raise RuntimeError(f"拒绝导出：branch 存在正增益 {global_max_gain:.6f} dB")
    if len(SECTION_KINDS) > PRODUCTION_SECTION_LIMIT:
        raise RuntimeError("SOS 数超过 production 容器上限")
    if np.max(np.abs(coefficients)) > 10.0:
        raise RuntimeError("拒绝导出：SOS coefficient magnitude > 10")

    payload = {
        "format_version": 1,
        "sample_rate_hz": target.FS,
        "reference_delay_samples": REFERENCE_DELAY,
        "coefficient_convention": "(b0+b1*z^-1+b2*z^-2)/(1+a1*z^-1+a2*z^-2)",
        "gain_scaling": "gain_db(L)=reference_gain_db*L/reference_delay_samples",
        "topology": list(SECTION_KINDS),
        "parameters": [
            {"name": name, "kind": kind, "fc_hz": float(fc),
             "q_or_slope": float(shape), "reference_gain_db": float(gain)}
            for name, kind, fc, shape, gain in zip(
                SECTION_NAMES, SECTION_KINDS, centers, shapes, reference_gains, strict=True
            )
        ],
        "branches": [
            {"delay_samples": int(delay), "sections": [
                {"name": SECTION_NAMES[j], "kind": SECTION_KINDS[j],
                 "fc_hz": float(centers[j]), "q_or_slope": float(shapes[j]),
                 "gain_db": float(reference_gains[j] * delay / REFERENCE_DELAY),
                 "b0": float(section[0]), "b1": float(section[1]),
                 "b2": float(section[2]), "a1": float(section[3]),
                 "a2": float(section[4])}
                for j, section in enumerate(branch)
            ]}
            for delay, branch in zip(delays, coefficients, strict=True)
        ],
        "fit_metrics": metrics,
        "max_pole_radius": max_radius,
        "max_gain_db_0_to_nyquist": global_max_gain,
        "optimizer": {"success": bool(optimizer_result.success),
                      "message": str(optimizer_result.message),
                      "nfev": int(optimizer_result.nfev)},
    }
    if candidate_mode:
        payload["candidate_source"] = candidate_source
        payload["production_section_count"] = PRODUCTION_SECTION_LIMIT
        payload["identity_sections_per_branch"] = PRODUCTION_SECTION_LIMIT - len(SECTION_KINDS)
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    (OUTPUT_DIR / ("candidate_sos.json" if candidate_mode else "coefficients.json")).write_text(
        json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    if not candidate_mode:
        (OUTPUT_DIR / "coefficients_cpp.txt").write_text(
            cpp_initializer(coefficients, delays), encoding="utf-8"
        )
        plot_curves(frequencies, desired, fitted, delays)
    lines = ["delay  RMS(dB)  max_abs(dB)  LF_RMS  MID_RMS  HF_RMS  pole_radius  max_gain(dB)"]
    for row, radius in zip(metrics, section_radius, strict=True):
        bands = row["bands"]
        lines.append(
            f"{row['delay_samples']:5d}  {row['rms_db']:7.3f}  {row['max_abs_db']:11.3f}  "
            f"{bands['low_20_300']['rms_db']:6.3f}  {bands['mid_300_3000']['rms_db']:7.3f}  "
            f"{bands['high_3000_nyquist']['rms_db']:6.3f}  {np.max(radius):11.6f}  "
            f"{row['max_gain_db']:12.6f}"
        )
    quality_warning = any(
        row["rms_db"] > RMS_WARN_DB or row["max_abs_db"] > MAX_WARN_DB for row in metrics
    )
    overall_rms = float(np.sqrt(np.mean((fitted - desired) ** 2)))
    lines += [
        f"max pole radius (all sections): {max_radius:.8f}",
        f"max gain 0..Nyquist: {global_max_gain:.8f} dB; >0 dB: {global_max_gain > GAIN_TOLERANCE_DB}",
        "WARNING: 误差超过经验门槛，请勿直接接入 production。" if quality_warning
        else "PASS: branch 幅度误差低于脚本经验门槛；仍需独立 eval、FDN pole 与试听检查。",
    ]
    if candidate_mode:
        lines.insert(-3, f"overall RMS dB error (all branches, 20 Hz..Nyquist): {overall_rms:.6f}")
    (OUTPUT_DIR / "fit_report.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))
    print(f"输出目录：{OUTPUT_DIR}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--delay-candidate-json", type=Path)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    candidate_mode = args.delay_candidate_json is not None
    if candidate_mode:
        if args.output is None:
            parser.error("--delay-candidate-json requires --output to protect v2a output")
        data = json.loads(args.delay_candidate_json.read_text(encoding="utf-8"))
        chosen = data.get("best_temporally_feasible")
        if not isinstance(chosen, dict):
            raise RuntimeError("candidate JSON lacks best_temporally_feasible")
        raw = chosen.get("delays")
        delays = np.asarray([int(x) for x in raw.split()] if isinstance(raw, str) else raw, dtype=int)
        modal = data.get("best_modal_only")
        modal_raw = modal.get("delays") if isinstance(modal, dict) else None
        modal_delays = np.asarray([int(x) for x in modal_raw.split()] if isinstance(modal_raw, str) else modal_raw, dtype=int) if modal_raw is not None else None
        if modal_delays is None or not np.array_equal(delays, modal_delays):
            print("NOTICE: best_modal_only differs; using best_temporally_feasible")
        else:
            print("Candidate check: best_modal_only == best_temporally_feasible")
        if int(chosen.get("sum_delays", -1)) != int(np.sum(delays)):
            raise RuntimeError("candidate sum_delays disagrees with delays")
        print(f"Candidate delays from JSON: {delays.tolist()}; sum={int(np.sum(delays))}")
    else:
        delays = np.asarray(target.FDN_DELAYS, dtype=int)
    if len(delays) != 8 or len(set(delays)) != 8 or np.any(delays <= 0):
        raise RuntimeError("target branch 配置与本次设计假设不符")
    if not candidate_mode and int(max(delays)) != REFERENCE_DELAY:
        raise RuntimeError("default target branch 配置与本次设计假设不符")
    global OUTPUT_DIR
    if args.output is not None:
        OUTPUT_DIR = args.output.resolve()
    if candidate_mode and OUTPUT_DIR == (Path(__file__).resolve().parent / "output" / "fit"):
        raise RuntimeError("candidate cannot overwrite v2a production fit output")
    result = fit_parameters(delays)
    # 导出按 C++ float 精度量化的系数；诊断与用户复制的值必须一致。
    coefficients = make_coefficients(result.x, delays).astype(np.float32).astype(float)
    export(result.x, coefficients, delays, result, candidate_mode=candidate_mode,
           candidate_source=str(args.delay_candidate_json.resolve()) if candidate_mode else None)


if __name__ == "__main__":
    main()
