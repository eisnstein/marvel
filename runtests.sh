#!/bin/bash
#set -e

echo ""
echo "Running all Tests:"
echo ""

cd tests
rm -f ./error_log.txt

for t in ./*_test
do
    if test -f $t
    then
        if $VALGRIND $t 2>> ./error_log.txt 
        then
            echo ""
        else
            echo "ERROR in test $t:"
            echo "--------"
            tail ./error_log.txt
            exit 1
        fi
    fi
done 