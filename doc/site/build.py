"""Build the public landing page and listening assets for GitHub Pages."""

from html import escape
from pathlib import Path
import shutil


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
    OUTPUT.mkdir(parents=True)

    shutil.copy2(SOURCE / "index.html", OUTPUT / "index.html")
    for folder in ("css", "js", "assets", "audio"):
        shutil.copytree(
            SOURCE / folder,
            OUTPUT / folder,
            ignore=shutil.ignore_patterns(".DS_Store"),
        )
    (OUTPUT / ".nojekyll").touch()

    shutil.copy2(SOURCE / "bBpiano.html", OUTPUT / "bBpiano.html")
    redirect("Audio/Audio.html", "../index.html#sound")

    for required in (
        "index.html",
        "css/style.css",
        "js/script.js",
        "bBpiano.html",
        "audio/Audio.html",
        "Audio/Audio.html",
    ):
        assert (OUTPUT / required).is_file(), required


if __name__ == "__main__":
    main()
