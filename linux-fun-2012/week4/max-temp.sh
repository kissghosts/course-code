#!/bin/bash

# find the highest temperature

path=$@
high_temp=0
file_name=""

for dir in $path; do
    for file in $(find $dir -type f -name '*hp-temps.txt'); do
        temp=$(cat $file | grep 'PROCESSOR_ZONE' | sed 's/[ |\/]\+/,/g' | \
        cut -d "," -f 3 | cut -d "C" -f 1)
        
        if [ $temp -gt $high_temp ]; then
            high_temp=$temp
            file_name=$file
        fi
    done
done

echo "$file_name $high_temp C"

exit 0
