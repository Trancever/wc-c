#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

expected="$(format_lw_line 4 10 "-")"
actual="$(printf 'one two three\n\nfour\tfive\nsix seven eight nine\nlast' | "$MYWC_BIN" -lw -)"

assert_eq "output" "$expected" "$actual"

echo "PASS"
