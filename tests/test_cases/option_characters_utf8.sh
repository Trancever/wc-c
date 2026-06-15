#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file="${TEST_TMP_DIR}/${test_name}/file.txt"
printf 'abc ąęć\n€ café\n🙂 rocket 🚀\nlast line no newline' > "$file"

expected="$(format_1_param 46 "$file")"
actual="$("$MYWC_BIN" -m "$file")"

assert_eq "output" "$expected" "$actual"
