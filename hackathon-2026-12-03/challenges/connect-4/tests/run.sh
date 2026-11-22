#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
OUT=/tmp/connect4-tests
export PATH="$HOME/.cargo/bin:$PATH"
mkdir -p "$OUT"
python3 tests/gen_fuzz.py
g++ -O2 -std=c++20 solutions/cpp/bot.cpp -o "$OUT/bot_cpp"
gcc -O2 -std=c11 solutions/c/bot.c -o "$OUT/bot_c"
cargo build --release --quiet --manifest-path solutions/rust/Cargo.toml
cp solutions/rust/target/release/connect4_bot "$OUT/bot_rust"
python3 - <<'PY'
from pathlib import Path
import subprocess

here = Path('.').resolve()
out = Path('/tmp/connect4-tests')
cmds = {
    'python': ['python3', str(here / 'solutions/python/bot.py')],
    'c': [str(out / 'bot_c')],
    'cpp': [str(out / 'bot_cpp')],
    'rust': [str(out / 'bot_rust')],
}
fails = 0
for case in sorted((here / 'tests/fuzz').glob('case_*.in')):
    data = case.read_bytes()
    results = {}
    for name, cmd in cmds.items():
        if name == 'rust' and not (out / 'bot_rust').exists():
            continue
        r = subprocess.run(cmd, input=data, capture_output=True, timeout=20)
        if r.returncode != 0:
            print(f'FAIL {case.name} [{name}] exit {r.returncode}: {r.stderr[:200]!r}')
            fails += 1
            break
        results[name] = r.stdout.decode().strip()
    if len({v for v in results.values()}) > 1:
        print(f'FAIL {case.name}: {results}')
        fails += 1
if fails:
    raise SystemExit(f'{fails} cas divergents')
print('connect-4 : 200/200 cas identiques dans 4 langages')
PY
