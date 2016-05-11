#!/bin/bash

for((i=0;i<512;i++))
do
    i2=$(printf "%4.4i" $i)
    file="partition_$i2.node"
    nline=$(cat $file | wc -l)
    if [ $nline -gt 10 ] ; then
        echo "set term png" > temp.txt
        echo "set output \"partition_$i2.png\"" >> temp.txt
        echo "plot \"$file\" using 2:3:4 with points pt 7 ps 1 lt palette" >> temp.txt
        gnuplot temp.txt
    fi
done
