#/bin/bash
#todo: shellcheck

set -euo pipefail

usage="usage: $0 [-h] [-p|--show-passing] <debug|release>"
pass_count="0"
failure_count="0"
show_passing="0"
build=""
program_name="sha1++"
temp_dir=$(mktemp -d)
trap 'rm -rf -- "$temp_dir"' EXIT
stdout_file="$temp_dir/out.txt"
stderr_file="$temp_dir/err.txt"
expected_file="$temp_dir/expected.txt"

error_exit() {
  echo "$0: $1" >&2
  exit 1
}

while [ $# -gt 0 ]; do
  case "$1" in
    -h)
      echo "$usage"
      exit
      ;;
    -p|--show-passing)
      show_passing="1"
      shift
      ;;
    debug|release)
      if [ -z "$build" ]; then
        build="$1"
      else
        error_exit "multiple build options listed"
      fi
      shift
      ;;
    *)
      error_exit "unknown option: $1"
      ;;
  esac
done
if [ -z "$build" ]; then
  error_exit "build option not given"
fi

exe="../../build-$build/$program_name"

# Make sure we can find the required files
if [ -d input-files ]; then
  cd input-files
elif [ -d test/input-files ]; then
  cd test/input-files
else
  error_exit "please cd into test/.. or test/. to run the tests."
fi
if ! [ -f "$exe" ]; then
  error_exit "Can't find the executable. Is the $build build built?"
fi

CHECK_EQUAL() {
  local test_name="$1"
  local actual_output_file="$2"
  local test_result=""

  if ! cmp -s "$actual_output_file" "$expected_file"; then
    failure_count=$((failure_count + 1))
    test_result="FAIL"
  else
    pass_count=$((pass_count + 1))
    if [ "$show_passing" != "0" ]; then
      test_result="pass"
    fi
  fi
  if [ -n "$test_result" ]; then
    local actual_output="$(< "$actual_output_file")"
    local expected_output="$(< "$expected_file")"
    echo "$test_result($test_name): \"$actual_output\" == \"$expected_output\"" 
  fi
}
TEST() {
  local test_name="$1"
  local expected_code="$2"
  local expected_output="$3"
  shift 3 

  $exe "$@" > "$stdout_file" 2> "$stderr_file" && true
  local actual_code="$?"

  if [ "$expected_code" = "0" ]; then
    printf "%s\n" "$expected_output" > "$expected_file"
    CHECK_EQUAL "$test_name > stdout" "$stdout_file"

    printf "%s" "" > "$expected_file"
    CHECK_EQUAL "$test_name > stderr" "$stderr_file" 
  else
    printf "%s" "" > "$expected_file"
    CHECK_EQUAL "$test_name > stdout" "$stdout_file"

    printf "%s\n" "$expected_output" > "$expected_file"
    CHECK_EQUAL "$test_name > stderr" "$stderr_file" 
  fi
  printf "%s" "$actual_code" > "$stdout_file"
  printf "%s" "$expected_code" > "$expected_file"
  CHECK_EQUAL "$test_name > return code" "$stdout_file"
}

# Main functionality
TEST "Empty file" 0 "da39a3ee5e6b4b0d3255bfef95601890afd80709" \
  empty.txt
TEST "Much less than 1 block" 0 "2fb5e13419fc89246865e7a324f476ec624e8740" \
  less-than-1.txt
TEST "Length makes exactly 1 block" 0 "1f1e20cd8496c3537884d9fdd400edaac31e36ab" \
  length-exact-1.txt
TEST "Padding makes 1 block" 0 "8fe4c5f8464d6e1891d66bdf51e32c4f4f22604c" \
  pad-exact-1.txt
TEST "Data exactly 1 block" 0 "39e9ce0a2babc1af18b725f17ad85e90b2210e69" \
  data-exact-1.txt
TEST "Length crosses to 2 blocks" 0 "f36576a373ccb6674648b3c618457afe80d1c8b8" \
  length-to-2.txt
TEST "Healthly 2 blocks" 0 "bef7ee8de130b19df8f85c8ba6cb1dda5c5e2953" \
  2-blocks.txt
TEST "Lots of blocks" 0 "2de5d925c11a5accf01842a5579a243cfd9f714a" \
  fuzz.bin
TEST "File with leading dash" 0 "69bca99b923859f2dc486b55b87f49689b7358c7" \
  -- -file.txt

# Help flag
help_output="$program_name: this program prints the SHA-1 hash of the given file.
usage: $program_name [-h] [--] file" 

TEST "Help flag" 0 "$help_output" \
  -h
TEST "Help flag with file" 0 "$help_output" \
  -h fuzz.bin
TEST "Help flag, trailing text" 0 "$help_output" \
  -h hello world

# Error situations
TEST "Non-existent file" 1 "$program_name: error: unable to open file: non-existent.txt" \
    non-existent.txt

unknown_cmd_line_output="$program_name: unknown command line
help: $program_name -h"

TEST "Unknown flag" 1 "$unknown_cmd_line_output" \
    -z
TEST "File starts with dash" 1 "$unknown_cmd_line_output" \
    -file.txt
TEST "Out of order" 1 "$unknown_cmd_line_output" \
    fuzz.bin -h
TEST "Extra arguments" 1 "$unknown_cmd_line_output" \
    fuzz.bin foobar
TEST "Extra arguments, --" 1 "$unknown_cmd_line_output" \
    -- fuzz.bin foobar

if [ $failure_count -gt 0 ]; then
  echo
  echo "There are test failures! (BUILD: $build)"
  echo "Pass: $pass_count, Fail: $failure_count"
  echo
  exit 1
else
  echo
  echo "All tests passed. (BUILD: $build)"
  echo "Pass: $pass_count, Fail: $failure_count"
  echo
  exit 0
fi
