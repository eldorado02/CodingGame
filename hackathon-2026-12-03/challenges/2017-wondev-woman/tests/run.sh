#!/bin/sh
# Compare les 4 implémentations sur les cas déterministes.
set -e
cd "$(dirname "$0")/.."
BIN=/tmp/wondev-tests
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
cp solutions/rust/target/release/wondev_woman_bot "$BIN/bot_rust"

fail=0
for input in tests/*.in; do
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
    echo "wondev-woman : tous les cas passent dans tous les langages"
fi
exit "$fail"
