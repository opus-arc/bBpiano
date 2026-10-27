# Core FDN paper sources and access record

Checked: 2026-10-08.

## Saved full text

### Schlecht and Habets (2019)

Sebastian J. Schlecht and Emanuël A. P. Habets, “Modal Decomposition of
Feedback Delay Networks,” *IEEE Transactions on Signal Processing*, vol. 67,
no. 20, pp. 5340-5351, 2019.

- DOI: <https://doi.org/10.1109/TSP.2019.2937286>
- Saved file:
  `2019_Schlecht_Habets_Modal_Decomposition_of_Feedback_Delay_Networks_AAM.pdf`
- Source: Aalto University institutional repository,
  <https://acris.aalto.fi/ws/portalfiles/portal/39543702/Modal_Decomposition_of_Feedback_Delay_Networks.pdf>
- Version: peer-reviewed accepted author manuscript/post-print, explicitly
  identified as an electronic reprint; it may differ from the IEEE version in
  pagination and typography.
- SHA-256:
  `359484bb40a902f431a4aeb8fe4cfa22001139ef6837b43d7351c2d95a1dc961`
- Access note: the repository permits duplication for research or educational
  use and disallows sale or other unauthorized distribution.  Preserve the
  repository cover page and copyright notice.

Algorithm details checked against the manuscript:

- generalized characteristic polynomial
  `p(z)=det(P(z))`, `P(z)=diag(z**m_i)-A`;
- matrix-polynomial Ehrlich-Aberth iteration using
  `p'(z)/p(z)=tr(P(z)^-1 P'(z))`;
- simultaneous deflation `sum_{l != i} 1/(lambda_i-lambda_l)`;
- roots-of-unity initialization for lossless FDNs;
- singularity/step stopping criteria and reversed-polynomial evaluation outside
  the unit circle;
- optional EAI-AD near/far deflation approximation, whose error bound is
  empirically calibrated for a configuration.

## Metadata verified; full text not saved

### Das, Canfield-Dafilou, and Abel (2019)

Orchisama Das, Elliot K. Canfield-Dafilou, and Jonathan S. Abel, “On the
Behavior of Delay Network Reverberator Modes,” in *2019 IEEE Workshop on
Applications of Signal Processing to Audio and Acoustics (WASPAA)*, New Paltz,
NY, USA, 20-23 October 2019, pp. 50-54.

- DOI: <https://doi.org/10.1109/WASPAA.2019.8937260>
- Publisher record: IEEE/WASPAA.
- Author/publication confirmation: Orchisama Das's University of Surrey
  publication list and Jonathan Abel's Stanford profile both list the paper.
- Access status: OpenAlex reports the publisher version as closed access and no
  repository full text.  ResearchGate reports a full text uploaded by Orchisama
  Das on 2019-11-19, but automated retrieval was blocked by a security check.
  The file was therefore **not saved**; no poster, search extract, or secondary
  source has been substituted for the paper.

Details cross-checked from the publisher metadata and author-uploaded text made
visible in the indexed record:

- dense state-transition eigenvalue formulation of FDN modes;
- scalar and first-order shelf loss cases;
- continuous orthogonal homotopy
  `R(theta) = [[cos(theta), sin(theta)], [-sin(theta), cos(theta)]]` and
  `M(theta)=R(theta) tensor ... tensor R(theta)`;
- identity at `theta=0`, Hadamard-type maximum mixing at `theta=pi/4`;
- observed nearby-mode approach in damping followed by frequency deflection.

If a legally accessible publisher PDF or author manuscript becomes available,
save that exact version here, record its version/source/hash, and update this
entry.  Do not relabel the existing WASPAA poster as the paper.
