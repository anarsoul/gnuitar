#!/bin/sh

files=`ls distort2lookup*`
for i in $files; do
    ./strip_lookup_table $i $i.tmp
    mv -f $i.tmp $i
done
