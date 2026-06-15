#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file1="${TEST_TMP_DIR}/${test_name}/file1.txt"
printf 'one two three\n\nfour\tfive\nsix seven eight nine\nlast' > "$file1"

file2="${TEST_TMP_DIR}/${test_name}/file2.txt"
printf 'short line' > "$file2"

flag="-L"


expected="$(printf "%s\n%s\n%s" \
  "$(format_1_param 20 "$file1")" \
  "$(format_1_param 10 "$file2")" \
  "$(format_1_param 20 "total")")"
actual="$("$MYWC_BIN" "$flag" "$file1" "$file2")"

assert_eq "output" "$expected" "$actual"
