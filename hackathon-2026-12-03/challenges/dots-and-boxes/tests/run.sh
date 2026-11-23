#!/usr/bin/env bash
# Differential fuzz: build the 4 languages, run every tests/fuzz/case_*.in,
# compare stdout byte for byte against the Python reference.
# Usage: bash tests/run.sh [N_CASES]      env: CG_NODES (search budget, default 20000)
set -euo pipefail
cd "$(dirname "$0")/.."
NAME="$(basename "$(pwd)")"
OUT="${TMPDIR:-/tmp}/cgfuzz-$NAME"
export PATH="$HOME/.cargo/bin:$PATH"
export CG_NODES="${CG_NODES:-1500}"
mkdir -p "$OUT"
python3 tests/gen_fuzz.py "${1:-200}" >/dev/null
g++ -O2 -std=c++20 -Wall -Wextra solutions/cpp/bot.cpp -o "$OUT/bot_cpp"
gcc -O2 -std=c11 -Wall -Wextra solutions/c/bot.c -o "$OUT/bot_c" -lm
cargo build --release --quiet --manifest-path solutions/rust/Cargo.toml
BIN=$(sed -n 's/^name *= *"\(.*\)"/\1/p' solutions/rust/Cargo.toml | tail -1)
cp "solutions/rust/target/release/$BIN" "$OUT/bot_rust"
python3 - "$OUT" "$NAME" <<'PY'
import subprocess, sys
from pathlib import Path
out, name = Path(sys.argv[1]), sys.argv[2]
cmds = {
    "python": ["python3", "solutions/python/bot.py"],
    "c": [str(out / "bot_c")],
    "cpp": [str(out / "bot_cpp")],
    "rust": [str(out / "bot_rust")],
}
cases = sorted(Path("tests/fuzz").glob("case_*.in"))
fails = 0
for case in cases:
    data = case.read_bytes()
    res = {}
    for lang, cmd in cmds.items():
        try:
            r = subprocess.run(cmd, input=data, capture_output=True, timeout=60)
        except subprocess.TimeoutExpired:
            print(f"FAIL {case.name} [{lang}] timeout"); fails += 1; break
        if r.returncode != 0:
            print(f"FAIL {case.name} [{lang}] exit {r.returncode}: {r.stderr[:300]!r}")
            fails += 1; break
        if b"TIMEGUARD" in r.stderr:
            print(f"WARN {case.name} [{lang}] TIMEGUARD hit (raise TIME_MS or lower CG_NODES)")
        res[lang] = r.stdout
    else:
        if len(set(res.values())) > 1:
            fails += 1
            print(f"FAIL {case.name}:")
            for k, v in res.items():
                print(f"  {k:6} {v[:200]!r}")
if fails:
    sys.exit(f"{name}: {fails}/{len(cases)} divergent cases")
print(f"{name} : {len(cases)}/{len(cases)} cas identiques dans 4 langages")
PY
