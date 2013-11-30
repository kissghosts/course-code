#!/bin/bash

# find the highest temperature

path=$@
high_temp=0
low_temp=60
high_temp_date=""
low_temp_date=""

for dir in $path; do
    data=$(find $dir -type f -name '*hp-temps.txt' -exec grep "PROCESSOR_ZONE" \
    {} + | sed 's/[ |\/]\+/ /g' | sed 's/ tmp yanhe lost24 monitor //g' \
    | cut -d ' ' -f 1,2,5 | sed 's/C//g' | sed 's/ /\n/g')
    
    i=0
    date=""
    while read line; do
        date+=" $line"
        i=$[$i + 1]
        if [ $i == 3 ]; then
            if [ $line -gt $high_temp ]; then
                high_temp=$line
                high_temp_date=$date
            elif [ $line -lt $low_temp ]; then
                low_temp=$line
                low_temp_date=$date
            fi

            i=0
            date=""
        fi
    done <<< "$data"
done

echo "$high_temp_date"
echo "$low_temp_date"

exit 0
