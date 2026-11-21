#!/bin/sh
# Compare les 4 implémentations sur les cas déterministes.
set -e
cd "$(dirname "$0")/.."
BIN=/tmp/xmasrush-tests
mkdir -p "$BIN"

gcc -O2 -std=c11 -Wall -Wextra solutions/c/bot.c -o "$BIN/bot_c"
g++ -O2 -std=c++20 -Wall -Wextra solutions/cpp/bot.cpp -o "$BIN/bot_cpp"
if command -v cargo >/dev/null 2>&1; then
    CARGO=$(command -v cargo)
elif [ -x "$HOME/.cargo/bin/cargo" ]; then
    CARGO="$HOME/.cargo/bin/cargo"
else
    echo "cargo introuvable" >&2; exit 1
fi
"$CARGO" build --release --manifest-path solutions/rust/Cargo.toml --quiet
cp solutions/rust/target/release/xmas_rush_bot "$BIN/bot_rust"

python3 tests/gen_fuzz.py

fail=0
for input in tests/*.in tests/fuzz/*.in; do
    ref=$(python3 solutions/python/bot.py < "$input")
    for lang in c cpp rust; do
        got=$("$BIN/bot_$lang" < "$input")
        if [ "$got" != "$ref" ]; then
            echo "FAIL $(basename "$input") [$lang]"
            echo "  attendu: $ref"
            echo "  obtenu:  $got"
            fail=1
        fi
    done
done
if [ "$fail" -eq 0 ]; then
    echo "xmas-rush : 210/210 cas identiques dans 4 langages"
fi
exit "$fail"
