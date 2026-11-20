#!/bin/sh
# Lance les suites de tests du socle DSA dans les 4 langages.
set -e
cd "$(dirname "$0")"
mkdir -p /tmp/opencode

gcc -O2 -std=c11 -Wall -Wextra C/dsa_selftest.c -o /tmp/opencode/dsa_c
/tmp/opencode/dsa_c

g++ -O2 -std=c++20 -Wall -Wextra CPP/dsa_selftest.cpp -o /tmp/opencode/dsa_cpp
/tmp/opencode/dsa_cpp

python3 -m doctest Python/dsa.py
python3 Python/dsa.py

if command -v cargo >/dev/null 2>&1; then
    CARGO=cargo
elif [ -x "$HOME/.cargo/bin/cargo" ]; then
    CARGO="$HOME/.cargo/bin/cargo"
else
    echo "dsa Rust : PAS DE TEST (cargo absent)" >&2
    exit 0
fi
"$CARGO" test --release --manifest-path Rust/Cargo.toml --quiet
echo "dsa Rust : OK"
