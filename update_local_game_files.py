from __future__ import annotations

import ctypes
import os
import sys
from datetime import datetime
from pathlib import Path
from typing import List, Optional


# ==========================================================
# CONSOLE LOGGING
# ==========================================================

def log(msg: str) -> None:
    ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(f"{ts} {msg}")


def has_flag(flag: str) -> bool:
    flag_l = flag.lower()
    return any(a.lower() == flag_l for a in sys.argv[1:])


def pause() -> None:
    try:
        input("Press ENTER to close...")
    except Exception:
        pass


# ==========================================================
# ADMIN BOOTSTRAP
# ==========================================================

def ensure_admin() -> None:
    if has_flag("--elevated"):
        return

    try:
        is_admin = bool(ctypes.windll.shell32.IsUserAnAdmin())
    except Exception:
        is_admin = False

    if is_admin:
        return

    new_args: List[str] = []

    for arg in sys.argv[1:]:
        if arg.lower() == "--elevated":
            continue
        new_args.append(arg)

    new_args.append("--elevated")

    args = " ".join(f'"{arg}"' for arg in new_args)
    cmdline = f'"{Path(__file__).resolve()}" {args}'.strip()

    log("Not admin. Relaunching elevated...")

    rc = ctypes.windll.shell32.ShellExecuteW(
        None,
        "runas",
        sys.executable,
        cmdline,
        None,
        1,
    )

    if rc <= 32:
        log("ERROR: Elevation cancelled or failed.")
        raise SystemExit(1)

    raise SystemExit(0)


# ==========================================================
# CONFIG
# ==========================================================

SRC_ASI = Path(
    r"C:\Development\Git\MGSPatriotFix\x64\Release\MGSPatriotFix.asi"
)

SRC_CFG = Path(
    r"C:\Development\Git\MGSPatriotFix\x64\Release\MGSPatriotFix Config Tool.exe"
)


GAME_DIR = Path(
    r"F:\Steam\steamapps\common\METAL GEAR SOLID 4"
)

LAUNCHER_DIR = GAME_DIR / "Launcher"
MGS4_DIR = GAME_DIR / "MGS4"


PW_GAME_DIR = Path(
    r"F:\Steam\steamapps\common\MGS_PW"
)

PW_LAUNCHER_DIR = PW_GAME_DIR / "launcher"
PW_MGSPW_DIR = PW_GAME_DIR / "mgspw"


DEPLOY_TARGETS = [
    (
        SRC_CFG,
        GAME_DIR / "MGSPatriotFix Config Tool.exe",
    ),
    (
        SRC_ASI,
        LAUNCHER_DIR  / "scripts" / "MGSPatriotFix.asi",
    ),
    (
        SRC_ASI,
        MGS4_DIR  / "scripts" / "MGSPatriotFix.asi",
    ),
    (
        SRC_CFG,
        PW_GAME_DIR / "MGSPatriotFix Config Tool.exe",
    ),
    (
        SRC_ASI,
        PW_LAUNCHER_DIR / "scripts" / "MGSPatriotFix.asi",
    ),
    (
        SRC_ASI,
        PW_MGSPW_DIR / "scripts" / "MGSPatriotFix.asi",
    ),
]


# ==========================================================
# SYMLINK HELPERS
# ==========================================================

def resolve_symlink_target(path: Path) -> Optional[Path]:
    try:
        if not path.is_symlink():
            return None

        return Path(os.readlink(path)).resolve()
    except OSError:
        return None


def ensure_file_symlink(src_file: Path, dst_file: Path) -> str:
    if not src_file.exists():
        return f"[ERROR] Source missing: {src_file}"

    if not dst_file.parent.exists():
        return f"[ERROR] Destination directory missing: {dst_file.parent}"

    existing_target = resolve_symlink_target(dst_file)

    if existing_target is not None:
        try:
            if existing_target == src_file.resolve():
                return f"[OK] Link correct: {dst_file}"
        except OSError:
            pass

    try:
        if dst_file.exists() or dst_file.is_symlink():
            dst_file.unlink()

        dst_file.symlink_to(src_file)

        return f"[LINK] {dst_file} -> {src_file}"

    except OSError as exc:
        return (
            f"[ERROR] Failed to create symlink: "
            f"{dst_file} -> {src_file} ({exc})"
        )


# ==========================================================
# DEPLOY
# ==========================================================

def deploy() -> bool:
    success = True

    for src, dst in DEPLOY_TARGETS:
        result = ensure_file_symlink(src, dst)
        log(result)

        if result.startswith("[ERROR]"):
            success = False

    return success


# ==========================================================
# MAIN
# ==========================================================

def main() -> int:
    log("=== update_local_game_files.py start ===")

    if os.environ.get("CI"):
        log("CI environment detected. Skipping update.")
        return 0

    if os.environ.get("SHIZ_LOCAL_VORTEX_FILE_SYNC") != "1":
        log("Skipping local game file sync.")
        return 0

    log(f"Python: {sys.version}")
    log(f"Script: {Path(__file__).resolve()}")
    log(f"User:   {os.environ.get('USERNAME', '')}")
    log(f"CWD:    {Path.cwd().resolve()}")
    log(f"Args:   {' '.join(sys.argv[1:])}")

    log("Ensuring admin...")
    ensure_admin()
    log("Admin OK.")

    log(
        "Checking build outputs:\n"
        f"  ASI: {SRC_ASI}\n"
        f"  CFG: {SRC_CFG}"
    )

    if not SRC_ASI.exists():
        log(f"ERROR: Source ASI missing: {SRC_ASI}")
        return 1

    if not SRC_CFG.exists():
        log(f"ERROR: Source Config Tool missing: {SRC_CFG}")
        return 1

    log("Deploying local game symlinks...")

    if not deploy():
        return 1

    log("Done.")
    return 0


if __name__ == "__main__":
    rc = 1

    try:
        rc = main()

    except SystemExit as exc:
        code = exc.code if isinstance(exc.code, int) else 0
        log(f"SystemExit: {code}")
        rc = code
        raise

    except Exception as exc:
        log(f"FATAL: {type(exc).__name__}: {exc}")
        raise

    finally:
        log(f"=== exit code {rc} ===")

        if has_flag("--pause"):
            pause()

    raise SystemExit(rc)