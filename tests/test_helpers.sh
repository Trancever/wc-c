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

assert_contains() {
  local label="$1"
  local haystack="$2"
  local needle="$3"

  if [[ "$haystack" != *"$needle"* ]]; then
    echo "FAIL"
    echo "expected $label to contain: $needle"
    echo "actual $label:             $haystack"
    exit 1
  fi
}

format_optional_name() {
  if [ "$#" -eq 0 ]; then
    return
  fi

  if [ "$#" -ne 1 ]; then
    echo "format helper received too many name arguments" >&2
    exit 1
  fi

  printf " %s" "$1"
}

format_1_param() {
  if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "format_1_param expects 1 count and optional name" >&2
    exit 1
  fi

  printf "%8s" "$1"
  shift
  format_optional_name "$@"
}

format_2_params() {
  if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
    echo "format_2_params expects 2 counts and optional name" >&2
    exit 1
  fi

  printf "%8s%8s" "$1" "$2"
  shift 2
  format_optional_name "$@"
}

format_3_params() {
  if [ "$#" -lt 3 ] || [ "$#" -gt 4 ]; then
    echo "format_3_params expects 3 counts and optional name" >&2
    exit 1
  fi

  printf "%8s%8s%8s" "$1" "$2" "$3"
  shift 3
  format_optional_name "$@"
}

format_4_params() {
  if [ "$#" -lt 4 ] || [ "$#" -gt 5 ]; then
    echo "format_4_params expects 4 counts and optional name" >&2
    exit 1
  fi

  printf "%8s%8s%8s%8s" "$1" "$2" "$3" "$4"
  shift 4
  format_optional_name "$@"
}
