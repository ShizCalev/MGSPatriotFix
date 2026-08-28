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


LOCAL_PATHS_FILE = Path(__file__).resolve().parent / "local_game_paths.txt"


def read_local_paths(path: Path) -> dict:
    result: dict = {}

    if not path.exists():
        log(f"WARNING: Local paths file not found: {path}")
        return result

    for line_no, raw_line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        line = raw_line.strip()

        if not line or line.startswith("#"):
            continue

        if "=" not in line:
            log(f"WARNING: Ignoring malformed line {line_no} in {path.name}: {raw_line!r}")
            continue

        key, _, value = line.partition("=")
        key = key.strip()
        value = value.strip()

        if len(value) >= 2 and value[0] == value[-1] and value[0] in ('"', "'"):
            value = value[1:-1].strip()

        result[key] = value

    return result


# key, display name, example path, expected subfolder, expected exe
PATH_KEYS = [
    ("MGS4_PATH", "METAL GEAR SOLID 4", r"F:\Steam\steamapps\common\METAL GEAR SOLID 4", "MGS4", "mgs4.exe"),
    ("PEACEWALKER_PATH", "MGS_PW", r"F:\Steam\steamapps\common\MGS_PW", "mgspw", "METAL GEAR SOLID PEACE WALKER.exe"),
]


def write_local_paths(path: Path, values: dict) -> None:
    lines = [
        "# Plug in your game paths below for builds to automatically update your game folders.",
        "",
    ]

    for key, _game_name, _example, _subfolder, _exe_name in PATH_KEYS:
        lines.append(f"{key}={values.get(key, '')}")

    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def prompt_and_create_local_paths(path: Path) -> dict:
    values: dict = {}

    if not sys.stdin.isatty():
        log(f"WARNING: {path.name} not found and not running interactively -- skipping local deploy.")
        return values

    log(f"{path.name} not found, setting it up now.\n\nThese paths can be updated later in {path.name}.")

    print()

    for key, game_name, example, subfolder, exe_name in PATH_KEYS:
        if path.exists():
            log(f"{path.name} was just created by another build -- using it instead.")
            return read_local_paths(path)

        while True:
            try:
                answer = input(
                    f'Please enter the full path to your {game_name} steam folder, e.g. "{example}"\n'
                    f"(leave blank to skip build syncing for {game_name}): "
                ).strip()
            except (EOFError, KeyboardInterrupt):
                answer = ""

            if not answer:
                break

            if (Path(answer) / subfolder / exe_name).exists():
                break

            if path.exists():
                log(f"{path.name} was just created by another build -- using it instead.")
                return read_local_paths(path)

            print()
            log(f'ERROR: "{answer}" does not contain {subfolder}\\{exe_name} -- that doesn\'t look like a {game_name} folder.')
            print()

        values[key] = answer
        print()

    write_local_paths(path, values)
    log(f"Saved: {path}")

    return values


def build_deploy_targets(mgs4_path: Optional[Path], peacewalker_path: Optional[Path]) -> List[tuple]:
    targets: List[tuple] = []

    if mgs4_path is not None:
        launcher_dir = mgs4_path / "Launcher"
        mgs4_dir = mgs4_path / "MGS4"

        targets.extend([
            (SRC_CFG, mgs4_path / "MGSPatriotFix Config Tool.exe"),
            (SRC_ASI, launcher_dir / "scripts" / "MGSPatriotFix.asi"),
            (SRC_ASI, mgs4_dir / "scripts" / "MGSPatriotFix.asi"),
        ])
    else:
        log("MGS4_PATH is blank/missing -- skipping METAL GEAR SOLID 4 deployment.")

    if peacewalker_path is not None:
        pw_launcher_dir = peacewalker_path / "launcher"
        pw_mgspw_dir = peacewalker_path / "mgspw"

        targets.extend([
            (SRC_CFG, peacewalker_path / "MGSPatriotFix Config Tool.exe"),
            (SRC_ASI, pw_launcher_dir / "scripts" / "MGSPatriotFix.asi"),
            (SRC_ASI, pw_mgspw_dir / "scripts" / "MGSPatriotFix.asi"),
        ])
    else:
        log("PEACEWALKER_PATH is blank/missing -- skipping MGS_PW deployment.")

    return targets


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

def deploy(targets: List[tuple]) -> bool:
    success = True

    for src, dst in targets:
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

    log(f"Reading local game paths: {LOCAL_PATHS_FILE}")

    if LOCAL_PATHS_FILE.exists():
        local_paths = read_local_paths(LOCAL_PATHS_FILE)
    else:
        local_paths = prompt_and_create_local_paths(LOCAL_PATHS_FILE)

    mgs4_path_str = local_paths.get("MGS4_PATH", "")
    peacewalker_path_str = local_paths.get("PEACEWALKER_PATH", "")

    mgs4_path = Path(mgs4_path_str) if mgs4_path_str else None
    peacewalker_path = Path(peacewalker_path_str) if peacewalker_path_str else None

    deploy_targets = build_deploy_targets(mgs4_path, peacewalker_path)

    if not deploy_targets:
        log("No deploy targets (both MGS4_PATH and PEACEWALKER_PATH are blank). Nothing to do.")
        return 0

    log("Deploying local game symlinks...")

    if not deploy(deploy_targets):
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