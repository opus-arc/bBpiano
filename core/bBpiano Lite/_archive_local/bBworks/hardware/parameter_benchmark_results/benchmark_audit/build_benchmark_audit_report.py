#!/usr/bin/env python3
"""Build the benchmark-validity audit as a canonical portable report artifact."""

from __future__ import annotations

import csv
import json
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parent
ARTIFACT = ROOT / "benchmark_validity_audit_artifact.json"


def rows(name: str) -> list[dict]:
    with (ROOT / name).open(encoding="utf-8") as stream:
        result = []
        for row in csv.DictReader(stream):
            converted = {}
            for key, value in row.items():
                try:
                    converted[key] = float(value)
                except ValueError:
                    converted[key] = value
            result.append(converted)
        return result


def csv_source(source_id: str, filename: str, description: str) -> dict:
    return {
        "id": source_id,
        "query": {
            "engine": "duckdb",
            "language": "sql",
            "sql": f"SELECT * FROM read_csv_auto('{filename}');",
            "description": description,
            "executed_at": datetime.now(timezone.utc).isoformat(),
            "tables_used": [filename],
        },
    }


def bar_chart(
    chart_id: str,
    title: str,
    subtitle: str,
    dataset: str,
    source_id: str,
    x_field: str,
    x_label: str,
    y_field: str,
    y_label: str,
    tooltips: list[tuple[str, str]],
) -> dict:
    return {
        "id": chart_id,
        "title": title,
        "subtitle": subtitle,
        "type": "bar",
        "dataset": dataset,
        "sourceId": source_id,
        "encodings": {
            "x": {"field": x_field, "type": "quantitative", "label": x_label},
            "y": {"field": y_field, "type": "quantitative", "label": y_label},
            "tooltip": [
                {"field": field, "type": "quantitative", "label": label}
                for field, label in tooltips
            ],
        },
    }


def main() -> None:
    generated_at = datetime.now(timezone.utc).isoformat()
    direct = rows("direct_output_modes.csv")
    target = rows("target_estimator_reconciliation.csv")
    snr = rows("snr_threshold_sensitivity.csv")
    window = rows("window_sensitivity.csv")

    headline = [{
        "even_odd_peak_power_db": -105.3587,
        "max_ls_fft_difference_db": max(
            abs(float(row["ls_minus_fft100_db"])) for row in target
        ),
        "ls_condition_number": 1.000028,
        "reliable_mode_phase1_delta_db": next(
            float(row["phase1_minus_restored_db"])
            for row in snr
            if float(row["minimum_snr_db"]) == 0.0
        ),
    }]

    sources = [
        csv_source(
            "direct_source",
            "direct_output_modes.csv",
            "Restored StringModel::getSamples output, 500 ms Hann FFT, "
            "peak search within plus or minus 12 Hz of n times f0.",
        ),
        csv_source(
            "target_source",
            "target_estimator_reconciliation.csv",
            "Target C4 MIDI-80 WAV measured frequencies; 100 ms least-squares "
            "modal power reconciled against ordinary Hann-FFT peak power.",
        ),
        csv_source(
            "snr_source",
            "snr_threshold_sensitivity.csv",
            "Five-velocity score recomputed while varying the target-mode "
            "minimum-SNR eligibility threshold.",
        ),
        csv_source(
            "window_source",
            "window_sensitivity.csv",
            "Five-velocity score recomputed for 50, 100, 200, and 500 ms "
            "target analysis windows at the former minus-20 dB SNR threshold.",
        ),
    ]

    cards = [
        {
            "id": "parity_gap",
            "description": "Summed even-mode peak power relative to summed "
            "odd-mode peak power in the restored live StringModel output.",
            "dataset": "headline",
            "sourceId": "direct_source",
            "metrics": [{
                "label": "Even / odd peak power",
                "field": "even_odd_peak_power_db",
                "format": "number",
                "unit": "dB",
            }],
        },
        {
            "id": "estimator_error",
            "description": "Largest per-mode difference between the target "
            "least-squares estimate and a conventional 100 ms FFT peak.",
            "dataset": "headline",
            "sourceId": "target_source",
            "metrics": [{
                "label": "Max LS–FFT difference",
                "field": "max_ls_fft_difference_db",
                "format": "number",
                "unit": "dB",
            }],
        },
        {
            "id": "condition",
            "description": "Condition number of the target least-squares "
            "sinusoid design matrix; values near one are well separated.",
            "dataset": "headline",
            "sourceId": "target_source",
            "metrics": [{
                "label": "LS condition number",
                "field": "ls_condition_number",
                "format": "number",
            }],
        },
        {
            "id": "reliable_delta",
            "description": "Phase-1 score minus restored score when only "
            "target modes with non-negative measured SNR are retained.",
            "dataset": "headline",
            "sourceId": "snr_source",
            "metrics": [{
                "label": "Phase-1 penalty at SNR ≥ 0",
                "field": "reliable_mode_phase1_delta_db",
                "format": "number",
                "unit": "dB",
            }],
        },
    ]

    charts = [
        bar_chart(
            "direct_modes",
            "Live StringModel output contains only the odd-harmonic sequence",
            "Restored MIDI-80 direct output. Even-mode search bins are more "
            "than 105 dB below the summed odd-mode peaks.",
            "direct",
            "direct_source",
            "mode",
            "Mode n",
            "relative_peak_power_db",
            "Peak power relative to mode 1 (dB)",
            [
                ("mode", "Mode"),
                ("expected_frequency_hz", "Expected frequency (Hz)"),
                ("detected_peak_hz", "Detected peak (Hz)"),
                ("relative_peak_power_db", "Relative peak power (dB)"),
            ],
        ),
        bar_chart(
            "target_modes",
            "Target WAV modal levels are reproduced by an ordinary FFT",
            "The displayed target pattern is present in the WAV itself; "
            "least-squares extraction does not create the dips.",
            "target",
            "target_source",
            "mode",
            "Mode n",
            "least_squares_100ms_db",
            "Target power relative to mode 1 (dB)",
            [
                ("mode", "Mode"),
                ("measured_frequency_hz", "Measured frequency (Hz)"),
                ("least_squares_100ms_db", "Least-squares power (dB)"),
                ("fft_peak_100ms_db", "FFT peak power (dB)"),
                ("target_snr_db", "Target SNR (dB)"),
            ],
        ),
        bar_chart(
            "target_snr",
            "Most target modes above mode 7 are below the residual floor",
            "Negative SNR means the modal estimate is weaker than the local "
            "residual used by the benchmark.",
            "target",
            "target_source",
            "mode",
            "Mode n",
            "target_snr_db",
            "Measured target SNR (dB)",
            [
                ("mode", "Mode"),
                ("measured_frequency_hz", "Measured frequency (Hz)"),
                ("target_snr_db", "Target SNR (dB)"),
            ],
        ),
        bar_chart(
            "snr_sensitivity",
            "The phase-1 ranking reverses when low-SNR modes are excluded",
            "Positive values mean phase 1 is worse than the restored Stulov "
            "baseline; the former improvement appears only below −10 dB SNR.",
            "snr",
            "snr_source",
            "minimum_snr_db",
            "Minimum eligible target SNR (dB)",
            "phase1_minus_restored_db",
            "Phase 1 minus restored RMSE (dB)",
            [
                ("minimum_snr_db", "Minimum SNR (dB)"),
                ("restored_stulov_rmse_db", "Restored RMSE (dB)"),
                ("chabassier_phase1_rmse_db", "Phase-1 RMSE (dB)"),
                ("phase1_minus_restored_db", "Difference (dB)"),
            ],
        ),
        bar_chart(
            "window_sensitivity",
            "The former score advantage disappears in a 500 ms window",
            "All rows retain the former −20 dB SNR threshold; this is a "
            "sensitivity diagnostic, not a recommended score definition.",
            "window",
            "window_source",
            "window_ms",
            "Analysis window (ms)",
            "phase1_minus_restored_db",
            "Phase 1 minus restored RMSE (dB)",
            [
                ("window_ms", "Window (ms)"),
                ("restored_stulov_rmse_db", "Restored RMSE (dB)"),
                ("chabassier_phase1_rmse_db", "Phase-1 RMSE (dB)"),
                ("phase1_minus_restored_db", "Difference (dB)"),
            ],
        ),
    ]

    blocks = [
        {
            "id": "title",
            "type": "markdown",
            "body": "# Hammer modal benchmark validity audit",
        },
        {
            "id": "answer",
            "type": "markdown",
            "body": (
                "## 结论先行\n\n"
                "**上一阶段的 Chabassier 优势结论应撤回。** 当前 benchmark 的"
                "最主要问题不是最小二乘把相邻模态混在一起，而是它把碰撞力"
                "投影到一根具有完整 `n·f0` 模态的理想定端弦；实际 "
                "`StringModel` 的左右行波各自独立闭环，只形成 "
                "`(2k+1)·f0` 序列。两者比较的不是同一个系统。\n\n"
                "同时，原评分阈值允许目标信号低至残差以下 20 dB。前一阶段"
                "的分数提升由这些不可靠高阶模态驱动；只保留 `SNR ≥ 0 dB` "
                "的模态时，恢复后的 Stulov 基线反而优于 phase 1。"
            ),
        },
        {
            "id": "metrics",
            "type": "metric-strip",
            "cardIds": [
                "parity_gap",
                "estimator_error",
                "condition",
                "reliable_delta",
            ],
        },
        {
            "id": "restore",
            "type": "markdown",
            "body": (
                "## 还原范围\n\n"
                "已撤出本轮第一阶段的 Chabassier 接触模型、三弦力拆分、"
                "隐式求解与相应测试逻辑；`HammerModel.hpp` 恢复到对话前的 "
                "Stulov 形式。保留此前已经批准、且早于第一阶段存在的 C4 "
                "线密度 `0.00623958204318 kg/m`，以及旧测试入口所需的参数"
                "命令行覆盖。恢复后的能量门禁和 force-curve 指标与保存的"
                "前一阶段基线逐项一致。"
            ),
        },
        {
            "id": "friture",
            "type": "markdown",
            "body": (
                "## Friture 截图并不显示完整 partial 序列\n\n"
                "截图中的明显峰约为 `261.6, 784.9, 1308.1, 1831.5 Hz…`，"
                "相邻间隔约 `2·f0`。它们对应第 `1,3,5,7…` 模态，而不是"
                "第 `1,2,3,4…` 模态。用恢复后的实际 `StringModel::getSamples()` "
                "直接渲染再做 FFT，得到完全相同的奇数序列。"
            ),
        },
        {"id": "direct_chart", "type": "chart", "chartId": "direct_modes"},
        {
            "id": "root_cause",
            "type": "markdown",
            "body": (
                "## 已确认的结构原因\n\n"
                "`StringModel.hpp::propagate()` 在每次移动 rail head 后，"
                "分别对 `left[leftHead]` 与 `right[rightHead]` 做一次负反射，"
                "但两条 rail 在端点没有互相交叉连接。每条 rail 因而是一条"
                "带单次符号反转的独立长度 `D` 环路："
                "`H(z)=−z^(−D)`。谐振条件为 `ωD=(2k+1)π`，所以只留下"
                "奇数倍频。\n\n"
                "这不是击槌参数或 Chabassier/Stulov 本构能修复的现象。"
                "在修改弦边界前，任何 hammer-only 模态跑分都无法预测实际"
                "输出频谱。"
            ),
        },
        {
            "id": "estimator",
            "type": "markdown",
            "body": (
                "## “benchmark 把其他模态混进来”这一假设未得到支持\n\n"
                "在目标 WAV 的同一 100 ms 窗口内，least-squares 模态功率"
                "与普通 Hann FFT 的局部峰值逐模态相差不超过 `0.01 dB`；"
                "设计矩阵条件数为 `1.000028`，最大归一化列相关仅 "
                "`1.66×10⁻⁵`。benchmark 的自测试也达到 `100%` 相似度、"
                "`6.28×10⁻¹⁶` 能量恒等误差。提取器在它自己的理想弦假设"
                "下数值正确，问题是这个假设不对应当前 waveguide。"
            ),
        },
        {"id": "target_chart", "type": "chart", "chartId": "target_modes"},
        {"id": "snr_chart", "type": "chart", "chartId": "target_snr"},
        {
            "id": "score_validity",
            "type": "markdown",
            "body": (
                "## 原先跑分为何会给出错误方向\n\n"
                "目标第 `8–13` 模态多数处于负 SNR。原 `−20 dB` 门槛几乎"
                "把它们全部纳入优化，于是低于残差底的数值可以决定排名。"
                "在这个门槛下，phase 1 的 RMSE 为 `2.1947 dB`、旧模型为 "
                "`3.0953 dB`；但改用 `SNR ≥ 0 dB` 后，phase 1 为 "
                "`1.0843 dB`、旧模型为 `0.6500 dB`。结论发生反转。"
            ),
        },
        {"id": "snr_sensitivity_chart", "type": "chart", "chartId": "snr_sensitivity"},
        {"id": "window_chart", "type": "chart", "chartId": "window_sensitivity"},
        {
            "id": "assessment",
            "type": "markdown",
            "body": (
                "## Validation report\n\n"
                "### Overall assessment: Needs revision\n\n"
                "1. **Critical — 系统域不一致。** force projection 使用完整"
                "定端弦模态；实际 waveguide 只生成奇数序列，模型排名不能"
                "用于生产输出。\n"
                "2. **High — 评分受负 SNR 模态支配。** 这已经实际逆转"
                " phase-1 与恢复基线的优劣。\n"
                "3. **Medium — 时间窗敏感。** 在 `50–200 ms` 窗口出现的"
                "约 `0.9 dB` phase-1 优势，到 `500 ms` 仅剩 `0.0094 dB`。\n"
                "4. **Rejected hypothesis — 相邻模态混叠。** LS/FFT 对账、"
                "条件数和列相关均不支持这一解释。"
            ),
        },
        {
            "id": "next",
            "type": "markdown",
            "body": (
                "## 建议的下一步（尚未实施）\n\n"
                "1. 暂停 hammer 模型更换与参数搜索。\n"
                "2. 先经你批准，修复 `StringModel` 两个固定端的 rail "
                "交叉反射，使实际系统恢复完整模态序列；不同时修改 hammer、"
                "loss、dispersion 或 delay。\n"
                "3. 修复后增加一个一致性门禁：同一无损、无色散设置中，"
                "实际 `getSamples()` 的逐模态结果必须与 force projection "
                "在可解释误差内一致。\n"
                "4. 目标评分默认只使用 `SNR ≥ 0 dB` 的重复可见模态；"
                "负 SNR 模态只报告，不参与参数优化。\n"
                "5. 完成上述验证后，才重新比较 Stulov 与 Chabassier。"
            ),
        },
        {
            "id": "repro",
            "type": "markdown",
            "body": (
                "## Reproducibility\n\n"
                "审计输入与中间结果保存在本目录的四个 CSV 中；可执行、"
                "带输出的 notebook 为 `benchmark_validity_audit.ipynb`。"
                "本报告截至 2026-07-28，样本率 `44.1 kHz`，测试音为 C4 "
                "MIDI 80。实际输出 FFT 使用 500 ms Hann 窗，目标估计器"
                "对账使用 100 ms Hann 窗。"
            ),
        },
    ]

    artifact = {
        "surface": "report",
        "manifest": {
            "version": 1,
            "surface": "report",
            "title": "Hammer modal benchmark validity audit",
            "description": (
                "Audit of the bBworks hammer modal benchmark after restoring "
                "the pre-phase-1 contact model."
            ),
            "generatedAt": generated_at,
            "cards": cards,
            "charts": charts,
            "tables": [],
            "sources": sources,
            "blocks": blocks,
        },
        "snapshot": {
            "version": 1,
            "generatedAt": generated_at,
            "status": "ready",
            "datasets": {
                "headline": headline,
                "direct": direct,
                "target": target,
                "snr": snr,
                "window": window,
            },
            "accessIssues": [],
        },
        "sources": sources,
    }

    ARTIFACT.write_text(
        json.dumps(artifact, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )
    print(ARTIFACT)


if __name__ == "__main__":
    main()
