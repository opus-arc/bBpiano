"""Build the landing page and MkDocs research section as one Pages artifact."""

from html import escape
from pathlib import Path
import shutil
import subprocess


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "doc/site"
OUTPUT = ROOT / "site"


def redirect(path: str, destination: str) -> None:
    target = OUTPUT / path
    target.parent.mkdir(parents=True, exist_ok=True)
    url = escape(destination, quote=True)
    target.write_text(
        "<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
        f'<meta http-equiv="refresh" content="0; url={url}">'
        f'<link rel="canonical" href="{url}"><title>Moved</title></head>'
        f'<body><a href="{url}">Continue</a></body></html>\n',
        encoding="utf-8",
    )


def main() -> None:
    if OUTPUT.exists():
        shutil.rmtree(OUTPUT)
    subprocess.run(["mkdocs", "build", "--clean", "--strict"], cwd=ROOT, check=True)

    shutil.copy2(SOURCE / "index.html", OUTPUT / "index.html")
    for folder in ("css", "js", "assets", "audio"):
        shutil.copytree(
            SOURCE / folder,
            OUTPUT / folder,
            ignore=shutil.ignore_patterns(".DS_Store"),
        )
    (OUTPUT / ".nojekyll").touch()

    redirect("Audio/Audio.html", "../research/audio.html")
    redirect("bBpiano.html", "research/project.html")
    redirect("doc/research/From PDE to PCM.html", "../../research/from-pde-to-pcm.html")

    for required in (
        "index.html",
        "research/index.html",
        "research/from-pde-to-pcm.html",
        "research/project.html",
        "research/audio.html",
    ):
        assert (OUTPUT / required).is_file(), required


if __name__ == "__main__":
    main()
