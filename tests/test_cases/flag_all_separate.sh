#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file="${TEST_TMP_DIR}/${test_name}/file.txt"
printf 'one two three\n\nfour\tfive\nsix seven eight nine\nlast' > "$file"

flag_l="-l"
flag_w="-w"
flag_c="-c"

expected="$(format_lwc_line 4 10 50 "$file")"
actual="$("$MYWC_BIN" "$flag_l" "$flag_c" "$flag_w" "$file")"

assert_eq "output" "$expected" "$actual"

echo "PASS"
