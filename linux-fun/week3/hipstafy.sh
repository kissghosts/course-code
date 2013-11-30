#!/bin/bash

# The Immelmann
# exercise for loops

path=$1
if [ "$path" == "" ]; then
    echo "Please input the directory path!"
    exit 1
fi

cd $path
if [ $? != 0 ]; then
    echo "Cannot enter into your input directory, please check it!"
    exit 1
fi

mkdir ../output-pics
echo "The converting starts. It may take a few minutes if there are \
several pics"

# find . -type f -name '*.jpg' | cut -d "/" -f 2
for file in $(ls | grep '.jpg$')
do
    name=${file%.jpg}
    output_file=$name-hipstah.jpg
    convert -sepia-tone 60% +polaroid $file ../output-pics/$output_file
done

exit 0
