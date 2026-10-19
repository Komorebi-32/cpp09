#!/usr/bin/env bash

set -eu

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
exercise_dir=$(cd -- "$script_dir/.." && pwd)
binary="$exercise_dir/PmergeMe"

make -C "$exercise_dir"

run_valid_test()
{
	local description=$1
	shift
	local output actual expected

	output=$("$binary" "$@")
	actual=$(printf '%s\n' "$output" | sed -n '2s/^After: //p')
	expected=$(printf '%s\n' "$@" | sort -n | paste -sd ' ' -)
	if [[ "$actual" != "$expected" ]]; then
		printf 'FAIL: %s\nExpected: %s\nActual:   %s\n' \
			"$description" "$expected" "$actual" >&2
		exit 1
	fi
	printf 'PASS: %s\n' "$description"
}

run_rejected_test()
{
	local description=$1
	shift

	if "$binary" "$@" >/dev/null 2>&1; then
		printf 'FAIL: %s was accepted\n' "$description" >&2
		exit 1
	fi
	printf 'PASS: %s is rejected\n' "$description"
}

run_valid_test "mixed input" 3 5 9 7 4 2
run_valid_test "duplicate values" 5 5 3 3 1 1 5

ascending=()
for ((value = 1; value <= 3000; ++value)); do
	ascending+=("$value")
done
run_valid_test "3000 distinct ascending integers" "${ascending[@]}"

descending=()
for ((value = 3000; value >= 1; --value)); do
	descending+=("$value")
done
run_valid_test "3000 distinct descending integers" "${descending[@]}"

alternating=()
low=1
high=3000
while ((low <= high)); do
	alternating+=("$low")
	if ((low != high)); then
		alternating+=("$high")
	fi
	((++low))
	((--high))
done
run_valid_test "3000 distinct alternating-end integers" "${alternating[@]}"

run_rejected_test "negative integer" -1
run_rejected_test "non-numeric input" abc
run_rejected_test "numeric input with trailing text" 12abc
run_rejected_test "unsigned integer overflow" 4294967296
run_rejected_test "zero input" 0
run_rejected_test "empty input token" ""
