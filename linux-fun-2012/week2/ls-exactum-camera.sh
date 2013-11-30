#!/bin/bash

# this script is used for list files and subdirectories of Exactum camera

path="/cs/fs/home/tkt_cam/public_html/"
#file_path="${path}2011/11 ${path}2011/12 ${path}2012"
file_path="${path}2011/11"
cmd=$1

if [ "$cmd" == "-jpg" ]; then	
    ls -R $file_path | grep '.jpg' > ./ls-exactum-cam-jpg.txt
elif [ "$cmd" == "-c" ]; then
    ls -R $file_path | grep '.jpg' | wc -l
else
    ls -R $file_path > ./ls-exactum-cam.txt
fi
