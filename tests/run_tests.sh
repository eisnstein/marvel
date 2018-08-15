#!/bin/bash
#set -e

for test in "tests/*_test"; do
    if [ -x $test ]; then
        echo -n "running test:" $test " ... "
        if ./$test 2>> ./tests/error_log.txt; then
            echo "OK"
        else
            echo "NOT OK"
            tail ./tests/error_log.txt
        fi

        echo
    fi
done 