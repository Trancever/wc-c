#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file1="${TEST_TMP_DIR}/${test_name}/file1.txt"
printf 'one two three\n\nfour\tfive\nsix seven eight nine\nlast' > "$file1"

file2="${TEST_TMP_DIR}/${test_name}/file2.txt"
printf '\nleading spaces\n  alpha   beta\tgamma\nno-newline' > "$file2"

file3="${TEST_TMP_DIR}/${test_name}/file3.txt"
printf '' > "$file3"

expected="$(printf "%s\n%s\n%s\n%s" \
  "$(format_lwc_line 4 10 50 "$file1")" \
  "$(format_lwc_line 3 6 47 "$file2")" \
  "$(format_lwc_line 0 0 0 "$file3")" \
  "$(format_lwc_line 7 16 97 "total")")"
actual="$("$MYWC_BIN" "$file1" "$file2" "$file3")"

assert_eq "output" "$expected" "$actual"

echo "PASS"
