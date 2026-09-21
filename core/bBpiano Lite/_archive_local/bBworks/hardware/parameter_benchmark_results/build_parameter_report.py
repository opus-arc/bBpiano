#!/usr/bin/env python3
"""Build the reviewed hammer-parameter optimization report artifact."""

from __future__ import annotations

import csv
import json
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parent


def read_modal_rows() -> tuple[list[dict], list[dict], list[dict]]:
    path = (
        ROOT
        / "recommended_string"
        / "single_v80_target"
        / "modal_fingerprint.csv"
    )
    with path.open(encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    target = []
    current = []
    residual = []
    for row in rows:
        shared = {
            "mode": int(row["mode"]),
            "frequency_hz": float(row["frequency_hz"]),
        }
        target.append({
            **shared,
            "energy_percent": 100.0 * float(row["target_fraction"]),
            "relative_db": float(row["target_db"]),
        })
        current.append({
            **shared,
            "energy_percent": 100.0 * float(row["candidate_fraction"]),
            "relative_db": float(row["candidate_db"]),
        })
        residual.append({
            **shared,
            "delta_db": float(row["delta_db"]),
        })
    return target, current, residual


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


def bar_chart(
    chart_id: str,
    title: str,
    subtitle: str,
    dataset: str,
    source_id: str,
    y_field: str,
    y_label: str,
) -> dict:
    return {
        "id": chart_id,
        "title": title,
        "subtitle": subtitle,
        "type": "bar",
        "dataset": dataset,
        "sourceId": source_id,
        "encodings": {
            "x": {
                "field": "mode",
                "type": "quantitative",
                "label": "Mode n",
            },
            "y": {
                "field": y_field,
                "type": "quantitative",
                "label": y_label,
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
                    "field": y_field,
                    "type": "quantitative",
                    "label": y_label,
                },
            ],
        },
    }


def main() -> None:
    generated_at = datetime.now(timezone.utc).isoformat()
    target_modal, current_modal, modal_residual = read_modal_rows()

    headline = [{
        "paired_rmse_db": 3.095318621673605,
        "contact_duration_ms": 5.600907029478456,
        "maximum_force_N": 10.0682570718,
        "maximum_energy_ratio": 1.0,
    }]
    score_comparison = [
        {"candidate": 1, "shape_rmse_db": 3.408233103355109},
        {"candidate": 2, "shape_rmse_db": 3.095318621673605},
        {"candidate": 3, "shape_rmse_db": 2.447157080660191},
        {"candidate": 4, "shape_rmse_db": 4.007095966315477},
    ]

    sources = [
        source(
            "paired_score_source",
            "recommended_string_paired_velocity_benchmark.csv",
            "Five-velocity transfer-cancelled comparison against the "
            "Pianoteq C4 recordings at MIDI 50, 65, 80, 95, and 110.",
        ),
        source(
            "modal_source",
            "recommended_string_v80_modal_fingerprint.csv",
            "Measured-frequency 13-mode force projection and microphone "
            "partial-power proxy for MIDI 80.",
        ),
        source(
            "force_source",
            "recommended_string_force_midi_80.txt",
            "MIDI-80 force log analyzed by hammer_analyzer.py.",
        ),
        source(
            "search_source",
            "all_parameter_searches",
            "Structured anchor, one-factor, Latin-hypercube, and local "
            "refinement searches across 1,660 parameter candidates.",
        ),
        source(
            "string_source",
            "steinway_rt425_c4_reference",
            "Read-only RT-425 C4 string row normalized to the bBworks "
            "length and fundamental.",
        ),
    ]

    cards = [
        {
            "id": "paired_rmse",
            "description": "Primary five-velocity modal-shape error after "
            "removing one constant gain offset per target recording.",
            "dataset": "headline",
            "sourceId": "paired_score_source",
            "metrics": [{
                "label": "Paired shape RMSE",
                "field": "paired_rmse_db",
                "format": "number",
                "unit": "dB",
            }],
        },
        {
            "id": "contact_duration",
            "description": "MIDI-80 duration above five percent of peak.",
            "dataset": "headline",
            "sourceId": "force_source",
            "metrics": [{
                "label": "Contact duration",
                "field": "contact_duration_ms",
                "format": "number",
                "unit": "ms",
            }],
        },
        {
            "id": "maximum_force",
            "description": "MIDI-80 maximum contact force.",
            "dataset": "headline",
            "sourceId": "force_source",
            "metrics": [{
                "label": "Maximum force",
                "field": "maximum_force_N",
                "format": "number",
                "unit": "N",
            }],
        },
        {
            "id": "energy_ratio",
            "description": "Largest hammer-plus-string mechanical energy "
            "divided by incident hammer energy.",
            "dataset": "headline",
            "sourceId": "force_source",
            "metrics": [{
                "label": "Maximum energy ratio",
                "field": "maximum_energy_ratio",
                "format": "number",
            }],
        },
    ]

    charts = [
        bar_chart(
            "target_modal",
            "Target modal-energy distribution",
            "Pianoteq C4 MIDI 80 microphone partial-power proxy; "
            "13 frequencies measured from the same WAV.",
            "target_modal",
            "modal_source",
            "energy_percent",
            "Target energy proxy (%)",
        ),
        bar_chart(
            "current_modal",
            "Current bBworks modal-energy distribution",
            "Force-only projection using the retained HammerModel and "
            "the physically referenced C4 linear density.",
            "current_modal",
            "modal_source",
            "energy_percent",
            "Projected energy (%)",
        ),
        bar_chart(
            "modal_residual",
            "MIDI-80 modal residual",
            "Positive means current projection is high relative to the "
            "microphone target after peak normalization.",
            "modal_residual",
            "modal_source",
            "delta_db",
            "Current minus target (dB)",
        ),
        {
            "id": "score_comparison",
            "title": "Paired five-velocity score comparison",
            "subtitle": "Lower is better. Candidate 3 is audio-only and "
            "fails contact duration; candidate 4 has real duration but "
            "no unloading branch.",
            "type": "bar",
            "dataset": "score_comparison",
            "sourceId": "search_source",
            "encodings": {
                "x": {
                    "field": "candidate",
                    "type": "quantitative",
                    "label": "Candidate ID",
                },
                "y": {
                    "field": "shape_rmse_db",
                    "type": "quantitative",
                    "label": "Shape RMSE (dB)",
                },
                "tooltip": [
                    {
                        "field": "candidate",
                        "type": "quantitative",
                        "label": "Candidate",
                    },
                    {
                        "field": "shape_rmse_db",
                        "type": "quantitative",
                        "label": "Shape RMSE (dB)",
                    },
                ],
            },
        },
    ]

    blocks = [
        {
            "id": "title",
            "type": "markdown",
            "body": "# Hammer parameter optimization report",
        },
        {
            "id": "summary",
            "type": "markdown",
            "sourceId": "search_source",
            "body": (
                "## Technical summary\n\n"
                "本轮没有找到可诚实写回的四参数“最终胜者”。当前 Stulov "
                "单指数模型在保持完整卸载支时，C4 接触时长稳定在约 "
                "`5.2–5.6 ms`；一旦参数把时长压到 `1.2–3.5 ms`，力峰全部"
                "落在最后一个正力样本，卸载冲量为零。扩大 `p`、`epsilon`、"
                "`tau0` 范围以及试验双支路 Maxwell 后，这个二分现象仍然"
                "存在。纯弹性硬度测试进一步显示：硬度高到足以明显缩短接触"
                "时，现有显式 hammer/string 耦合开始违反能量门禁。\n\n"
                "可以安全保留的改进只有 C4 弦参数：线密度改为 RT-425 参考值 "
                "`0.00623958204318 kg/m`，由 `f0` 与 `L` 推导张力 "
                "`721.778609 N`。它把五力度配对 RMSE 从 `3.408 dB` 降到 "
                "`3.095 dB`，同时所有力度保持能量比 `≤ 1.0`。"
            ),
        },
        {
            "id": "metrics",
            "type": "metric-strip",
            "cardIds": [
                "paired_rmse",
                "contact_duration",
                "maximum_force",
                "energy_ratio",
            ],
        },
        {
            "id": "a_model",
            "type": "markdown",
            "body": (
                "## A. 当前模型分析\n\n"
                "当前本构式为 "
                "`F(t)=K[u(t)^p−epsilon·h(t)]`，其中 "
                "`tau0·dh/dt+h=u^p`，压缩量以 mm 进入幂律。`p` 控制随压缩"
                "增加的硬化程度；`hardness=K` 同比例缩放瞬时刚度；"
                "`epsilon` 决定松弛支路相对量，越接近 1，静态刚度 "
                "`K(1−epsilon)` 越低且加载/卸载差异越强；`tau0` 决定历史"
                "跟随时间。代码用相邻采样间 `u^p` 的分段线性精确积分更新"
                "指数历史。\n\n"
                "接触状态机在 `compression<=0` 或 `rawForce<=0` 时释放。"
                "这条判据本身符合单边接触，但在显式耦合下无法稳定承载足够"
                "高的接触刚度。"
            ),
        },
        {
            "id": "b_literature",
            "type": "markdown",
            "body": (
                "## B. 文献依据\n\n"
                "- Stulov 的测量型参数示例为 `K=8800 N/mm^p, p=3.95, "
                "epsilon=0.992, tau0=2 µs`，测得 `0.72 m/s` 与 "
                "`1.31 m/s` 时接触约 `2.5 ms` 与 `1.6 ms`。同文也指出该"
                "参数存在等效组合，并给出短松弛近似 Model II。\n"
                "- Chabassier 等使用 `F=k·u^p+r·d(u^p)/dt` 的耗散接触模型，"
                "报告实用 `p` 范围约 `1.5–3.5`，并强调全系统需使用能量型"
                "离散耦合。\n"
                "- 钢琴击槌测量显示真实力脉冲并非对称半正弦，接触时长随"
                "峰值力增加而缩短；C4 静态幂律指数通常约 `2.5`。\n"
                "- Renner 的公开产品资料能确认毛毡结构与压制工艺，但没有"
                "发布可直接代入本模型的四参数测量表，因此不能把品牌名当作"
                "参数证据。"
            ),
        },
        {
            "id": "c_method",
            "type": "markdown",
            "sourceId": "paired_score_source",
            "body": (
                "## C. 测试方法\n\n"
                "沿用项目现有 `hammer_energy_guard.cpp`、"
                "`hammer_analyzer.py` 与 `hammer_modal_benchmark.py`。目标为"
                "同一 C4、同录音链的 MIDI `50/65/80/95/110` 五个 WAV；"
                "初速度来自生产代码已有的 Goebl 映射，并额外拟合一个全局"
                "速度比例。13 个模态频率只从 MIDI 80 目标测量一次，再固定"
                "用于所有力度。\n\n"
                "主评分是相对 MIDI 80 的逐模态增益变化误差：固定录音链的"
                "每模态传递函数在速度比值中抵消，再去除每个文件一个常量"
                "增益，计算 shape RMSE。单 WAV 条形图只是可复现的声学代理，"
                "不能解释为绝对 hammer-only 距离。"
            ),
        },
        {
            "id": "target_intro",
            "type": "markdown",
            "sourceId": "modal_source",
            "body": (
                "## 目标与当前模态分布\n\n"
                "下面按你的要求将目标与当前分开画成两张条形图。目标图仍含"
                "音板、辐射与麦克风传递；当前图是击槌力投影到无损、无色散"
                "理想弦。因此参数优化以五力度比值评分为主，而不是强迫这两张"
                "绝对图完全相同。"
            ),
        },
        {"id": "target_chart", "type": "chart", "chartId": "target_modal"},
        {"id": "current_chart", "type": "chart", "chartId": "current_modal"},
        {
            "id": "residual_text",
            "type": "markdown",
            "sourceId": "modal_source",
            "body": (
                "MIDI 80 单 WAV 代理中，当前第 `2/4/8/13` 模态偏高约 "
                "`+6.75/+7.64/+11.28/+10.94 dB`；第 `1/5/7/9` 模态偏低"
                "约 `−12.74/−9.69/−10.98/−20.68 dB`，第 6 模态最接近"
                "（`+0.96 dB`）。这些数值适合定位差异，不应脱离录音传递"
                "函数解释成真实弦模态能量误差。"
            ),
        },
        {"id": "residual_chart", "type": "chart", "chartId": "modal_residual"},
        {
            "id": "d_combinations",
            "type": "markdown",
            "sourceId": "search_source",
            "body": (
                "## D–E. 代表性参数组合与结果\n\n"
                "|ID|模型/用途|K|p|epsilon|tau0|mu|速度比例|RMSE|时长|"
                "Fmax|峰位|冲量|判定|\n"
                "|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|"
                "---:|---:|---:|---|\n"
                "|1|原始基线|400|2.4|0.51|0.400 ms|0.007000|1.000|"
                "3.408 dB|5.374 ms|10.793 N|0.398|0.02935 Ns|平滑但过长|\n"
                "|2|物理弦参数写回|400|2.4|0.51|0.400 ms|0.00623958|"
                "1.000|3.095 dB|5.601 ms|10.068 N|0.710|0.02899 Ns|"
                "评分提升，接触仍过长|\n"
                "|3|单指数纯音频最优|169.69|2.984|0.619|1.135 ms|"
                "0.00628355|1.082|2.447 dB|5.601 ms|10.574 N|0.407|"
                "0.03107 Ns|音频更近但物理门禁失败|\n"
                "|4|短接触候选|300|3.088|0.9927|0.00972 ms|0.00664126|"
                "0.884|4.007 dB|2.789 ms|10.177 N|1.000|0.00835 Ns|"
                "无卸载支，拒绝|\n"
                "|5|双支路音频最优试验|195.63|2.666|0.721|1.591 ms|"
                "0.00620422|0.846|2.419 dB|5.624 ms|8.078 N|0.407|"
                "0.02410 Ns|结构升级仍未缩短，已撤回|\n\n"
                "候选 1–4 的评分比较如下。"
            ),
        },
        {"id": "score_chart", "type": "chart", "chartId": "score_comparison"},
        {
            "id": "f_recommendation",
            "type": "markdown",
            "sourceId": "string_source",
            "body": (
                "## F. 推荐最终参数（当前阶段）\n\n"
                "四个击槌参数暂时保留，不宣称已经物理最优：\n\n"
                "```cpp\n"
                "hardness = 400.0;\n"
                "p = 2.4;\n"
                "epsilon = 0.51;\n"
                "tau0 = 0.0004;\n"
                "```\n\n"
                "已经写回且有独立物理依据的弦参数为：\n\n"
                "```cpp\n"
                "linear_density = 0.00623958204318; // kg/m\n"
                "tension = 721.778608814;            // N, derived\n"
                "```\n\n"
                "原因不是这四个击槌值已达终点，而是所有低评分替代值都违反"
                "至少一个重要物理条件。保留它们比写回峰值截断的伪最优更"
                "可靠。"
            ),
        },
        {
            "id": "required_change",
            "type": "markdown",
            "body": (
                "## 为继续收敛所需的下一处逻辑修改\n\n"
                "下一步需要改 `HammerModel.hpp::hammerMovement()` 中从"
                "`compression → process() → F → hammer update → "
                "injectForce()` 的显式顺序，改为接触端口的局部隐式/离散"
                "能量求解：在同一采样内联立下一时刻压缩、锤速度和波导端口"
                "速度，求得非负接触力后一次性提交历史状态。"
                "`StringModel` 的 delay、dispersion、loss、传播结构均不改；"
                "只需要提供接触点入射速度/阻抗用于这个局部方程。这样才能在"
                "更高真实刚度下保持被动性，并检验接触时长是否能落入真实"
                "范围。该修改尚未实施，等待授权。"
            ),
        },
        {
            "id": "limitations",
            "type": "markdown",
            "body": (
                "## 限制与稳健性\n\n"
                "- 目标来自 Pianoteq 麦克风输出，不是力传感器或弦速度"
                "测量；绝对模态分布包含目标系统的固定传递函数。\n"
                "- 每个力度只有一个 10 秒样本，无法估计击次方差。\n"
                "- MIDI 到击槌速度只有生产代码映射与一个全局比例，尚无该"
                "音频组的实测锤速标定。\n"
                "- 当前测试刻意不评估色散、损耗与音色后处理；这符合本轮"
                "hammer-only 目标，也意味着报告不能替代最终整琴听感验证。"
            ),
        },
    ]

    artifact = {
        "surface": "report",
        "manifest": {
            "version": 1,
            "surface": "report",
            "title": "Hammer parameter optimization report",
            "description": "bBworks C4 hammer parameter, force-curve, "
            "modal-fingerprint, and convergence analysis.",
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
                "target_modal": target_modal,
                "current_modal": current_modal,
                "modal_residual": modal_residual,
                "score_comparison": score_comparison,
            },
            "accessIssues": [],
        },
        "sources": sources,
    }

    output = ROOT / "hammer_parameter_report_artifact.json"
    output.write_text(
        json.dumps(artifact, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )
    print(output)


if __name__ == "__main__":
    main()
