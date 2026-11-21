#!/usr/bin/env bash
# Différentiel 4 langages : 10 cas déterministes + 200 cas de fuzz.
set -uo pipefail
cd "$(dirname "$0")/.."
BIN=/tmp/tron-battle-tests
export PATH="$HOME/.cargo/bin:$PATH"
rm -rf "$BIN" && mkdir -p "$BIN"

python3 tests/gen_cases.py
gcc -O2 -std=c11 -Wall -Wextra solutions/c/bot.c -o "$BIN/bot_c" || exit 1
g++ -O2 -std=c++20 -Wall -Wextra solutions/cpp/bot.cpp -o "$BIN/bot_cpp" || exit 1
cargo build --release --manifest-path solutions/rust/Cargo.toml --quiet || exit 1
cp solutions/rust/target/release/tron_battle_bot "$BIN/bot_rust"

failed=0
compare() {
    local input="$1" ref got
    ref=$(python3 solutions/python/bot.py < "$input")
    for cmd in "$BIN/bot_c" "$BIN/bot_cpp" "$BIN/bot_rust"; do
        got=$($cmd < "$input")
        if [ "$got" != "$ref" ]; then
            echo "FAIL $(basename "$input") [$cmd]"
            echo "  attendu: $(echo "$ref" | tr '\n' ' ')"
            echo "  obtenu:  $(echo "$got" | tr '\n' ' ')"
            failed=1
            return
        fi
    done
}

for input in tests/*.in; do compare "$input"; done
[ "$failed" -eq 0 ] && echo "tron-battle: 10/10 cas déterministes identiques dans 4 langages"

for seed in $(seq 200 399); do
    python3 -c "import sys; sys.path.insert(0,'tests'); from gen_cases import make_case; sys.stdout.write(make_case($seed))" > "$BIN/fuzz.in"
    compare "$BIN/fuzz.in" || break
done
[ "$failed" -eq 0 ] && echo "tron-battle: 200/200 cas de fuzz identiques dans 4 langages"
exit "$failed"
