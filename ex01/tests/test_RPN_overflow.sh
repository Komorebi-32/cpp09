#!/usr/bin/env bash

set -eu

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
exercise_dir=$(cd -- "$script_dir/.." && pwd)
binary="$exercise_dir/RPN"
green='\033[0;32m'
red='\033[0;31m'
reset='\033[0m'

make -C "$exercise_dir"

power_of_two()
{
	local exponent=$1
	local expression=2

	for ((i = 1; i < exponent; ++i)); do
		expression+=" 2 *"
	done
	printf '%s' "$expression"
}

run_overflow_test()
{
	local description=$1
	local expression=$2
	local output
	local status=0

	output=$("$binary" "$expression" 2>&1) || status=$?
	if ((status == 0)) || ! grep -qx 'Error' <<< "$output"; then
		printf '%bFAIL%b: %s\n' "$red" "$reset" "$description"
		printf '  Input:          %s\n' "$expression"
		printf '  Exit status:    %s\n' "$status"
		printf '  Program output (stdout/stderr):\n'
		printf '%s\n' "$output" | sed 's/^/    /'
		exit 1
	fi
	printf '%bPASS%b: %s\n' "$green" "$reset" "$description"
	printf '  Input:          %s\n' "$expression"
	printf '  Exit status:    %s\n' "$status"
	printf '  Program output (stdout/stderr):\n'
	printf '%s\n' "$output" | sed 's/^/    /'
	printf '\n'
}

power_30=$(power_of_two 30)
int_min="$power_30 1 3 - *"
int_max="$power_30 $power_30 1 - +"

run_overflow_test "addition above INT_MAX" \
	"$int_max 1 +"
run_overflow_test "addition below INT_MIN" \
	"$int_min 1 2 - +"
run_overflow_test "subtraction above INT_MAX" \
	"$int_max 1 2 - -"
run_overflow_test "subtraction below INT_MIN" \
	"$int_min 1 -"
run_overflow_test "multiplication below INT_MIN" \
	"$int_min 2 *"

power_of_nine=9
for ((i = 1; i < 10; ++i)); do
	power_of_nine+=" 9 *"
done
run_overflow_test "multiplication above INT_MAX" "$power_of_nine"
run_overflow_test "INT_MIN divided by -1" "$int_min 1 2 - /"
