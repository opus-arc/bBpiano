#!/usr/bin/env python3
"""Build the post-waveguide-fix hammer optimization report artifact."""

from __future__ import annotations

import csv
import json
import re
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parent
VALIDATION = ROOT / "written_winner_validation"
SEARCH = ROOT / "search_refined_snr0"


def read_csv(path: Path) -> list[dict]:
    with path.open(encoding="utf-8") as stream:
        result = []
        for row in csv.DictReader(stream):
            converted = {}
            for key, value in row.items():
                if value in {"True", "False"}:
                    converted[key] = value == "True"
                    continue
                try:
                    converted[key] = float(value)
                except ValueError:
                    converted[key] = value
            result.append(converted)
        return result


def parse_analyzer(path: Path) -> dict:
    result = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        match = re.match(r"^([A-Za-z0-9_]+)\s*:\s*(.+)$", line)
        if match:
            result[match.group(1)] = float(match.group(2))
    return result


def parse_guard(path: Path) -> dict:
    result = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            try:
                result[key] = float(value.strip('"'))
            except ValueError:
                result[key] = value.strip('"')
    return result


def write_force_metrics() -> list[dict]:
    velocities = {
        50: 0.8072183648066608,
        65: 1.106749721551205,
        80: 1.6266395205247568,
        95: 2.666111600264876,
        110: 5.31723012667424,
    }
    rows = []
    for midi, velocity in velocities.items():
        analyzer = parse_analyzer(
            VALIDATION / f"analyzer_midi_{midi}.txt"
        )
        guard = parse_guard(VALIDATION / f"guard_midi_{midi}.txt")
        rows.append({
            "midi_velocity": midi,
            "impact_velocity_m_s": velocity,
            "contact_duration_ms": 1000.0
            * analyzer["contact_duration_s"],
            "maximum_force_N": analyzer["Fmax_N"],
            "peak_position_ratio": analyzer["peak_position_ratio"],
            "rise_fall_ratio": analyzer["rise_fall_ratio"],
            "impulse_Ns": analyzer["impulse_Ns"],
            "contact_event_count": int(analyzer["contact_event_count"]),
            "rebound_velocity_m_s": guard["rebound_velocity_m_s"],
            "maximum_energy_ratio": guard[
                "maximum_mechanical_energy_ratio"
            ],
        })
    path = VALIDATION / "force_metrics.csv"
    with path.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    return rows


def source(
    source_id: str,
    filename: str,
    description: str,
    sql_reader: str = "read_csv_auto",
    filters: list[str] | None = None,
) -> dict:
    query = {
        "engine": "duckdb",
        "language": "sql",
        "sql": f"SELECT * FROM {sql_reader}('{filename}');",
        "description": description,
        "executed_at": datetime.now(timezone.utc).isoformat(),
        "tables_used": [filename],
    }
    if filters:
        query["filters"] = filters
    return {"id": source_id, "query": query}


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
    winner_payload = json.loads(
        (SEARCH / "winner.json").read_text(encoding="utf-8")
    )
    winner = winner_payload["winner"]
    paired = json.loads(
        (
            VALIDATION
            / "paired_score"
            / "paired_velocity_benchmark.json"
        ).read_text(encoding="utf-8")
    )
    modal = read_csv(
        VALIDATION
        / "single_v80_target"
        / "modal_fingerprint.csv"
    )
    force = write_force_metrics()

    target_modal = [{
        "mode": int(row["mode"]),
        "frequency_hz": row["frequency_hz"],
        "energy_percent": 100.0 * row["target_fraction"],
        "relative_db": row["target_db"],
    } for row in modal]
    current_modal = [{
        "mode": int(row["mode"]),
        "frequency_hz": row["frequency_hz"],
        "energy_percent": 100.0 * row["candidate_fraction"],
        "relative_db": row["candidate_db"],
    } for row in modal]
    residual = [{
        "mode": int(row["mode"]),
        "frequency_hz": row["frequency_hz"],
        "current_minus_target_db": row["delta_db"],
    } for row in modal]

    midi_by_velocity = {
        0.8072183648066608: 50,
        1.106749721551205: 65,
        2.666111600264876: 95,
        5.31723012667424: 110,
    }
    per_velocity = [{
        "midi_velocity": midi_by_velocity[row["velocity_m_s"]],
        **row,
    } for row in paired["per_velocity"]]

    headline = [{
        "paired_rmse_db": paired["overall_shape_rmse_db"],
        "previous_rmse_db": 2.2851614361586776,
        "rmse_reduction_percent": 100.0 * (
            1.0
            - paired["overall_shape_rmse_db"]
            / 2.2851614361586776
        ),
        "midi80_contact_duration_ms": winner[
            "midi80_contact_duration_ms"
        ],
        "maximum_energy_ratio": winner["max_energy_ratio"],
        "candidate_count": winner_payload["candidate_count"],
    }]

    sources = [
        source(
            "winner_source",
            "search_refined_snr0/winner.json",
            "Selected candidate and search metadata after 7,051 evaluations.",
            sql_reader="read_json_auto",
            filters=[
                "minimum target modal SNR >= 0 dB",
                "five paired C4 velocities",
            ],
        ),
        source(
            "paired_source",
            (
                "written_winner_validation/paired_score/"
                "paired_velocity_benchmark.csv"
            ),
            "Transfer-cancelled five-velocity modal-shape benchmark.",
            filters=[
                "MIDI 80 is the baseline",
                "measured modal frequencies",
                "minimum target modal SNR >= 0 dB",
            ],
        ),
        source(
            "modal_source",
            (
                "written_winner_validation/single_v80_target/"
                "modal_fingerprint.csv"
            ),
            "MIDI-80 target and written-winner modal-energy proxies.",
            filters=[
                "100 ms target window",
                "13 measured C4 modes",
            ],
        ),
        source(
            "force_source",
            "written_winner_validation/force_metrics.csv",
            "Force-curve and energy-guard metrics regenerated from the "
            "parameters written into the source code.",
            filters=["MIDI velocities 50, 65, 80, 95, and 110"],
        ),
    ]

    cards = [
        {
            "id": "rmse",
            "description": "Transfer-cancelled modal-shape error across "
            "MIDI 50, 65, 95, and 110 relative to MIDI 80.",
            "dataset": "headline",
            "sourceId": "paired_source",
            "metrics": [
                {
                    "label": "Paired shape RMSE",
                    "field": "paired_rmse_db",
                    "format": "number",
                    "unit": "dB",
                },
                {
                    "label": "Before parameter refinement",
                    "field": "previous_rmse_db",
                    "format": "number",
                    "unit": "dB",
                },
            ],
        },
        {
            "id": "improvement",
            "description": "Relative reduction from the post-waveguide-fix "
            "current-parameter baseline.",
            "dataset": "headline",
            "sourceId": "paired_source",
            "metrics": [{
                "label": "RMSE reduction",
                "field": "rmse_reduction_percent",
                "format": "number",
                "unit": "%",
            }],
        },
        {
            "id": "duration",
            "description": "MIDI-80 duration above five percent of peak; "
            "the physical target gate remains 1.2–3.5 ms.",
            "dataset": "headline",
            "sourceId": "force_source",
            "metrics": [{
                "label": "Contact duration",
                "field": "midi80_contact_duration_ms",
                "format": "number",
                "unit": "ms",
            }],
        },
        {
            "id": "energy",
            "description": "Largest hammer-plus-string mechanical energy "
            "divided by incident hammer energy over all five velocities.",
            "dataset": "headline",
            "sourceId": "force_source",
            "metrics": [{
                "label": "Maximum energy ratio",
                "field": "maximum_energy_ratio",
                "format": "number",
            }],
        },
        {
            "id": "search_count",
            "description": "Deterministic, space-filling, and local-refinement "
            "candidates evaluated in the final search.",
            "dataset": "headline",
            "sourceId": "winner_source",
            "metrics": [{
                "label": "Candidates evaluated",
                "field": "candidate_count",
                "format": "number",
            }],
        },
    ]

    charts = [
        bar_chart(
            "target_modal",
            "Target MIDI-80 modal-energy distribution",
            "Pianoteq C4 microphone partial-power proxy at 13 measured "
            "frequencies; values sum to 100%.",
            "target_modal",
            "modal_source",
            "mode",
            "Mode n",
            "energy_percent",
            "Target energy proxy (%)",
            [
                ("mode", "Mode"),
                ("frequency_hz", "Frequency (Hz)"),
                ("energy_percent", "Energy proxy (%)"),
                ("relative_db", "Relative power (dB)"),
            ],
        ),
        bar_chart(
            "current_modal",
            "Written winner MIDI-80 modal-energy distribution",
            "Force projection into the lossless, dispersion-free ideal "
            "string using the code-written parameters.",
            "current_modal",
            "modal_source",
            "mode",
            "Mode n",
            "energy_percent",
            "Projected modal energy (%)",
            [
                ("mode", "Mode"),
                ("frequency_hz", "Frequency (Hz)"),
                ("energy_percent", "Projected energy (%)"),
                ("relative_db", "Relative energy (dB)"),
            ],
        ),
        bar_chart(
            "modal_residual",
            "Written winner MIDI-80 modal residual",
            "Positive means the force projection is high relative to the "
            "microphone target after peak normalization.",
            "residual",
            "modal_source",
            "mode",
            "Mode n",
            "current_minus_target_db",
            "Current minus target (dB)",
            [
                ("mode", "Mode"),
                ("frequency_hz", "Frequency (Hz)"),
                ("current_minus_target_db", "Residual (dB)"),
            ],
        ),
        bar_chart(
            "velocity_rmse",
            "Paired modal-shape error by MIDI velocity",
            "Each velocity is compared with MIDI 80 after removing one "
            "constant gain offset; lower is better.",
            "per_velocity",
            "paired_source",
            "midi_velocity",
            "MIDI velocity",
            "shape_rmse_db",
            "Shape RMSE (dB)",
            [
                ("midi_velocity", "MIDI velocity"),
                ("velocity_m_s", "Impact velocity (m/s)"),
                ("valid_mode_count", "Eligible modes"),
                ("shape_rmse_db", "Shape RMSE (dB)"),
            ],
        ),
    ]

    force_rows = "\n".join(
        "|{midi_velocity:.0f}|{impact_velocity_m_s:.3f}|"
        "{contact_duration_ms:.3f}|{maximum_force_N:.3f}|"
        "{peak_position_ratio:.3f}|{rise_fall_ratio:.3f}|"
        "{impulse_Ns:.5f}|{rebound_velocity_m_s:.4f}|".format(**row)
        for row in force
    )

    blocks = [
        {
            "id": "title",
            "type": "markdown",
            "body": "# Hammer parameter optimization report — post waveguide fix",
        },
        {
            "id": "summary",
            "type": "markdown",
            "sourceId": "winner_source",
            "body": (
                "## 技术结论：频谱评分显著提高，但接触时长限制仍未解决\n\n"
                "在修复 StringModel 的端点交叉反射与共点力注入后，共测试 "
                "`7,051` 组参数。没有候选同时达到 `1.2–3.5 ms` 接触时长"
                "并保持完整卸载与负回弹。因此按预先声明的回退规则，从"
                "能量守恒、单次接触、完整卸载、负回弹、合理 Fmax/峰位"
                "候选中选择 paired RMSE 最低者，并已写入程序。\n\n"
                "最终 RMSE 为 `1.1266 dB`，相对修复波导后的当前参数 "
                "`2.2852 dB` 降低 `50.7%`。但 MIDI 80 接触仍为 "
                "`7.778 ms`，不能宣称已经得到实验意义上的真实击槌接触"
                "时长。"
            ),
        },
        {
            "id": "metrics",
            "type": "metric-strip",
            "cardIds": [
                "rmse",
                "improvement",
                "duration",
                "energy",
                "search_count",
            ],
        },
        {
            "id": "parameters",
            "type": "markdown",
            "sourceId": "winner_source",
            "body": (
                "## 已写入的最终参数\n\n"
                "```cpp\n"
                "hardness = 109.87459216355096;\n"
                "p = 1.5;\n"
                "epsilon = 0.8104484280879438;\n"
                "tau0 = 8.741820760093148e-06;\n"
                "linear_density = 0.006751517084270981; // kg/m\n"
                "tension = 780.9979217748913;            // N, derived\n"
                "midi80_impact_velocity = 1.6266395205247568; // m/s\n"
                "```\n\n"
                "`p` 与 hardness 都接近搜索下界，说明当前模型依赖非常软、"
                "近线性的等效毛毡来拟合力度间模态变化；它是模型补偿，不应"
                "直接解释为独立材料测量值。"
            ),
        },
        {
            "id": "modal_intro",
            "type": "markdown",
            "body": (
                "## 目标与当前模态分布\n\n"
                "以下两图分别展示目标和写入参数后的 MIDI-80 分布。两图"
                "可用于定位具体模态，但目标仍包含音板、辐射与麦克风传递，"
                "所以正式排名使用五力度相对变化，而不是强迫单个绝对分布"
                "完全重合。"
            ),
        },
        {"id": "target_chart", "type": "chart", "chartId": "target_modal"},
        {"id": "current_chart", "type": "chart", "chartId": "current_modal"},
        {
            "id": "residual_intro",
            "type": "markdown",
            "sourceId": "modal_source",
            "body": (
                "## 单力度残差仍集中在高阶低 SNR 模态\n\n"
                "MIDI 80 中，第 `5/6/9/10` 模态的投影偏低，第 `8/11/12/13` "
                "模态偏高。第 8 及以上模态多数曾在目标残差底附近或以下，"
                "因此这些绝对残差只作诊断，不进入 `SNR ≥ 0 dB` 的配对"
                "优化结论。"
            ),
        },
        {"id": "residual_chart", "type": "chart", "chartId": "modal_residual"},
        {
            "id": "velocity_intro",
            "type": "markdown",
            "sourceId": "paired_source",
            "body": (
                "## 低中力度拟合最好，MIDI 110 是主要剩余误差\n\n"
                "相对 MIDI 80，MIDI `65` 与 `95` 的 shape RMSE 分别只有 "
                "`0.477` 与 `0.542 dB`；MIDI `50` 为 `1.168 dB`，MIDI "
                "`110` 上升至 `1.790 dB`。因此总体误差的主要剩余来源是"
                "最高力度下的非线性变化。"
            ),
        },
        {"id": "velocity_chart", "type": "chart", "chartId": "velocity_rmse"},
        {
            "id": "force",
            "type": "markdown",
            "sourceId": "force_source",
            "body": (
                "## 五力度力曲线均稳定，但接触时长几乎不随力度缩短\n\n"
                "|MIDI|锤速 m/s|时长 ms|Fmax N|峰位|rise/fall|冲量 Ns|"
                "回弹速度 m/s|\n"
                "|---:|---:|---:|---:|---:|---:|---:|---:|\n"
                f"{force_rows}\n\n"
                "所有力度均为单次接触、负回弹，最大机械能比均为 `1.0`。"
                "但是接触时长保持在约 `7.71–7.80 ms`，与真实钢琴通常随"
                "力度上升而缩短的行为不符。"
            ),
        },
        {
            "id": "scope",
            "type": "markdown",
            "body": (
                "## 数据、评分与选择规则\n\n"
                "目标为同一 C4、同一 Pianoteq 录音链的 MIDI "
                "`50/65/80/95/110` 五个 WAV。13 个频率从 MIDI 80 测量"
                "一次并固定；只允许目标与基线都达到 `SNR ≥ 0 dB` 的模态"
                "参与评分。候选响应来自 hammer force 投影到无损、无色散"
                "理想弦。每个力度相对 MIDI 80 计算逐模态增益，再删除一个"
                "常量增益偏置后汇总 shape RMSE。"
            ),
        },
        {
            "id": "method",
            "type": "markdown",
            "sourceId": "winner_source",
            "body": (
                "## 搜索方法与稳健性检查\n\n"
                "搜索包括确定性锚点与单因素扫描、`2,500` 组无 SciPy 依赖"
                "的拉丁超立方，以及围绕 30 个低误差/近门禁/最短负回弹种子"
                "的 `4,500` 组局部精炼。每个候选都在五个力度运行现有能量"
                "门禁、force analyzer 与 paired modal benchmark。写回后"
                "重新编译并仅使用代码默认值复现全部五个 force log，RMSE "
                "与搜索结果逐位一致。"
            ),
        },
        {
            "id": "limitations",
            "type": "markdown",
            "body": (
                "## 限制与验证结论：Share with caveats\n\n"
                "- 没有候选通过完整 `1.2–3.5 ms` 接触时长门禁；写入的是"
                "具有完整回弹曲线的最低 RMSE 候选，而不是物理全合格候选。\n"
                "- 目标是 Pianoteq 麦克风输出，不是实测 hammer force 或"
                "弦速度；paired 方法只能抵消固定传递函数，不能把目标变成"
                "绝对材料测量。\n"
                "- 每个 MIDI 力度只有一个目标文件，无法估计击次方差。\n"
                "- `p=1.5` 和 hardness 接近搜索边界；继续放宽范围可能降低"
                "分数，但物理可解释性会更弱。\n"
                "- 本次结论只针对 C4 和当前修复后的 StringModel。"
            ),
        },
        {
            "id": "next",
            "type": "markdown",
            "body": (
                "## 推荐下一步\n\n"
                "保留本次写入参数作为当前最佳声学基线，不再通过继续扩大"
                "四参数范围追求虚假收敛。下一阶段应把“为什么完整负回弹总在"
                "约 7.8 ms”作为独立模型问题：先验证接触点返回波与位移状态，"
                "再决定是否需要升级为 Chabassier 型能量一致接触模型。任何"
                "结构修改都应与本报告基线进行同一五力度回归。"
            ),
        },
        {
            "id": "questions",
            "type": "markdown",
            "body": (
                "## 后续需要回答的问题\n\n"
                "1. 约 `7.8 ms` 的平台是否由接触状态历史、固定端返回波，"
                "还是 Stulov 单松弛支路本身造成？\n"
                "2. 使用真实钢琴 C4 的锤速与弦速度测量后，当前 "
                "`velocity_scale=1.546` 是否仍成立？\n"
                "3. MIDI 110 的剩余非线性误差是否需要第二松弛时间，还是"
                "目标录音链在高力度下并非完全固定？"
            ),
        },
    ]

    artifact = {
        "surface": "report",
        "manifest": {
            "version": 1,
            "surface": "report",
            "title": (
                "Hammer parameter optimization report — post waveguide fix"
            ),
            "description": (
                "Final C4 hammer-parameter search, code writeback, and "
                "validation after correcting the string waveguide."
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
                "target_modal": target_modal,
                "current_modal": current_modal,
                "residual": residual,
                "per_velocity": per_velocity,
                "force": force,
            },
            "accessIssues": [],
        },
        "sources": sources,
    }

    output = ROOT / "final_report_artifact.json"
    output.write_text(
        json.dumps(artifact, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )
    print(output)


if __name__ == "__main__":
    main()
