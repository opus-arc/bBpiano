#!/usr/bin/env python3
"""Build the canonical report artifact from reviewed energy-guard outputs."""

from __future__ import annotations

import csv
import json
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parent


def read_force_curve() -> list[dict]:
    with (ROOT / "force_v2p0.csv").open(encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    first_time = float(rows[0]["time"])
    return [
        {
            "time_ms": 1000.0 * (float(row["time"]) - first_time),
            "force_N": float(row["force"]),
        }
        for row in rows
    ]


def read_modal_energy() -> list[dict]:
    path = ROOT / "modal_v2p0" / "modal_fingerprint.csv"
    with path.open(encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    return [
        {
            "mode": int(row["mode"]),
            "frequency_hz": float(row["frequency_hz"]),
            "energy_percent": 100.0 * float(row["candidate_fraction"]),
            "relative_db": float(row["candidate_db"]),
        }
        for row in rows
    ]


def source(source_id: str, table: str, description: str) -> dict:
    return {
        "id": source_id,
        "query": {
            "engine": "duckdb",
            "language": "sql",
            "sql": f"SELECT * FROM read_csv_auto('{table}');",
            "description": description,
            "executed_at": datetime.now(timezone.utc).isoformat(),
            "tables_used": [table],
        },
    }


def main() -> None:
    generated_at = datetime.now(timezone.utc).isoformat()
    summary = [{
        "maximum_energy_ratio": 1.0,
        "release_energy_ratio": 0.203380444961,
        "contact_duration_ms": 5.260771,
        "maximum_force_N": 21.6469119845,
    }]
    force_curve = read_force_curve()
    modal_energy = read_modal_energy()

    sources = [
        source(
            "energy_guard_source",
            "hammer_energy_guard_result",
            "Direct 2 m/s energy audit of the corrected bBworks "
            "HammerModel and StringModel at 44.1 kHz.",
        ),
        source(
            "force_curve_source",
            "force_v2p0.csv",
            "Positive-force samples emitted by the corrected model and "
            "analyzed with hammer_analyzer.py.",
        ),
        source(
            "modal_source",
            "modal_fingerprint.csv",
            "Undamped, dispersion-free ideal-mode projection evaluated at "
            "the configured harmonic frequencies n*261.626 Hz.",
        ),
    ]

    cards = [
        {
            "id": "energy_ratio_card",
            "description": "Maximum observed hammer-plus-string energy "
            "divided by the 0.0300 J incident energy.",
            "dataset": "summary",
            "sourceId": "energy_guard_source",
            "metrics": [{
                "label": "Maximum energy ratio",
                "field": "maximum_energy_ratio",
                "format": "number",
            }],
        },
        {
            "id": "release_energy_card",
            "description": "String and fractional-allpass stored energy at "
            "first physical separation "
            "as a fraction of incident hammer energy.",
            "dataset": "summary",
            "sourceId": "energy_guard_source",
            "metrics": [{
                "label": "String energy at release",
                "field": "release_energy_ratio",
                "format": "percent",
            }],
        },
        {
            "id": "contact_duration_card",
            "description": "Force duration above 5 percent of peak.",
            "dataset": "summary",
            "sourceId": "energy_guard_source",
            "metrics": [{
                "label": "Contact duration",
                "field": "contact_duration_ms",
                "format": "number",
                "unit": "ms",
            }],
        },
        {
            "id": "maximum_force_card",
            "description": "Largest force in the corrected 2 m/s strike.",
            "dataset": "summary",
            "sourceId": "energy_guard_source",
            "metrics": [{
                "label": "Maximum force",
                "field": "maximum_force_N",
                "format": "number",
                "unit": "N",
            }],
        },
    ]

    charts = [
        {
            "id": "force_curve",
            "title": "Corrected hammer-string force curve",
            "subtitle": "2 m/s strike; time relative to first positive force sample.",
            "type": "line",
            "dataset": "force_curve",
            "sourceId": "force_curve_source",
            "encodings": {
                "x": {
                    "field": "time_ms",
                    "type": "quantitative",
                    "label": "Time (ms)",
                },
                "y": {
                    "field": "force_N",
                    "type": "quantitative",
                    "label": "Force (N)",
                },
                "tooltip": [
                    {
                        "field": "time_ms",
                        "type": "quantitative",
                        "label": "Time (ms)",
                    },
                    {
                        "field": "force_N",
                        "type": "quantitative",
                        "label": "Force (N)",
                    },
                ],
            },
        },
        {
            "id": "modal_energy",
            "title": "Ideal modal-energy fingerprint",
            "subtitle": "Modes 1-32 at n × 261.626 Hz; share of counterfactual ideal-string projected energy.",
            "type": "bar",
            "dataset": "modal_energy",
            "sourceId": "modal_source",
            "encodings": {
                "x": {
                    "field": "mode",
                    "type": "quantitative",
                    "label": "Mode n",
                },
                "y": {
                    "field": "energy_percent",
                    "type": "quantitative",
                    "label": "Projected energy (%)",
                },
                "tooltip": [
                    {
                        "field": "mode",
                        "type": "quantitative",
                        "label": "Mode",
                    },
                    {
                        "field": "frequency_hz",
                        "type": "quantitative",
                        "label": "Frequency (Hz)",
                    },
                    {
                        "field": "energy_percent",
                        "type": "quantitative",
                        "label": "Energy (%)",
                    },
                    {
                        "field": "relative_db",
                        "type": "quantitative",
                        "label": "Relative energy (dB)",
                    },
                ],
            },
        },
    ]

    blocks = [
        {
            "id": "title",
            "type": "markdown",
            "body": "# Hammer-string 能量修正与基准报告",
        },
        {
            "id": "technical_summary",
            "type": "markdown",
            "sourceId": "energy_guard_source",
            "body": (
                "## 结论摘要\n\n"
                "修正后的 bBworks 在 `2 m/s` 击槌下满足被动性：锤头动能、"
                "波导能量与分数延迟全通滤波器内部储能之和从未超过入射的 "
                "`0.0300 J`。此前测得的 `6.263×` 能量增长来自错误的时序："
                "新注入的行波先被传播离开接触端口，之后才用不再共轭的速度"
                "积分接触位移。能量错误已经消除；但当前力曲线仍不是最终物理"
                "拟合：5% 阈值接触时长 `5.261 ms`，峰值位于接触区间的 "
                "`73.64%`，并有三个明显波峰。因此四个毛毡参数仍需在修正后的"
                "耦合关系下重新优化。"
            ),
        },
        {
            "id": "headline_metrics",
            "type": "metric-strip",
            "cardIds": [
                "energy_ratio_card",
                "release_energy_card",
                "contact_duration_card",
                "maximum_force_card",
            ],
        },
        {
            "id": "energy_result",
            "type": "markdown",
            "sourceId": "energy_guard_source",
            "body": (
                "## 耦合模型不再产生机械能\n\n"
                "首次真实分离时，弦与全通滤波器共储存 `0.006101 J`，即入射"
                "能量的 `20.34%`。20 ms 后，回弹锤头含 `0.022003 J`，弦与"
                "全通状态含 `0.005729 J`，合计 `0.027732 J`（`92.44%`）。"
                "其余 `0.002268 J` 与毛毡迟滞及现有被动边界损耗一致。回弹"
                "速度为 `-1.71282 m/s`。"
            ),
        },
        {
            "id": "force_interpretation",
            "type": "markdown",
            "sourceId": "force_curve_source",
            "body": (
                "## 能量正确后仍可见三峰接触曲线\n\n"
                "三个局部峰值约为 `0.181 ms / 8.78 N`、`2.086 ms / "
                "21.61 N` 和 `3.991 ms / 21.65 N`。这不再是能量发散，而是"
                "一次连续接触内受到返波强烈调制。`5.261 ms` 的 5% 阈值时长、"
                "`0.7364` 的偏晚峰值位置，以及 `2.794` 的 rise/fall 比值均"
                "说明它还不能作为最终真实钢琴击槌拟合。"
            ),
        },
        {
            "id": "force_chart",
            "type": "chart",
            "chartId": "force_curve",
        },
        {
            "id": "modal_interpretation",
            "type": "markdown",
            "sourceId": "modal_source",
            "body": (
                "## 理想弦投影主要集中于第 2、4、6 模态\n\n"
                "在 `n × 261.626 Hz` 的无损、无色散投影中，第 2 模态占 "
                "`53.60%`，第 4 模态 `28.64%`，第 6 模态 `9.57%`；第 1 "
                "模态仅 `1.87%`，2/4/6 三个偶数模态合计 `91.82%`。这是候选"
                "指纹，尚不是相对目标的误差。只有接入同音、同录音链、多个"
                "已知击槌速度的目标 WAV 后，才能报告具体哪些模态偏高或偏低。"
            ),
        },
        {
            "id": "modal_chart",
            "type": "chart",
            "chartId": "modal_energy",
        },
        {
            "id": "scope",
            "type": "markdown",
            "body": (
                "## 本次修正内容\n\n"
                "- 在传播前使用接触点功率共轭速度积分击弦位移。\n"
                "- 压缩量和力仍为正时保持同一次接触，局部力回升不再误触发"
                "释放。\n"
                "- 修正 SpatialPort 线性插值权重及整数格点行为。\n"
                "- 新击槌把锤头位置重置到弦当前位置，避免继承旧压缩量。\n"
                "- 张力与阻抗由 `f0=261.626 Hz`、`L=0.65 m`、"
                "`mu=0.007 kg/m` 一致推导。\n"
                "- 左右边界分别使用独立的分数延迟全通滤波器状态。\n"
                "- 守恒测试加入全通滤波器的解析内部储能项。\n"
                "- NumPy 1.x/2.x 共用兼容的梯形积分路径。\n"
                "- 模态脚本不再把外力重放投影误标为闭合系统守恒测试。"
            ),
        },
        {
            "id": "method",
            "type": "markdown",
            "body": (
                "## 测试方法\n\n"
                "守恒测试直接运行现有 HammerModel 与 StringModel：44.1 kHz、"
                "15 g 锤头、2 m/s。行波能量为 "
                "`Z/fs × Σ(left² + right²)`；一阶全通状态 "
                "`s=x₁-a y₁` 的储能为 `Z/fs × s²/(1-a²)`。将它们与锤头"
                "动能相加，并在每个采样点与初始 `0.5mv²` 比较。同一力日志"
                "再由 `hammer_analyzer.py` 分析，并通过无损、无色散理想弦"
                "公式生成归一化模态指纹。"
            ),
        },
        {
            "id": "limitations",
            "type": "markdown",
            "body": (
                "## 模态投影的物理边界\n\n"
                "分数延迟全通滤波器使低频总延迟匹配配置的 C4，因此指纹使用"
                "名义频率 `n × 261.626 Hz`。32 模态投影的未归一化总量为 "
                "`0.030244 J`（入射能量的 `1.00813×`），但这不是守恒失败："
                "该计算把已生成的力脉冲作为外部执行器重放到另一根理想无损弦，"
                "其做功由那根理想弦的速度决定，并非原耦合系统的能量。闭合系统"
                "被动性只能由上面的直接耦合状态测试判定。模态投影在本基准中"
                "只用于比较归一化分布。"
            ),
        },
        {
            "id": "next_steps",
            "type": "markdown",
            "body": (
                "## 下一步建议\n\n"
                "1. 每一组参数实验都必须先通过本守恒测试。\n"
                "2. 在修正后的耦合上重新执行 hardness/p/epsilon/tau0 的结构化"
                "扫描；旧耦合下的排序不应沿用。\n"
                "3. 即使能量被动，也应淘汰接触过长、峰位过晚或重复波峰过强"
                "的候选。\n"
                "4. 通过这些门槛后，再接入多速度目标 WAV，输出逐模态 dB "
                "残差、偏高/偏低列表与总体距离。"
            ),
        },
        {
            "id": "questions",
            "type": "markdown",
            "body": (
                "## 尚未完成的目标比较\n\n"
                "当前没有目标 WAV 清单，因此本次只能确认模型被动性、力曲线"
                "形状和候选模态分布，不能诚实地给出“离目标还有多远”或判定"
                "具体模态偏高/偏低。脚本已支持同音、同录音链、多击槌速度的"
                "配对清单；拿到目标音频后即可执行该阶段。"
            ),
        },
    ]

    artifact = {
        "surface": "report",
        "manifest": {
            "version": 1,
            "surface": "report",
            "title": "Hammer-string 能量修正与基准报告",
            "description": "修正 bBworks hammer-string 接触耦合后的能量审计、力曲线与模态诊断。",
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
                "summary": summary,
                "force_curve": force_curve,
                "modal_energy": modal_energy,
            },
            "accessIssues": [],
        },
        "sources": sources,
    }

    output = ROOT / "energy_report_artifact.json"
    output.write_text(
        json.dumps(artifact, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )
    print(output)


if __name__ == "__main__":
    main()
