#!/bin/sh
# Compare les 4 implémentations sur tous les cas déterministes.
set -e
cd "$(dirname "$0")/.."
BIN="${TMPDIR:-/tmp}/smash-tests"
mkdir -p "$BIN"

gcc -O2 -std=c11 -Wall -Wextra solutions/c/bot.c -o "$BIN/bot_c"
g++ -O2 -std=c++20 -Wall -Wextra solutions/cpp/bot.cpp -o "$BIN/bot_cpp"
if command -v cargo >/dev/null 2>&1; then CARGO=cargo
elif [ -x "$HOME/.cargo/bin/cargo" ]; then CARGO="$HOME/.cargo/bin/cargo"
else CARGO=""; fi
if [ -n "$CARGO" ]; then
    "$CARGO" build --release --manifest-path solutions/rust/Cargo.toml --quiet 2>/dev/null || true
    cp solutions/rust/target/release/smash_bot "$BIN/bot_rust" 2>/dev/null || true
fi

fail=0
for input in tests/*.in; do
    expected=$(python3 solutions/python/bot.py < "$input")
    for lang in c cpp python rust; do
        case "$lang" in
            c)      cmd="$BIN/bot_c" ;;
            cpp)    cmd="$BIN/bot_cpp" ;;
            python) cmd="python3 solutions/python/bot.py" ;;
            rust)   cmd="$BIN/bot_rust" ;;
        esac
        [ -x "$BIN/bot_$lang" ] || [ "$lang" = python ] || continue
        got=$($cmd < "$input")
        if [ "$got" != "$expected" ]; then
            echo "FAIL $(basename "$input") [$lang]: attendu $expected, obtenu $got"
            fail=1
        fi
    done
done
if [ "$fail" -eq 0 ]; then echo "smash-the-code : tous les cas passent dans tous les langages"; fi
exit "$fail"
