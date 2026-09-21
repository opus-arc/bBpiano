#!/usr/bin/env python3
"""
Fit a stable allpass dispersion filter for a stiff-string digital waveguide.

The target partials are generated from:

    f_n = n * F0 * sqrt(1 + B * n^2)
    F0  = f1 / sqrt(1 + B)

The fitted filter is a stable cascade of second-order allpass sections.  The
fit and verification do not unwrap sparse phase samples.  Instead, continuous
allpass phase is obtained by integrating the positive group delay of the stable
sections:

    phi(w) = -integral_0^w tau(u) du

Loop resonance verification solves:

    f * loop_delay / fs + integral_0^w tau(u) du / (2*pi) = n

This is the actual waveguide resonance condition after retuning partial 1.
"""

from __future__ import annotations

import argparse
import dataclasses
import math
import sys
import time
from typing import Iterable

import numpy as np
from numpy.polynomial.legendre import leggauss
from scipy.optimize import brentq, least_squares


NYQUIST_FRACTION = 0.49


@dataclasses.dataclass(frozen=True)
class TargetSet:
    sample_rate: float
    f1: float
    b: float
    f0: float
    partials: np.ndarray
    freqs: np.ndarray
    omegas: np.ndarray


@dataclasses.dataclass(frozen=True)
class AllpassDesign:
    sample_rate: float
    order: int
    radii: np.ndarray
    angles: np.ndarray
    first_order_pole: float | None
    cost: float
    max_fit_cent_residual: float
    rms_fit_cent_residual: float
    nfev: int
    elapsed_sec: float


def flexible_f0_from_first_partial(f1: float, b: float) -> float:
    return f1 / math.sqrt(1.0 + b)


def target_frequency(f1: float, b: float, n: int) -> float:
    f0 = flexible_f0_from_first_partial(f1, b)
    return n * f0 * math.sqrt(1.0 + b * n * n)


def cents_error(predicted: float, target: float) -> float:
    return 1200.0 * math.log2(predicted / target)


def build_targets(
    sample_rate: float,
    f1: float,
    b: float,
    max_partial: int,
) -> TargetSet:
    if sample_rate <= 0.0:
        raise ValueError("sample_rate must be positive")
    if f1 <= 0.0:
        raise ValueError("f1 must be positive")
    if b < 0.0:
        raise ValueError("B must be non-negative")
    if max_partial < 1:
        raise ValueError("max_partial must be >= 1")

    f0 = flexible_f0_from_first_partial(f1, b)
    partials: list[int] = []
    freqs: list[float] = []
    max_freq = sample_rate * NYQUIST_FRACTION

    for n in range(1, max_partial + 1):
        freq = target_frequency(f1, b, n)
        if freq >= max_freq:
            continue
        partials.append(n)
        freqs.append(freq)

    if not partials or partials[0] != 1:
        raise ValueError("partial 1 is not below the usable Nyquist limit")
    if len(partials) < 2:
        raise ValueError("need at least two usable target partials")

    freqs_array = np.array(freqs, dtype=float)
    return TargetSet(
        sample_rate=sample_rate,
        f1=f1,
        b=b,
        f0=f0,
        partials=np.array(partials, dtype=float),
        freqs=freqs_array,
        omegas=2.0 * math.pi * freqs_array / sample_rate,
    )


def group_delay_sos(
    omega: np.ndarray,
    radii: np.ndarray,
    angles: np.ndarray,
    first_order_pole: float | None = None,
) -> np.ndarray:
    """Positive group delay, in samples, for the stable allpass cascade."""
    tau = np.zeros_like(omega, dtype=float)

    for radius, angle in zip(radii, angles):
        radius2 = radius * radius
        numerator = 1.0 - radius2
        tau += numerator / (1.0 - 2.0 * radius * np.cos(omega - angle) + radius2)
        tau += numerator / (1.0 - 2.0 * radius * np.cos(omega + angle) + radius2)

    if first_order_pole is not None:
        pole = first_order_pole
        pole2 = pole * pole
        tau += (1.0 - pole2) / (1.0 - 2.0 * pole * np.cos(omega) + pole2)

    return tau


class TargetPhaseIntegrator:
    """Gauss-Legendre integration on intervals ending at the target partials."""

    def __init__(self, target: TargetSet, quadrature_points: int) -> None:
        if quadrature_points < 16:
            raise ValueError("quadrature_points should be at least 16")

        xg, wg = leggauss(quadrature_points)
        bounds = np.concatenate(([0.0], target.omegas))
        lo = bounds[:-1]
        hi = bounds[1:]

        self.nodes = (
            0.5 * (hi[:, None] + lo[:, None])
            + 0.5 * (hi[:, None] - lo[:, None]) * xg[None, :]
        )
        self.weights = 0.5 * (hi[:, None] - lo[:, None]) * wg[None, :]
        self.omegas = target.omegas

    def phase_integrals(
        self,
        radii: np.ndarray,
        angles: np.ndarray,
        first_order_pole: float | None,
    ) -> np.ndarray:
        tau = group_delay_sos(self.nodes, radii, angles, first_order_pole)
        interval_integrals = np.sum(tau * self.weights, axis=1)
        return np.cumsum(interval_integrals)

    def phase_delays(
        self,
        radii: np.ndarray,
        angles: np.ndarray,
        first_order_pole: float | None,
    ) -> np.ndarray:
        return self.phase_integrals(radii, angles, first_order_pole) / self.omegas


def unpack_params(
    params: np.ndarray,
    num_biquads: int,
    has_first_order: bool,
) -> tuple[np.ndarray, np.ndarray, float | None]:
    radii = params[:num_biquads]
    angles = params[num_biquads : 2 * num_biquads]

    first_order_pole = None
    if has_first_order:
        first_order_pole = float(params[-1])

    return radii, angles, first_order_pole


def resonance_cent_residuals(
    target: TargetSet,
    integrator: TargetPhaseIntegrator,
    params: np.ndarray,
    num_biquads: int,
    has_first_order: bool,
    weight_power: float,
) -> np.ndarray:
    radii, angles, first_order_pole = unpack_params(
        params,
        num_biquads,
        has_first_order,
    )
    phase_delays = integrator.phase_delays(radii, angles, first_order_pole)
    loop_delay = target.sample_rate / target.f1 - phase_delays[0]

    # At the target frequencies, this is zero exactly when the loop resonance
    # equation is satisfied for the requested mode number.
    cycle_error = (
        target.freqs[1:] * (loop_delay + phase_delays[1:]) / target.sample_rate
        - target.partials[1:]
    )

    cent_like = (1200.0 / math.log(2.0)) * cycle_error / target.partials[1:]
    if weight_power != 0.0:
        weights = (target.partials[1:] / target.partials[-1]) ** weight_power
        cent_like = cent_like * weights

    return cent_like


def initial_angle_sets(num_biquads: int, omega1: float, omega_max: float) -> list[np.ndarray]:
    if num_biquads == 0:
        return [np.array([], dtype=float)]

    lo = max(omega1 * 0.15, 1.0e-5)
    hi = min(omega_max * 0.98, math.pi - 1.0e-5)
    k = np.arange(num_biquads, dtype=float)

    sets = [
        np.geomspace(lo, hi, num_biquads),
        np.geomspace(max(omega1 * 0.50, 1.0e-5), hi, num_biquads),
        np.linspace(lo, hi, num_biquads),
        np.linspace(max(omega1, 1.0e-5), hi, num_biquads),
        lo + (hi - lo) * (1.0 - np.cos((k + 0.5) * math.pi / num_biquads)) / 2.0,
    ]
    return sets


def make_initial_guesses(
    order: int,
    target: TargetSet,
    starts: int,
    max_radius: float,
    seed: int,
) -> list[np.ndarray]:
    num_biquads = order // 2
    has_first_order = (order % 2) == 1
    guesses: list[np.ndarray] = []

    angle_sets = initial_angle_sets(num_biquads, target.omegas[0], target.omegas[-1])
    for angles in angle_sets:
        for radius in (0.70, 0.86, 0.94, min(0.985, max_radius * 0.995)):
            radii = np.full(num_biquads, min(radius, max_radius * 0.995))
            parts = [radii, angles]
            if has_first_order:
                parts.append(np.array([min(0.75, max_radius * 0.995)]))
            guesses.append(np.concatenate(parts))
            if len(guesses) >= starts:
                return guesses

    rng = np.random.default_rng(seed)
    lo_angle = max(target.omegas[0] * 0.12, 1.0e-5)
    hi_angle = min(target.omegas[-1], math.pi - 1.0e-5)

    while len(guesses) < starts:
        radii = rng.uniform(0.45, max_radius * 0.998, num_biquads)
        if num_biquads:
            angles = np.sort(
                np.exp(rng.uniform(np.log(lo_angle), np.log(hi_angle), num_biquads))
            )
        else:
            angles = np.array([], dtype=float)
        parts = [radii, angles]
        if has_first_order:
            parts.append(np.array([rng.uniform(0.0, max_radius * 0.998)]))
        guesses.append(np.concatenate(parts))

    return guesses


def fit_allpass(
    target: TargetSet,
    order: int,
    starts: int,
    max_nfev: int,
    max_radius: float,
    quadrature_points: int,
    weight_power: float,
    radius_regularization: float,
    seed: int,
    verbose: bool,
) -> AllpassDesign:
    if order < 1:
        raise ValueError("order must be >= 1")
    if not (0.0 < max_radius < 1.0):
        raise ValueError("max_radius must be between 0 and 1")
    if starts < 1:
        raise ValueError("starts must be >= 1")

    num_biquads = order // 2
    has_first_order = (order % 2) == 1
    integrator = TargetPhaseIntegrator(target, quadrature_points)

    lower = np.concatenate(
        (
            np.full(num_biquads, 1.0e-4),
            np.full(num_biquads, 1.0e-5),
            np.array([0.0]) if has_first_order else np.array([], dtype=float),
        )
    )
    upper = np.concatenate(
        (
            np.full(num_biquads, max_radius),
            np.full(num_biquads, math.pi - 1.0e-5),
            np.array([max_radius]) if has_first_order else np.array([], dtype=float),
        )
    )

    def residual(params: np.ndarray) -> np.ndarray:
        cent_residual = resonance_cent_residuals(
            target,
            integrator,
            params,
            num_biquads,
            has_first_order,
            weight_power,
        )
        if radius_regularization <= 0.0:
            return cent_residual

        radii, _, first_order_pole = unpack_params(params, num_biquads, has_first_order)
        reg_values = [radii - 0.85]
        if first_order_pole is not None:
            reg_values.append(np.array([first_order_pole - 0.70]))
        return np.concatenate((cent_residual, radius_regularization * np.concatenate(reg_values)))

    guesses = make_initial_guesses(order, target, starts, max_radius, seed)
    best_result = None
    best_score = float("inf")
    best_unweighted = None
    start_time = time.perf_counter()

    for index, guess in enumerate(guesses, start=1):
        x0 = np.clip(guess, lower + 1.0e-8, upper - 1.0e-8)
        result = least_squares(
            residual,
            x0,
            bounds=(lower, upper),
            method="trf",
            x_scale="jac",
            loss="linear",
            max_nfev=max_nfev,
            ftol=1.0e-10,
            xtol=1.0e-10,
            gtol=1.0e-10,
        )

        unweighted = resonance_cent_residuals(
            target,
            integrator,
            result.x,
            num_biquads,
            has_first_order,
            weight_power=0.0,
        )
        max_abs = float(np.max(np.abs(unweighted)))
        rms = float(np.sqrt(np.mean(unweighted * unweighted)))
        score = max_abs + 0.10 * rms

        if verbose:
            print(
                f"start {index:02d}/{len(guesses)}: "
                f"nfev={result.nfev:4d}, maxFitCent={max_abs:9.4f}, "
                f"rmsFitCent={rms:9.4f}",
                file=sys.stderr,
            )

        if score < best_score:
            best_score = score
            best_result = result
            best_unweighted = unweighted

    if best_result is None or best_unweighted is None:
        raise RuntimeError("least_squares did not produce a result")

    radii, angles, first_order_pole = unpack_params(
        best_result.x,
        num_biquads,
        has_first_order,
    )
    elapsed = time.perf_counter() - start_time

    return AllpassDesign(
        sample_rate=target.sample_rate,
        order=order,
        radii=np.array(radii, dtype=float),
        angles=np.array(angles, dtype=float),
        first_order_pole=first_order_pole,
        cost=float(best_result.cost),
        max_fit_cent_residual=float(np.max(np.abs(best_unweighted))),
        rms_fit_cent_residual=float(np.sqrt(np.mean(best_unweighted * best_unweighted))),
        nfev=int(best_result.nfev),
        elapsed_sec=elapsed,
    )


class PhaseDelayTable:
    """Dense continuous phase-integral table used for final resonance solving."""

    def __init__(
        self,
        design: AllpassDesign,
        grid_size: int,
        max_freq: float,
    ) -> None:
        if grid_size < 4096:
            raise ValueError("verify grid should be at least 4096")

        self.sample_rate = design.sample_rate
        self.max_freq = max_freq
        self.omega = np.linspace(0.0, 2.0 * math.pi * max_freq / design.sample_rate, grid_size)
        self.freq = self.omega * design.sample_rate / (2.0 * math.pi)
        self.tau = group_delay_sos(
            self.omega,
            design.radii,
            design.angles,
            design.first_order_pole,
        )
        self.integral = np.zeros_like(self.omega)
        d_omega = np.diff(self.omega)
        self.integral[1:] = np.cumsum(0.5 * (self.tau[1:] + self.tau[:-1]) * d_omega)

    def phase_integral_at(self, freq_hz: float) -> float:
        if freq_hz <= 0.0:
            return 0.0
        if freq_hz > self.max_freq:
            raise ValueError(f"frequency {freq_hz} exceeds phase table max {self.max_freq}")
        return float(np.interp(freq_hz, self.freq, self.integral))

    def phase_delay_at(self, freq_hz: float) -> float:
        if freq_hz <= 0.0:
            return float(self.tau[0])
        omega = 2.0 * math.pi * freq_hz / self.sample_rate
        return self.phase_integral_at(freq_hz) / omega

    def group_delay_at(self, freq_hz: float) -> float:
        if freq_hz <= 0.0:
            return float(self.tau[0])
        return float(np.interp(freq_hz, self.freq, self.tau))


def solve_predicted_partial(
    n: int,
    loop_delay: float,
    phase_table: PhaseDelayTable,
    sample_rate: float,
) -> float:
    def resonance_error(freq_hz: float) -> float:
        phase_cycles = phase_table.phase_integral_at(freq_hz) / (2.0 * math.pi)
        delay_cycles = freq_hz * loop_delay / sample_rate
        return delay_cycles + phase_cycles - n

    lo = 0.0
    hi = phase_table.max_freq
    if resonance_error(hi) < 0.0:
        return float("nan")

    return float(brentq(resonance_error, lo, hi, xtol=1.0e-10, rtol=1.0e-12, maxiter=100))


def denominator_coefficients(design: AllpassDesign) -> np.ndarray:
    coeff = np.array([1.0], dtype=float)
    for radius, angle in zip(design.radii, design.angles):
        section = np.array([1.0, -2.0 * radius * math.cos(angle), radius * radius])
        coeff = np.convolve(coeff, section)
    if design.first_order_pole is not None:
        coeff = np.convolve(coeff, np.array([1.0, -design.first_order_pole]))
    return coeff


def pole_radii(design: AllpassDesign) -> np.ndarray:
    values = []
    for radius in design.radii:
        values.extend([radius, radius])
    if design.first_order_pole is not None:
        values.append(abs(design.first_order_pole))
    return np.array(values, dtype=float)


def root_stability_report(design: AllpassDesign) -> tuple[float, int]:
    coeff = denominator_coefficients(design)
    roots = np.roots(coeff)
    if roots.size == 0:
        return 0.0, 0
    radii = np.abs(roots)
    return float(np.max(radii)), int(np.sum(radii >= 1.0))


def verification_rows(
    target: TargetSet,
    design: AllpassDesign,
    verify_grid: int,
) -> tuple[list[dict[str, float]], float, float, float]:
    max_freq = target.sample_rate * NYQUIST_FRACTION
    phase_table = PhaseDelayTable(design, verify_grid, max_freq)

    phase_delay_f1 = phase_table.phase_delay_at(target.f1)
    loop_delay = target.sample_rate / target.f1 - phase_delay_f1
    min_total_group_delay = loop_delay + float(np.min(phase_table.tau))

    rows: list[dict[str, float]] = []
    for n_float, target_freq in zip(target.partials, target.freqs):
        n = int(n_float)
        if n == 1:
            predicted = target.f1
        else:
            predicted = solve_predicted_partial(
                n,
                loop_delay,
                phase_table,
                target.sample_rate,
            )

        if math.isnan(predicted):
            delta_hz = float("nan")
            err_cent = float("nan")
            b_pred = float("nan")
        else:
            delta_hz = predicted - target_freq
            err_cent = cents_error(predicted, target_freq)
            b_pred = ((predicted / (n * target.f0)) ** 2 - 1.0) / (n * n)

        rows.append(
            {
                "n": float(n),
                "target_freq": float(target_freq),
                "predicted_freq": float(predicted),
                "delta_hz": float(delta_hz),
                "error_cent": float(err_cent),
                "b_pred": float(b_pred),
            }
        )

    return rows, loop_delay, phase_delay_f1, min_total_group_delay


def format_cpp_array(name: str, values: Iterable[float], indent: str = "    ") -> str:
    vals = list(values)
    lines = [f"std::array<double, {len(vals)}> {name} = {{"]
    for i, value in enumerate(vals):
        comma = "," if i != len(vals) - 1 else ""
        lines.append(f"{indent}{value:.17g}{comma}")
    lines.append("};")
    return "\n".join(lines)


def print_cpp_a_array_only(design: AllpassDesign) -> None:
    den = denominator_coefficients(design)
    print(format_cpp_array("dispersionAllpassDenominatorA", den[1:]))
    
def print_cpp_sos_only(design: AllpassDesign, loop_delay: float) -> None:
    print(f"constexpr double dispersionLoopDelaySamples = {loop_delay:.17g};")
    print("struct AllpassBiquad { double b0, b1, b2, a1, a2; };")
    print(f"std::array<AllpassBiquad, {len(design.radii)}> dispersionAllpass = {{")

    for i, (radius, angle) in enumerate(zip(design.radii, design.angles)):
        a1 = -2.0 * radius * math.cos(angle)
        a2 = radius * radius

        b0 = a2
        b1 = a1
        b2 = 1.0

        comma = "," if i != len(design.radii) - 1 else ""

        print(
            "    "
            f"{{ {b0:.17g}, {b1:.17g}, {b2:.17g}, "
            f"{a1:.17g}, {a2:.17g} }}"
            f"{comma}"
        )

    print("};")


def print_cpp_parameters(design: AllpassDesign, loop_delay: float) -> None:
    print("\n// C++ parameters")
    print("// Biquad denominator convention: 1 + a1 z^-1 + a2 z^-2")
    print("// Difference equation uses -a1*y[n-1] - a2*y[n-2].")
    print("// Each section is:")
    print("// H(z) = (b0 + b1 z^-1 + b2 z^-2) / (1 + a1 z^-1 + a2 z^-2)")
    print(f"constexpr double dispersionLoopDelaySamples = {loop_delay:.17g};")

    if len(design.radii):
        print("\nstruct AllpassBiquad { double b0, b1, b2, a1, a2; };")
        print(f"std::array<AllpassBiquad, {len(design.radii)}> dispersionAllpass = {{")
        for i, (radius, angle) in enumerate(zip(design.radii, design.angles)):
            a1 = -2.0 * radius * math.cos(angle)
            a2 = radius * radius
            b0 = a2
            b1 = a1
            b2 = 1.0
            comma = "," if i != len(design.radii) - 1 else ""
            print(
                "    "
                f"{{ {b0:.17g}, {b1:.17g}, {b2:.17g}, {a1:.17g}, {a2:.17g} }}"
                f"{comma}"
            )
        print("};")

    if design.first_order_pole is not None:
        pole = design.first_order_pole
        print("\nstruct AllpassFirstOrder { double b0, b1, a1; };")
        print("std::array<AllpassFirstOrder, 1> dispersionFirstOrder = {")
        print(f"    {{ {-pole:.17g}, 1.0, {-pole:.17g} }}")
        print("};")

    den = denominator_coefficients(design)
    print()
    print(format_cpp_array("dispersionAllpassDenominatorA", den[1:]))
    print("// Direct-form numerator is reverse([1, A...]). Prefer the SOS above for runtime use.")


def print_report(
    target: TargetSet,
    design: AllpassDesign,
    rows: list[dict[str, float]],
    loop_delay: float,
    phase_delay_f1: float,
    min_total_group_delay: float,
) -> None:
    section_radii = pole_radii(design)
    max_section_radius = float(np.max(section_radii)) if len(section_radii) else 0.0
    max_root_radius, outside_count = root_stability_report(design)

    print("\n// Fit summary")
    print(f"sample_rate = {target.sample_rate:.10g}")
    print(f"f1 = {target.f1:.10g}")
    print(f"B = {target.b:.12g}")
    print(f"flexible_string_F0 = {target.f0:.12g}")
    print(f"order = {design.order}")
    print(f"biquad_sections = {len(design.radii)}")
    print(f"has_first_order_section = {design.first_order_pole is not None}")
    print(f"used_partials = {[int(x) for x in target.partials]}")
    print("phase_method = group_delay_integral_no_unwrap")
    print(f"fit_elapsed_sec = {design.elapsed_sec:.3f}")
    print(f"optimizer_best_nfev = {design.nfev}")
    print(f"fit_residual_max_cent_like = {design.max_fit_cent_residual:.6f}")
    print(f"fit_residual_rms_cent_like = {design.rms_fit_cent_residual:.6f}")

    print("\n// Stability / loop report")
    print(f"max_section_pole_radius = {max_section_radius:.12f}")
    print(f"max_denominator_root_radius = {max_root_radius:.12f}")
    print(f"poles_outside_or_on_unit_circle = {outside_count}")
    print(f"stable_by_sos_construction = {max_section_radius < 1.0}")
    print(f"phase_delay_at_f1_samples = {phase_delay_f1:.12f}")
    print(f"loop_delay_after_retuning_samples = {loop_delay:.12f}")
    print(f"min_total_group_delay_samples = {min_total_group_delay:.12f}")
    if loop_delay <= 0.0:
        print("WARNING: loop delay is non-positive; reduce max_radius/order or retune architecture.")
    if min_total_group_delay <= 0.0:
        print("WARNING: total group delay is non-positive somewhere; resonance order may be ambiguous.")

    print("\n// SOS pole parameters")
    print(format_cpp_array("dispersionPoleRadius", design.radii))
    print()
    print(format_cpp_array("dispersionPoleAngleRadians", design.angles))
    print()
    print(format_cpp_array("dispersionPoleAngleHz", design.angles * target.sample_rate / (2.0 * math.pi)))
    if design.first_order_pole is not None:
        print(f"\nfirst_order_real_pole = {design.first_order_pole:.17g}")

    print_cpp_parameters(design, loop_delay)

    print("\n// Verification: true loop resonance solve")
    print("n\ttarget_freq\tpredicted_freq\tdeltaHz\terrorCent\tB_pred")
    for row in rows:
        print(
            f"{int(row['n'])}\t"
            f"{row['target_freq']:.6f}\t"
            f"{row['predicted_freq']:.6f}\t"
            f"{row['delta_hz']:+.6f}\t"
            f"{row['error_cent']:+.6f}\t"
            f"{row['b_pred']:.12g}"
        )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Fit a stable allpass dispersion filter from f1 and stiff-string B.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--sample-rate", type=float, default=44100.0)
    parser.add_argument("--f1", type=float, required=True, help="Actual first partial frequency in Hz")
    parser.add_argument("--B", type=float, required=True, help="Inharmonicity coefficient")
    parser.add_argument("--order", type=int, default=16, help="Total allpass order")
    parser.add_argument("--max-partial", type=int, default=24)
    parser.add_argument("--starts", type=int, default=12, help="Number of optimizer initial guesses")
    parser.add_argument("--max-nfev", type=int, default=1200, help="Max evaluations per start")
    parser.add_argument("--max-radius", type=float, default=0.999, help="Strict pole-radius bound")
    parser.add_argument("--quadrature", type=int, default=64, help="Gauss points per target interval")
    parser.add_argument("--verify-grid", type=int, default=65536, help="Dense grid for final root solving")
    parser.add_argument(
        "--weight-power",
        type=float,
        default=0.5,
        help="Extra high-partial weight: weight=(n/max_n)^power",
    )
    parser.add_argument(
        "--radius-regularization",
        type=float,
        default=0.0,
        help="Optional mild penalty on large radii",
    )
    parser.add_argument("--seed", type=int, default=20260614)
    parser.add_argument("--quiet", action="store_true", help="Suppress per-start stderr progress")
    parser.add_argument(
        "--cpp-a-only",
        action="store_true",
        help="Only print the C++ denominator a-coefficient array and suppress the full report",
    )
    parser.add_argument(
        "--cpp-sos-only",
        action="store_true",
        help="Only print the SOS allpass sections and suppress the full report",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    target = build_targets(args.sample_rate, args.f1, args.B, args.max_partial)
    design = fit_allpass(
        target=target,
        order=args.order,
        starts=args.starts,
        max_nfev=args.max_nfev,
        max_radius=args.max_radius,
        quadrature_points=args.quadrature,
        weight_power=args.weight_power,
        radius_regularization=args.radius_regularization,
        seed=args.seed,
        verbose=(
            not args.quiet
            and not args.cpp_a_only
            and not args.cpp_sos_only
        ),
    )
    rows, loop_delay, phase_delay_f1, min_total_group_delay = verification_rows(
        target,
        design,
        args.verify_grid,
    )

    if args.cpp_a_only:
        print_cpp_a_array_only(design)
        return

    if args.cpp_sos_only:
        print_cpp_sos_only(design, loop_delay)
        return

    print_report(target, design, rows, loop_delay, phase_delay_f1, min_total_group_delay)


if __name__ == "__main__":
    main()
