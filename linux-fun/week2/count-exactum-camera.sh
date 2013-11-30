#!/bin/bash

# this script is used for counting the number of jpg files in a specific month

path="/cs/fs/home/tkt_cam/public_html/"
filepath="${path}2011/11 ${path}2011/12 ${path}2012"

ls -R $filepath | grep '.jpg' | grep "^2011$(date +%m)" | wc -l
