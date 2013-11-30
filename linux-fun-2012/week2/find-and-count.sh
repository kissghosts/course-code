#!/bin/bash

# this script is used for text find and wc together

path="/cs/fs/home/tkt_cam/public_html/"

find $path -type f -name "2011$(date +%m)*.jpg" | wc -l
