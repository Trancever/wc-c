#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

expected="$(format_3_params 4 10 50 "-")"
actual="$(printf 'one two three\n\nfour\tfive\nsix seven eight nine\nlast' | "$MYWC_BIN" -)"

assert_eq "output" "$expected" "$actual"
