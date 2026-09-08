#
#  clap_embedding.py
#  bBpiano
#
#  Created by opus arc on 2026/6/9.
#

import sys
import numpy as np
import laion_clap


def main() -> None:
    if len(sys.argv) != 3:
        print("Usage: python clap_embedding.py <reference.wav> <test.wav>")
        sys.exit(1)

    reference_wav = sys.argv[1]
    test_wav = sys.argv[2]

    model = laion_clap.CLAP_Module(enable_fusion=False)
    model.load_ckpt()

    embeddings = model.get_audio_embedding_from_filelist(
        x=[reference_wav, test_wav]
    )

    reference_embedding = embeddings[0]
    test_embedding = embeddings[1]

    similarity = np.dot(
        reference_embedding,
        test_embedding
    ) / (
        np.linalg.norm(reference_embedding)
        * np.linalg.norm(test_embedding)
    )

    print(float(similarity))


if __name__ == "__main__":
    main()
