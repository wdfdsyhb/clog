#!/bin/sh
# Build and run the clog self-test with gcc/clang.
set -e
cc -std=c99 -Wall -Wextra -pedantic -O2 clog.c test_clog.c -o clog_test
./clog_test
