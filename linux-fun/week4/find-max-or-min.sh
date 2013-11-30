#!/bin/bash

# find the highest or lowest temperature

path=""

flag_max=0
flag_min=0
flag_tab=0
output_date=""

print_help() {
    echo -e "\
    Usage: $(basename $0) [-t] -c | -w -p dir
    Arguments:
        -t        output as tab-separated values
        -c        find the clodest temperature
        -w        find the hottest temperature
        -p dir    search all subdirs of dir"
}

print_result() {
    if [ $flag_tab -eq 1 ]; then
        echo $@ | sed 's/\//\ /g' | sed 's/^.*monitor\ //g' \
            | sed 's/\ hp-temps.txt\ /\    /g' | sed 's/\ C//g'
    else
        echo $@ | sed 's/\//\ /g' | sed 's/^.*monitor\ //g' \
            | sed 's/\ hp-temps.txt\ /\ /g' | sed 's/\ C//g'
    fi
}

while getopts ":tcwp:" opt; do
    case $opt in
        t)
            flag_tab=1
            ;;
        c)
            flag_min=1
            ;;
        w)  
            flag_max=1
            ;;
        p)
            path="$OPTARG"
            ;;
        *)
            print_help
            exit 1
            ;;
    esac
done

operation=$[$flag_max^$flag_min]

if [ "$path" == "" ] || [ $operation -eq 0 ]; then
    print_help
    exit 1
elif [ $flag_max -eq 1 ]; then
    result=$(./max-temp.sh "$path")
    print_result $result
elif [ $flag_min -eq 1 ]; then
    result=$(./min-temp.sh "$path")
    print_result $result
else
    echo "some other conditions I still havent found"
    exit 1
fi

exit 0
