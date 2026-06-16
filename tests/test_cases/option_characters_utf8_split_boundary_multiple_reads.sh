
#!/usr/bin/env bash
set -eu

source "$TEST_HELPERS"

script_name="$(basename "$0")"
test_name="${script_name%.sh}"

mkdir -p "${TEST_TMP_DIR}/${test_name}"

file="${TEST_TMP_DIR}/${test_name}/file.txt"

# Layout for BLOCK_SIZE = 4096:
# read 1: 4095 ascii bytes + first byte of emoji 1
# read 2: rest of emoji 1 + 4091 ascii bytes + first byte of emoji 2
# read 3: rest of emoji 2
printf '%4095s' '' | tr ' ' 'a' > "$file"
printf '🙂' >> "$file"
printf '%4091s' '' | tr ' ' 'b' >> "$file"
printf '🙂' >> "$file"

expected="$(format_3_params 8194 8188 8188 "$file")"
actual="$("$MYWC_BIN" -cmL "$file")"

assert_eq "output" "$expected" "$actual"