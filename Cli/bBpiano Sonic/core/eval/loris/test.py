import math

fs = 44100.0
f0 = float(input("f0 = "))

sections = []
section_count = int(input("section_count = "))

for i in range(section_count):
    a = float(input(f"a[{i}] = "))
    order = int(input(f"order[{i}] = "))
    sections.append((a, order))

print()
print("n\tideal(Hz)\tshifted(Hz)\tdeltaHz\tB_n")

delay_f1 = None

for n in range(1, 21):

    fn = n * f0

    D = 0.0

    for a, order in sections:
        D += order * (
            (1.0 - a * a)
            /
            (
                1.0
                + a * a
                + 2.0 * a * math.cos(
                    2.0 * math.pi * fn / fs
                )
            )
        )

    if delay_f1 is None:
        delay_f1 = D

    # Retune the fundamental back to f0.
    # Only relative phase-delay differences produce inharmonicity.
    shifted = n * f0 * (fs / f0 + delay_f1) / (fs / f0 + D)
    delta_hz = shifted - fn

    if n == 1:
        B_n = 0.0
    else:
        B_n = ((shifted / (n * f0)) ** 2 - 1.0) / (n * n)

    print(
        f"{n}\t"
        f"{fn:8.1f}\t"
        f"{shifted:11.4f}\t"
        f"{delta_hz:+.4f}\t"
        f"{B_n:.12f}"
    )
