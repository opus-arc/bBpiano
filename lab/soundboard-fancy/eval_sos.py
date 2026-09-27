"""独立读取 coefficients.json，重建 SOS 响应并评估 Soundboard v2a bank。

运行：.venv/bin/python eval_sos.py
不 import fit_sos，也不使用 optimizer 状态。target_damping 当前在 import 时
会自行重写旧 target CSV/图片；本脚本的 eval 产物写入 output/fit/。
"""

from __future__ import annotations

import json
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import target_damping as target

OUTPUT_DIR = Path(__file__).resolve().parent / "output" / "fit"
COEFFICIENT_KEYS = ("b0", "b1", "b2", "a1", "a2")
FREQUENCIES = np.geomspace(20.0, target.FS / 2.0, 8192)
RMS_WARN_DB = 1.0
MAX_WARN_DB = 3.0
POLE_LIMIT = 0.9995
GAIN_TOLERANCE_DB = 1e-3
SCALING_RMS_WARN_DB = 1.0  # 换算到最长 delay 的 dB 尺度
SCALING_MAX_WARN_DB = 2.5
NARROW_CURVATURE_WARN_DB = 1.0  # 约 1/6 octave 宽度的异常尖峰/陷波


def load_coefficients(path: Path):
    document = json.loads(path.read_text(encoding="utf-8"))
    if document.get("format_version") != 1:
        raise ValueError("不支持的 coefficients.json format_version")
    if float(document.get("sample_rate_hz", -1)) != target.FS:
        raise ValueError("系数 sample rate 与 target 不一致")
    expected_delays = np.asarray(target.FDN_DELAYS, dtype=int)
    branches = document.get("branches", [])
    delays = np.asarray([branch["delay_samples"] for branch in branches], dtype=int)
    if len(delays) != 8 or not np.array_equal(delays, expected_delays):
        raise ValueError("系数文件必须按 production 顺序包含全部 8 条 target delay")
    section_counts = [len(branch.get("sections", [])) for branch in branches]
    if len(set(section_counts)) != 1 or not 1 <= section_counts[0] <= 8:
        raise ValueError("每条 branch 必须有相同的 1..8 个 SOS")
    coefficients = np.asarray([
        [[section[key] for key in COEFFICIENT_KEYS] for section in branch["sections"]]
        for branch in branches
    ], dtype=float)
    if coefficients.shape != (8, section_counts[0], 5) or not np.isfinite(coefficients).all():
        raise ValueError("系数形状错误或包含 NaN/Inf")
    return document, delays, coefficients


def evaluate_response(coefficients: np.ndarray, frequencies: np.ndarray, fs: float):
    """独立使用 H(z)=(b0+b1z^-1+b2z^-2)/(1+a1z^-1+a2z^-2)。"""
    z = np.exp(-2j * np.pi * frequencies / fs)[None, None, :]
    c = coefficients[:, :, :, None]
    top = c[:, :, 0] + c[:, :, 1] * z + c[:, :, 2] * z * z
    bottom = 1.0 + c[:, :, 3] * z + c[:, :, 4] * z * z
    section_db = 20.0 * np.log10(np.abs(top / bottom))
    return np.sum(section_db, axis=1), section_db


def target_response(frequencies: np.ndarray, delays: np.ndarray):
    return np.stack([
        target.magnitude_to_db(target.branch_target_magnitude(frequencies, int(delay)))
        for delay in delays
    ])


def pole_radii(coefficients: np.ndarray):
    # 独立地求解每节分母多项式 z^2+a1*z+a2。
    return np.asarray([
        [max(np.abs(np.roots([1.0, section[3], section[4]]))) for section in branch]
        for branch in coefficients
    ])


def narrow_feature_curvature(response_db: np.ndarray, frequencies: np.ndarray):
    """与左右各 1/12 octave 的弦线比较，捕捉窄峰/窄 notch。

    在边界不外推；宽带 shelf 过渡不会仅凭整体斜率触发。
    """
    log_f = np.log2(frequencies)
    offset = 1.0 / 12.0
    valid = (log_f >= log_f[0] + offset) & (log_f <= log_f[-1] - offset)
    results = []
    for branch in response_db:
        left = np.interp(log_f[valid] - offset, log_f, branch)
        right = np.interp(log_f[valid] + offset, log_f, branch)
        curvature = branch[valid] - (left + right) / 2.0
        at = int(np.argmax(np.abs(curvature)))
        results.append({
            "max_abs_db": float(abs(curvature[at])),
            "frequency_hz": float(frequencies[valid][at]),
            "type": "peak" if curvature[at] > 0 else "notch",
        })
    return results


def branch_metrics(frequencies, wanted, fitted, delays, radii, narrow):
    error = fitted - wanted
    bands = (("low_20_300", 20, 300), ("mid_300_3000", 300, 3000),
             ("high_3000_nyquist", 3000, target.FS / 2))
    rows = []
    for index, delay in enumerate(delays):
        segment_metrics = {}
        for name, low, high in bands:
            mask = (frequencies >= low) & (frequencies <= high)
            segment_metrics[name] = {
                "rms_db": float(np.sqrt(np.mean(error[index, mask] ** 2))),
                "max_abs_db": float(np.max(np.abs(error[index, mask]))),
            }
        rows.append({
            "delay_samples": int(delay),
            "rms_db": float(np.sqrt(np.mean(error[index] ** 2))),
            "max_abs_db": float(np.max(np.abs(error[index]))),
            "max_pole_radius": float(np.max(radii[index])),
            "max_gain_db_20_to_nyquist": float(np.max(fitted[index])),
            "narrow_feature": narrow[index],
            "bands": segment_metrics,
        })
    return rows


def scaling_metrics(fitted: np.ndarray, delays: np.ndarray):
    # 理想 target 满足 G_i/L_i 相同。EQ 的 gain dB 线性缩放是参数约束，
    # 但 RBJ 系数到完整响应非线性，因此这里检验实际结果，而非相信元数据。
    reference_delay = int(max(delays))
    reference_index = int(np.argmax(delays))
    equivalent = fitted / delays[:, None] * reference_delay
    difference = equivalent - equivalent[reference_index]
    return {
        "reference_delay_samples": reference_delay,
        "rms_db_at_reference_scale": float(np.sqrt(np.mean(difference ** 2))),
        "max_abs_db_at_reference_scale": float(np.max(np.abs(difference))),
    }


def parameter_scaling_error(document, delays):
    """核对导出元数据中每节 gain 是否真按 L/Lref 缩放。"""
    reference = int(document.get("reference_delay_samples", -1))
    if reference != int(max(delays)):
        return float("inf")
    branches = document["branches"]
    reference_branch = branches[int(np.argmax(delays))]
    reference_gains = [section["gain_db"] for section in reference_branch["sections"]]
    differences = [
        abs(section["gain_db"] - reference_gains[j] * int(delay) / reference)
        for delay, branch in zip(delays, branches, strict=True)
        for j, section in enumerate(branch["sections"])
    ]
    return float(max(differences))


def plot_results(frequencies, wanted, fitted, delays):
    fig, axes = plt.subplots(4, 2, figsize=(13, 15), sharex=True)
    for index, ax in enumerate(axes.flat):
        ax.semilogx(frequencies, wanted[index], label="target", lw=1.5)
        ax.semilogx(frequencies, fitted[index], label="SOS", lw=1.3)
        ax.set_title(f"L = {int(delays[index])} samples")
        ax.grid(True, which="both", alpha=0.25)
        ax.set_ylabel("Attenuation (dB)")
        if index >= 6:
            ax.set_xlabel("Frequency (Hz)")
        if index == 0:
            ax.legend()
    fig.tight_layout()
    fig.savefig(OUTPUT_DIR / "eval_target_vs_fitted.png", dpi=180)
    plt.close(fig)

    fig, axes = plt.subplots(4, 2, figsize=(13, 15), sharex=True)
    for index, ax in enumerate(axes.flat):
        ax.semilogx(frequencies, fitted[index] - wanted[index])
        ax.axhline(0, color="black", linewidth=0.5)
        ax.set_title(f"L = {int(delays[index])} samples")
        ax.grid(True, which="both", alpha=0.25)
        ax.set_ylabel("SOS minus target (dB)")
        if index >= 6:
            ax.set_xlabel("Frequency (Hz)")
    fig.tight_layout()
    fig.savefig(OUTPUT_DIR / "eval_error_vs_frequency.png", dpi=180)
    plt.close(fig)


def summarize(document, coefficients, delays, frequencies, radii, rows, scaling):
    statuses = []
    max_radius = float(np.max(radii))
    dense_with_dc = np.concatenate(([0.0], frequencies))
    full_db, full_section_db = evaluate_response(coefficients, dense_with_dc, target.FS)
    max_gain = float(np.max(full_db))
    max_section_gain = float(np.max(full_section_db))
    statuses.append(("PASS" if max_radius < POLE_LIMIT else "FAIL",
                     f"所有 section 稳定：最大 pole radius={max_radius:.8f}"))
    statuses.append(("PASS" if max_gain <= GAIN_TOLERANCE_DB else "FAIL",
                     f"所有 branch 无 >0 dB 放大：最大 gain={max_gain:.6f} dB"))
    statuses.append(("PASS" if max_section_gain <= GAIN_TOLERANCE_DB else "WARN",
                     f"单节最大 gain={max_section_gain:.6f} dB"))
    worst_rms = max(row["rms_db"] for row in rows)
    worst_max = max(row["max_abs_db"] for row in rows)
    statuses.append(("PASS" if worst_rms <= RMS_WARN_DB and worst_max <= MAX_WARN_DB else "WARN",
                     f"目标误差：最大 branch RMS={worst_rms:.3f} dB，最大点误差={worst_max:.3f} dB"))
    scaling_ok = (scaling["rms_db_at_reference_scale"] <= SCALING_RMS_WARN_DB
                  and scaling["max_abs_db_at_reference_scale"] <= SCALING_MAX_WARN_DB)
    scaling_message = (
        "实际响应按 delay 归一的偏差："
        f"RMS={scaling['rms_db_at_reference_scale']:.3f} dB，"
        f"max={scaling['max_abs_db_at_reference_scale']:.3f} dB "
        f"(换算到 L={int(max(delays))})"
    )
    statuses.append(("PASS" if scaling_ok else "WARN", scaling_message))
    parameter_error = parameter_scaling_error(document, delays)
    statuses.append(("PASS" if parameter_error <= 1e-9 else "FAIL",
                     f"导出参数 gain 按 L/Lref 缩放：最大偏差={parameter_error:.3g} dB"))
    worst_narrow = max(row["narrow_feature"]["max_abs_db"] for row in rows)
    statuses.append(("PASS" if worst_narrow <= NARROW_CURVATURE_WARN_DB else "WARN",
                     f"窄峰/窄 notch 局部曲率最大={worst_narrow:.3f} dB"))
    overall = "FAIL" if any(state == "FAIL" for state, _ in statuses) else (
        "WARN" if any(state == "WARN" for state, _ in statuses) else "PASS"
    )
    report = {
        "overall": overall,
        "checks": [{"status": status, "message": message} for status, message in statuses],
        "max_pole_radius": max_radius,
        "max_gain_db_0_to_nyquist": max_gain,
        "max_section_gain_db_0_to_nyquist": max_section_gain,
        "scaling": scaling,
        "branches": rows,
    }
    header = f"{overall}: v2a SOS 独立评估；这只检验 branch filter，不代替完整 FDN pole 分析或试听。"
    lines = [header] + [f"{status}: {message}" for status, message in statuses]
    lines += ["", "delay  RMS(dB)  max_abs(dB)  LF_RMS  MID_RMS  HF_RMS  pole_radius"]
    for row in rows:
        band = row["bands"]
        lines.append(
            f"{row['delay_samples']:5d}  {row['rms_db']:7.3f}  {row['max_abs_db']:11.3f}  "
            f"{band['low_20_300']['rms_db']:6.3f}  {band['mid_300_3000']['rms_db']:7.3f}  "
            f"{band['high_3000_nyquist']['rms_db']:6.3f}  {row['max_pole_radius']:11.6f}"
        )
    (OUTPUT_DIR / "eval_report.json").write_text(
        json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    (OUTPUT_DIR / "eval_report.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))
    print(f"输出目录：{OUTPUT_DIR}")


def main():
    document, delays, coefficients = load_coefficients(OUTPUT_DIR / "coefficients.json")
    wanted = target_response(FREQUENCIES, delays)
    fitted, _ = evaluate_response(coefficients, FREQUENCIES, target.FS)
    radii = pole_radii(coefficients)
    narrow = narrow_feature_curvature(fitted, FREQUENCIES)
    rows = branch_metrics(FREQUENCIES, wanted, fitted, delays, radii, narrow)
    scaling = scaling_metrics(fitted, delays)
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    plot_results(FREQUENCIES, wanted, fitted, delays)
    summarize(document, coefficients, delays, FREQUENCIES, radii, rows, scaling)


if __name__ == "__main__":
    main()
