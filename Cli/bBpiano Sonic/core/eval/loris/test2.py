import math
import numpy as np
from scipy.optimize import least_squares


STABILITY_LIMIT = 0.98


def flexible_f0_from_first_partial(f1: float, b: float) -> float:
    """
    Stiff-string model:
        f_n = n * F0 * sqrt(1 + B * n^2)

    Here the user enters the actual first partial f1, therefore:
        F0 = f1 / sqrt(1 + B)
    """
    return f1 / math.sqrt(1.0 + b)


def target_freq(f1: float, b: float, n: int) -> float:
    flexible_f0 = flexible_f0_from_first_partial(f1, b)
    return n * flexible_f0 * math.sqrt(1.0 + b * n * n)


def reflection_to_denominator_coeffs(reflection: np.ndarray) -> np.ndarray:
    """
    Convert stable reflection coefficients to denominator coefficients.

    If every reflection coefficient satisfies |k_i| < 1, the resulting all-pole
    denominator is Schur stable. This gives a stable allpass by construction.

    Returns coeffs [a1, a2, ..., aN] for:
        A(z) = 1 + a1 z^-1 + ... + aN z^-N
    """
    a = np.array([], dtype=float)

    for k in reflection:
        if len(a) == 0:
            a = np.array([k], dtype=float)
        else:
            a = np.concatenate((a + k * a[::-1], np.array([k], dtype=float)))

    return a


def params_to_reflection(params: np.ndarray) -> np.ndarray:
    return STABILITY_LIMIT * np.tanh(params)


def params_to_coeffs(params: np.ndarray) -> np.ndarray:
    return reflection_to_denominator_coeffs(params_to_reflection(params))


def allpass_phase_response(coeffs: np.ndarray, freq_hz: float, sample_rate: float) -> float:
    """
    Phase response of the Nth-order allpass:

        H(z) = (a_N + a_{N-1} z^-1 + ... + a_1 z^{-(N-1)} + z^-N)
               /
               (1 + a_1 z^-1 + ... + a_N z^-N)

    coeffs = [a1, a2, ..., aN]
    """
    omega = 2.0 * math.pi * freq_hz / sample_rate
    z_inv = np.exp(-1j * omega)

    den = 1.0 + 0.0j
    for i, a in enumerate(coeffs, start=1):
        den += a * (z_inv ** i)

    num = 0.0 + 0.0j
    order = len(coeffs)
    for i, a in enumerate(reversed(coeffs), start=0):
        num += a * (z_inv ** i)
    num += z_inv ** order

    return float(np.angle(num / den))


# Vectorized phase response for the same Nth-order allpass.
def allpass_phase_response_many(
    coeffs: np.ndarray,
    freqs_hz: np.ndarray,
    sample_rate: float,
) -> np.ndarray:
    """Vectorized phase response for the same Nth-order allpass."""
    omegas = 2.0 * np.pi * freqs_hz / sample_rate
    z_inv = np.exp(-1j * omegas)

    den = np.ones_like(z_inv, dtype=np.complex128)
    for i, a in enumerate(coeffs, start=1):
        den += a * (z_inv ** i)

    num = np.zeros_like(z_inv, dtype=np.complex128)
    order = len(coeffs)
    for i, a in enumerate(reversed(coeffs), start=0):
        num += a * (z_inv ** i)
    num += z_inv ** order

    return np.angle(num / den)


def phase_delay_at_points(
    coeffs: np.ndarray,
    freqs: np.ndarray,
    sample_rate: float,
) -> np.ndarray:
    """
    Compute phase delay D(f) = -phi(f) / omega at selected frequencies.

    Important: do not unwrap only at the sparse partial frequencies. A high-order
    allpass can rotate by more than pi between two neighboring partials, which
    causes wrong phase branches. We unwrap on a dense grid and interpolate.
    """
    max_freq = min(sample_rate * 0.49, float(np.max(freqs)) * 1.05)
    dense_freqs = np.linspace(1.0, max_freq, 4096)
    dense_phases = np.unwrap(
        allpass_phase_response_many(coeffs, dense_freqs, sample_rate)
    )

    phases = np.interp(freqs, dense_freqs, dense_phases)
    omegas = 2.0 * math.pi * freqs / sample_rate
    return -phases / omegas


def denominator_poly(coeffs: np.ndarray) -> np.ndarray:
    """Return denominator polynomial in descending powers of z: z^N + a1 z^(N-1) + ... + aN."""
    return np.concatenate(([1.0], coeffs))


def pole_radius_report(coeffs: np.ndarray) -> str:
    roots = np.roots(denominator_poly(coeffs))
    if len(roots) == 0:
        return "no poles"

    outside_count = int(np.sum(np.abs(roots) >= 1.0))
    return (
        f"max={float(np.max(np.abs(roots))):.12f}, "
        f"outside_or_on_unit_circle={outside_count}"
    )


def make_unwrapped_phase_table(
    coeffs: np.ndarray,
    sample_rate: float,
    grid_size: int = 16384,
) -> tuple[np.ndarray, np.ndarray]:
    max_freq = sample_rate * 0.49
    freqs = np.linspace(1.0, max_freq, grid_size)
    phases = np.unwrap(
        allpass_phase_response_many(coeffs, freqs, sample_rate)
    )
    return freqs, phases


def interp_unwrapped_phase(
    freq_hz: float,
    phase_freqs: np.ndarray,
    phase_values: np.ndarray,
) -> float:
    if freq_hz <= 0.0:
        return 0.0
    freq = min(freq_hz, float(phase_freqs[-1]))
    return float(np.interp(freq, phase_freqs, phase_values))


def predicted_partial_freq(
    f1: float,
    coeffs: np.ndarray,
    n: int,
    sample_rate: float,
    phase_freqs: np.ndarray,
    phase_values: np.ndarray,
) -> float:
    """
    Solve for the predicted nth partial after retuning f1.

    Resonance condition, after removing the constant delay that tunes partial 1:
        N_loop = fs/f1 - D_ap(f1)
        f = fs * n / (N_loop + D_ap(f))
    where D_ap(f) = -phi_ap(f) / omega.
    """
    omega1 = 2.0 * math.pi * f1 / sample_rate
    phi1 = interp_unwrapped_phase(f1, phase_freqs, phase_values)
    d1 = -phi1 / omega1
    loop_delay = sample_rate / f1 - d1

    lo = max(1.0, n * f1 * 0.5)
    hi = min(sample_rate * 0.49, n * f1 * 4.0)

    def equation(freq: float) -> float:
        omega = 2.0 * math.pi * freq / sample_rate
        phi = interp_unwrapped_phase(freq, phase_freqs, phase_values)
        d = -phi / omega
        return freq * (loop_delay + d) / sample_rate - n

    grid = np.linspace(lo, hi, 1024)
    vals = [equation(float(x)) for x in grid]

    bracket = None
    for i in range(len(grid) - 1):
        if vals[i] == 0.0:
            return float(grid[i])
        if vals[i] * vals[i + 1] < 0.0:
            bracket = (float(grid[i]), float(grid[i + 1]))
            break

    if bracket is None:
        best_index = int(np.argmin(np.abs(vals)))
        return float(grid[best_index])

    a, b = bracket
    for _ in range(60):
        mid = 0.5 * (a + b)
        fa = equation(a)
        fm = equation(mid)
        if fa * fm <= 0.0:
            b = mid
        else:
            a = mid

    return 0.5 * (a + b)


def cents_error(predicted: float, target: float) -> float:
    return 1200.0 * math.log2(predicted / target)


def design_stable_allpass_fit(
    f1: float,
    b: float,
    order: int,
    sample_rate: float,
    max_partial: int,
) -> tuple[np.ndarray, np.ndarray, list[int]]:
    """
    Stable constrained allpass fitting.

    This replaces raw LSEE with a stable Schur/lattice parameterization.
    The target is still derived from B as a relative phase-delay target:

        D_target_rel(n) = fs * n / f_target(n) - fs / f1

    The optimizer adjusts stable reflection coefficients so that:

        D_allpass(f_target(n)) - D_allpass(f1) ~= D_target_rel(n)

    This keeps the design physical-model driven while avoiding unstable LSEE
    denominator polynomials.
    """
    used_partials: list[int] = []
    target_freqs = [f1]
    target_rel_delays = []
    weights = []

    for n in range(2, max_partial + 1):
        ft = target_freq(f1, b, n)
        if ft >= sample_rate * 0.49:
            continue

        used_partials.append(n)
        target_freqs.append(ft)
        target_rel_delays.append(sample_rate * n / ft - sample_rate / f1)
        weights.append(1.0 / math.sqrt(n))

    freqs = np.array(target_freqs, dtype=float)
    target_rel_delays = np.array(target_rel_delays, dtype=float)
    weights = np.array(weights, dtype=float)

    if len(used_partials) == 0:
        raise ValueError("No usable target partials below Nyquist.")

    def residual(params: np.ndarray) -> np.ndarray:
        coeffs = params_to_coeffs(params)
        delays = phase_delay_at_points(coeffs, freqs, sample_rate)
        rel_delays = delays[1:] - delays[0]
        delay_error = weights * (rel_delays - target_rel_delays)

        # Mild regularization keeps the optimizer away from violently resonant
        # near-unit-circle solutions unless they are truly needed.
        reflection = params_to_reflection(params)
        regularization = 0.02 * reflection

        return np.concatenate((delay_error, regularization))

    initial_guesses = []
    initial_guesses.append(np.zeros(order, dtype=float))
    initial_guesses.append(np.linspace(-0.20, 0.20, order))
    initial_guesses.append(np.linspace(0.20, -0.20, order))

    rng = np.random.default_rng(20260614)
    for _ in range(16):
        initial_guesses.append(rng.normal(0.0, 0.35, size=order))

    best = None
    best_cost = float("inf")

    for x0 in initial_guesses:
        result = least_squares(
            residual,
            x0,
            max_nfev=50000,
            ftol=1e-12,
            xtol=1e-12,
            gtol=1e-12,
            verbose=0,
        )

        if result.cost < best_cost:
            best_cost = result.cost
            best = result

    if best is None:
        raise RuntimeError("least_squares failed to produce a result.")

    reflection = params_to_reflection(best.x)
    coeffs = reflection_to_denominator_coeffs(reflection)

    print(f"best weighted delay cost = {best_cost:.12e}")
    return coeffs, reflection, used_partials


def print_result(
    f1: float,
    b: float,
    order: int,
    sample_rate: float,
    max_partial: int,
    coeffs: np.ndarray,
    reflection: np.ndarray,
    used_partials: list[int],
) -> None:
    print("\n// Stable Nth-order allpass via reflection coefficients")
    print("// H(z) = (aN + aN-1 z^-1 + ... + a1 z^{-(N-1)} + z^-N)")
    print("//        /")
    print("//        (1 + a1 z^-1 + ... + aN z^-N)")
    print(f"// first partial f1 = {f1}")
    print(f"// flexible-string F0 = {flexible_f0_from_first_partial(f1, b)}")
    print(f"// B  = {b}")
    print(f"// order = {order}")
    print(f"// used partials = {used_partials}")
    print(f"// pole radius = {pole_radius_report(coeffs)}")

    print("std::array<double, %d> reflectionK = {" % order)
    for i, k in enumerate(reflection):
        comma = "," if i != order - 1 else ""
        print(f"    {k:.15f}{comma}")
    print("};")

    print("\nstd::array<double, %d> allpassA = {" % order)
    for i, a in enumerate(coeffs):
        comma = "," if i != order - 1 else ""
        print(f"    {a:.15f}{comma}")
    print("};")

    print("\nPrecomputing unwrapped phase table...")
    phase_freqs, phase_values = make_unwrapped_phase_table(coeffs, sample_rate)

    print("\nn\ttarget(Hz)\tpredicted(Hz)\tdeltaHz\terrorCent\tB_pred")
    for n in range(1, max_partial + 1):
        tf = target_freq(f1, b, n)
        if tf >= sample_rate * 0.49:
            continue

        if n == 1:
            pf = f1
        else:
            pf = predicted_partial_freq(
                f1,
                coeffs,
                n,
                sample_rate,
                phase_freqs,
                phase_values,
            )

        delta_hz = pf - tf
        err_cent = cents_error(pf, tf)

        if n == 1:
            b_pred = 0.0
        else:
            flexible_f0 = flexible_f0_from_first_partial(f1, b)
            b_pred = ((pf / (n * flexible_f0)) ** 2 - 1.0) / (n * n)

        print(
            f"{n}\t"
            f"{tf:11.4f}\t"
            f"{pf:13.4f}\t"
            f"{delta_hz:+.4f}\t"
            f"{err_cent:+.4f}\t"
            f"{b_pred:.12f}"
        )


def main() -> None:
    sample_rate = float(input("sample_rate = ") or "44100")
    f1 = float(input("first partial f1 = "))
    b = float(input("B = "))
    order = int(input("order = "))
    max_partial = int(input("max_partial = ") or "24")

    coeffs, reflection, used_partials = design_stable_allpass_fit(
        f1=f1,
        b=b,
        order=order,
        sample_rate=sample_rate,
        max_partial=max_partial,
    )

    print_result(
        f1=f1,
        b=b,
        order=order,
        sample_rate=sample_rate,
        max_partial=max_partial,
        coeffs=coeffs,
        reflection=reflection,
        used_partials=used_partials,
    )


if __name__ == "__main__":
    main()
