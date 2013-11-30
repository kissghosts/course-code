#!/bin/bash

# find the lowest temperature

path=$@
min_temp=60
file_name=""

for dir in $path; do
    for file in $(find $dir -type f -name '*hp-temps.txt'); do
        temp=$(cat $file | grep 'PROCESSOR_ZONE' | sed 's/[ |\/]\+/,/g' | \
        cut -d "," -f 3 | cut -d "C" -f 1)
        
        if [ $temp -lt $min_temp ]; then
            min_temp=$temp
            file_name=$file
        fi
    done
done

echo "$file_name $min_temp C"

exit 0
