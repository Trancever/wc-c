#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file_two_byte="${TEST_TMP_DIR}/${test_name}/two_byte_split.txt"
file_four_byte="${TEST_TMP_DIR}/${test_name}/four_byte_split.txt"

printf '%4095s' '' | tr ' ' 'a' > "$file_two_byte"
printf 'ą\n' >> "$file_two_byte"

printf '%4095s' '' | tr ' ' 'a' > "$file_four_byte"
printf '🙂\n' >> "$file_four_byte"

expected="$(printf "%s\n%s\n%s" \
  "$(format_2_params 4098 4097 "$file_two_byte")" \
  "$(format_2_params 4100 4097 "$file_four_byte")" \
  "$(format_2_params 8198 8194 "total")")"
actual="$("$MYWC_BIN" -cm "$file_two_byte" "$file_four_byte")"

assert_eq "output" "$expected" "$actual"
