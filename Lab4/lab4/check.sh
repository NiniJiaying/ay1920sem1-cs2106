#!/bin/sh

for var in 1 2 3 4
do
    FILE_NAME=ex$var
    gcc runner.c $FILE_NAME.c -o $FILENAME -lrt -pthread
    $FILENAME < ${FILE_NAME}_sample1.in
#    diff -w $FILE_NAME.out ${FILE_NAME}_sample1.out
done
