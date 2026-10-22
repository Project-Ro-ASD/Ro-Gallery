"""Exercise the built application without reading the developer's photo library."""
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import zlib

binary = str(Path(sys.argv[1]).resolve())
with tempfile.TemporaryDirectory(prefix="ro-gallery-smoke-") as folder:
    root = Path(folder)
    env = os.environ.copy()
    env.update(HOME=folder, QT_QPA_PLATFORM="offscreen",
               XDG_CONFIG_HOME=str(root / "config"),
               XDG_CACHE_HOME=str(root / "cache"),
               XDG_DATA_HOME=str(root / "data"),
               XDG_RUNTIME_DIR=str(root / "runtime"))
    (root / "runtime").mkdir(mode=0o700)
    version = subprocess.check_output([binary, "--version"], env=env, text=True, timeout=10)
    if not version.startswith("ro-gallery "):
        raise SystemExit(f"Unexpected application identity: {version!r}")
    photos = root / "photos"
    photos.mkdir()
    photo = photos / "test.png"
    def chunk(kind, payload):
        return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", zlib.crc32(kind + payload))
    photo.write_bytes(b"\x89PNG\r\n\x1a\n"
                      + chunk(b"IHDR", struct.pack(">IIBBBBB", 32, 24, 8, 2, 0, 0, 0))
                      + chunk(b"IDAT", zlib.compress((b"\0" + bytes([40, 120, 200]) * 32) * 24))
                      + chunk(b"IEND", b""))
    settings = root / "config" / "Ro-ASD" / "ro-gallery.ini"
    settings.parent.mkdir(parents=True)
    settings.write_text(f"[library]\nfolders={photos}\n[photos]\nfavorites={photo}\ndeleted={photos / 'missing.png'}\n")
    shot = root / "smoke.png"
    subprocess.run([binary, "--size", "960x640", "--screenshot", str(shot)],
                   env=env, check=True, timeout=15)
    content = shot.read_bytes()
    if content[:8] != b"\x89PNG\r\n\x1a\n" or struct.unpack(">II", content[16:24]) != (960, 640):
        raise SystemExit("Application did not render the expected window")
    if "geometry=" not in settings.read_text():
        raise SystemExit("Ro-Gallery did not persist its window settings")
    viewer = root / "viewer.png"
    subprocess.run([binary, "--size", "960x640", "--viewer-shot", str(viewer)],
                   env=env, check=True, timeout=15)
    if viewer.read_bytes() == content:
        raise SystemExit("Viewer did not open the test photo")
    print("Ro-Gallery identity, library, favorites, viewer and isolated settings: OK")
