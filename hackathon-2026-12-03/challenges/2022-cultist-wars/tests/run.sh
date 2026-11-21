#!/usr/bin/env bash
set -euo pipefail
D="$(cd "$(dirname "$0")/.." && pwd)"
B=/tmp/cultist-wars-tests
export PATH="$HOME/.cargo/bin:$PATH"
rm -rf "$B" && mkdir -p "$B"

python3 "$D/tests/gen_cases.py"

gcc -O2 -std=c11 -o "$B/bot_c" "$D/solutions/c/bot.c"
g++ -O2 -std=c++17 -o "$B/bot_cpp" "$D/solutions/cpp/bot.cpp"
rustc -O -o "$B/bot_rust" "$D/solutions/rust/bot.rs"

ok=0
for f in "$D"/tests/*.in "$D"/tests/fuzz/*.in; do
  python3 "$D/solutions/python/bot.py" < "$f" > "$B/python.out"
  "$B/bot_c" < "$f" > "$B/c.out"
  "$B/bot_cpp" < "$f" > "$B/cpp.out"
  "$B/bot_rust" < "$f" > "$B/rust.out"
  diff -q "$B/python.out" "$B/c.out" > /dev/null
  diff -q "$B/python.out" "$B/cpp.out" > /dev/null
  diff -q "$B/python.out" "$B/rust.out" > /dev/null
  ok=$((ok + 1))
done
echo "cultist-wars: $ok/210 cas identiques dans 4 langages"
