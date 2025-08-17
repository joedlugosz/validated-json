#!/bin/bash
set -e
cd $(dirname $BASH_SOURCE[0])
cmake -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build build
ctest --output-on-failure --test-dir build
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/test/*' '*/gtest/*' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_report
