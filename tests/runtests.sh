#!/bin/bash
#set -e

echo ""
echo "Running all unit tests:"
echo ""

for t in tests/*_test
do
    if test -f $t
    then
        if $VALGRIND ./$t 2>> tests/error_log.txt 
        then
            echo $t "PASS"
        else
            echo "ERROR in test $t:"
            echo "--------"
            tail tests/error_log.txt
            exit 1
        fi
    fi
done 