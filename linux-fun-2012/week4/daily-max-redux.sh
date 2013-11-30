#!/bin/bash

# find the highest temperature of everyday in a certain dir

path=$@
high_temp=0
file_name=""
current_dir=""

for dir in $path; do
    for file in $(find $dir -type f -name '*hp-temps.txt'); do
        temp=$(cat $file | grep 'PROCESSOR_ZONE' | sed 's/[ |\/]\+/,/g' | \
        cut -d "," -f 3 | cut -d "C" -f 1)
        
        tmp_dir=$(echo $file | sed 's/[0-9]\{2\}:[0-9]\{2\}\/hp-temps\.txt$//g')
        
        if [ "$tmp_dir" != "$current_dir" ]; then
            if [ $high_temp -ne 0 ]; then
                echo "$file_name $high_temp C"
            fi

            current_dir=$tmp_dir
            high_temp=$temp
            file_name=$file
        
        else
            if [ $temp -gt $high_temp ]; then
                high_temp=$temp
                file_name=$file
            fi
        fi
    done
done

echo "$file_name $high_temp C"

exit 0
