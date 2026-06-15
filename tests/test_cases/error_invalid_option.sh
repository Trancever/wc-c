#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

actual_stdout="${TEST_TMP_DIR}/${test_name}.stdout"
actual_stderr="${TEST_TMP_DIR}/${test_name}.stderr"

set +e
"$MYWC_BIN" -x >"$actual_stdout" 2>"$actual_stderr"
actual_status=$?
set -e

actual_out="$(cat "$actual_stdout")"
actual_err="$(cat "$actual_stderr")"

assert_eq "status" 1 "$actual_status"
assert_eq "stdout" "" "$actual_out"
assert_contains "stderr" "$actual_err" "illegal option -- x"
