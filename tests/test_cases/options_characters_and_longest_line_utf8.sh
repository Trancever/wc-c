#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file="${TEST_TMP_DIR}/${test_name}/file.txt"
printf 'abc ąęć\n€ café\n🙂 rocket 🚀 the longest line\nlast line no newline' > "$file"

flag="-mL"

expected="$(format_2_params 63 27 "$file")"
actual="$("$MYWC_BIN" "$flag" "$file")"

assert_eq "output" "$expected" "$actual"
