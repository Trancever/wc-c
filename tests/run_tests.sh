#!/usr/bin/env bash
set -eu

script_dir="$(cd "$(dirname "$0")" && pwd)"
repo_dir="$(cd "$script_dir/.." && pwd)"

export MYWC_BIN="${repo_dir}/mywc"
export TEST_TMP_DIR="${script_dir}/tmp"
export TEST_HELPERS="${script_dir}/test_helpers.sh"

cc -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -o "$MYWC_BIN" "${repo_dir}/mywc.c"

mkdir -p "${script_dir}/tmp"

failed=0

for test_file in "${script_dir}"/test_cases/*.sh; do
  echo "$(basename "$test_file")"

  if bash "$test_file"; then
    :
  else
    failed=$((failed + 1))
  fi
done

if [ "$failed" -ne 0 ]; then
  echo "Some tests failed"
  exit 1
fi

echo "All tests passed"
