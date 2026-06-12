#!/usr/bin/env bash

assert_eq() {
  local label="$1"
  local expected="$2"
  local actual="$3"

  if [ "$actual" != "$expected" ]; then
    echo "FAIL"
    echo "expected $label: $expected"
    echo "actual $label:   $actual"
    exit 1
  fi
}

format_lwc_line() {
  local lines="$1"
  local words="$2"
  local bytes="$3"

  printf "%8s%8s%8s" "$lines" "$words" "$bytes"

  if [ "$#" -eq 4 ]; then
    printf " %s" "$4"
  fi
}

format_lw_line() {
  local lines="$1"
  local words="$2"

  printf "%8s%8s" "$lines" "$words"

  if [ "$#" -eq 3 ]; then
    printf " %s" "$3"
  fi
}

format_l_line() {
  printf "%8s" "$1"

  if [ "$#" -eq 2 ]; then
    printf " %s" "$2"
  fi
}

format_w_line() {
  printf "%8s" "$1"

  if [ "$#" -eq 2 ]; then
    printf " %s" "$2"
  fi
}

format_c_line() {
  printf "%8s" "$1"

  if [ "$#" -eq 2 ]; then
    printf " %s" "$2"
  fi
}
