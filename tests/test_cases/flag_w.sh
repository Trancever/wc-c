#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file="${TEST_TMP_DIR}/${test_name}/file.txt"
printf 'one two three\n\nfour\tfive\nsix seven eight nine\nlast' > "$file"

flag="-w"

expected="$(format_w_line 10 "$file")"
actual="$("$MYWC_BIN" "$flag" "$file")"

assert_eq "output" "$expected" "$actual"

echo "PASS"
