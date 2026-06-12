#!/usr/bin/env bash
set -eu

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

file="${TEST_TMP_DIR}/${test_name}/missing_file.txt"

actual_stdout="${TEST_TMP_DIR}/${test_name}.stdout"
actual_stderr="${TEST_TMP_DIR}/${test_name}.stderr"

set +e
"$MYWC_BIN" "$file" >"$actual_stdout" 2>"$actual_stderr"
actual_status=$?
set -e

actual_out="$(cat "$actual_stdout")"
actual_err="$(cat "$actual_stderr")"

expected_status=1
expected_out=""
expected_err="mywc: $file: open: No such file or directory"

if [ "$actual_status" -ne "$expected_status" ]; then
  echo "FAIL"
  echo "expected status: $expected_status"
  echo "actual status:   $actual_status"
  exit 1
fi

if [ "$actual_out" != "$expected_out" ]; then
  echo "FAIL"
  echo "expected stdout: $expected_out"
  echo "actual stdout:   $actual_out"
  exit 1
fi

if [ "$actual_err" != "$expected_err" ]; then
  echo "FAIL"
  echo "expected stderr: $expected_err"
  echo "actual stderr:   $actual_err"
  exit 1
fi

echo "PASS"