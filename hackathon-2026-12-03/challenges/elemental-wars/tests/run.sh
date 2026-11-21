#!/usr/bin/env bash
# Différentiel 4 langages : 200 cas de fuzz.
set -uo pipefail
cd "$(dirname "$0")/.."
BIN=/tmp/elemental-wars-tests
export PATH="$HOME/.cargo/bin:$PATH"
rm -rf "$BIN" && mkdir -p "$BIN"

python3 tests/gen_fuzz.py || exit 1
gcc -O2 -std=c11 -Wall -Wextra solutions/c/bot.c -o "$BIN/bot_c" || exit 1
g++ -O2 -std=c++20 -Wall -Wextra solutions/cpp/bot.cpp -o "$BIN/bot_cpp" || exit 1
cargo build --release --manifest-path solutions/rust/Cargo.toml --quiet || exit 1
cp solutions/rust/target/release/elemental_wars_bot "$BIN/bot_rust"

failed=0
count=0
for input in tests/fuzz/*.in; do
    count=$((count + 1))
    ref=$(python3 solutions/python/bot.py < "$input")
    for cmd in "$BIN/bot_c" "$BIN/bot_cpp" "$BIN/bot_rust"; do
        got=$($cmd < "$input")
        if [ "$got" != "$ref" ]; then
            echo "FAIL $(basename "$input") [$cmd]"
            echo "  attendu: $ref"
            echo "  obtenu:  $got"
            failed=1
        fi
    done
done
if [ "$failed" -eq 0 ]; then
    echo "elemental-wars : $count/$count cas identiques dans 4 langages"
fi
exit "$failed"
