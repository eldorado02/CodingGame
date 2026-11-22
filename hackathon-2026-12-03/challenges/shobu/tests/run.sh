#!/bin/sh
# Compare les 4 implémentations sur les cas de fuzz.
set -e
cd "$(dirname "$0")/.."

python3 tests/gen_cases.py

BIN=/tmp/opencode/shobu-tests
mkdir -p "$BIN"
export PATH="$HOME/.cargo/bin:$PATH"
g++ -O2 -std=c++20 solutions/cpp/bot.cpp -o "$BIN/shobu_cpp"
gcc -O2 -std=c11 -Wall -Wextra solutions/c/bot.c -o "$BIN/shobu_c" 2>/dev/null
(cd solutions/rust && rustc -O bot.rs -o "$BIN/shobu_rs")

fail=0
total=0
for f in tests/fuzz/*.in; do
    total=$((total+1))
    ref=$(python3 solutions/python/bot.py < "$f")
    cpp=$("$BIN/shobu_cpp" < "$f")
    c=$("$BIN/shobu_c" < "$f")
    rs=$("$BIN/shobu_rs" < "$f")
    if [ "$ref" != "$cpp" ] || [ "$ref" != "$c" ] || [ "$ref" != "$rs" ]; then
        echo "FAIL $f"
        echo "  py:  $ref"
        echo "  cpp: $cpp"
        echo "  c:   $c"
        echo "  rs:  $rs"
        fail=$((fail+1))
        if [ "$fail" -ge 5 ]; then break; fi
    fi
done
echo "shobu : $((total-fail))/$total cas identiques dans 4 langages"
[ "$fail" -eq 0 ]
