#!/bin/bash

# the last exercise of week3, find the highest temperature

path='/tmp/yanhe/lost24/monitor/2011.10.'
high_temp=0
file_name=""

for file in $(ls ${path}*/*/hp-temps.txt)
do
    temp=$(cat $file | grep 'PROCESSOR_ZONE' | sed 's/[ |\/]\+/,/g' | \
    cut -d "," -f 3 | cut -d "C" -f 1)
    
    if [ $temp -gt $high_temp ]; then
        high_temp=$temp
        file_name=$file
    fi
done

echo "The highest is ${high_temp}C"
echo "It is recorded in file $file_name"

exit 0
