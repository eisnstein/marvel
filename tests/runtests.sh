#!/bin/bash
#set -e

echo ""
echo "Running all unit tests:"
echo ""

for test in "tests/*_test"; do
    if [ -x $test ]; then
        if $VALGRIND ./$test 2>> ./tests/error_log.txt; then
            echo $test "PASS"
        else
            echo "ERROR in test $test:"
            echo "--------"
            tail ./tests/error_log.txt
            exit 1
        fi

        echo ""
    fi
done 