#!/bin/sh

files=`ls *.h *.c`
for i in $files; do
    cat $i|dos2unix>$i.tmp
    mv -f $i.tmp $i
    indent -orig $i
done
