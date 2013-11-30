#!/bin/bash

# this is a filter used together with max-temp.sh
# e.g. ./max-temp.sh /tmp/lost24/monitor/2011.07.21 | ./in-tsv-format.sh

while read line; do
    # echo $line

    echo $line | sed 's/\//\ /g' | sed 's/^.*monitor\ //g' \
         | sed 's/\ hp-temps.txt\ /\    /g' | sed 's/\ C//g' 
done

exit 0


