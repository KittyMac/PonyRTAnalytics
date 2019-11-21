#!/bin/sh

newPath=`echo $0 | awk '{split($0, a, ";"); split(a[1], b, "/"); for(x = 2; x < length(b); x++){printf("/%s", b[x]);} print "";}'`
cd "$newPath"

find . -type f -name '*.jpeg' -exec xattr -c {} \;
find . -type f -name '*.jpg' -exec xattr -c {} \;
find . -type f -name '*.png' -exec xattr -c {} \;
find . -type f -name '*.json' -exec xattr -c {} \;