#!/usr/bin/env sh
# ------------------------------------------------------------------------------
# Run test program until input and output differ. Input is a figure.
#
# Dependencies:
# - viu (https://github.com/atanunq/viu)
# - convert (ImageMagick)
# - md5 or md5sum
# ------------------------------------------------------------------------------
catprog="./ccat"

fn=$1 # figure filename
if [ -z "$fn" ]; then
    echo "usage: $0 <figure>"
    exit 1
elif [ ! -f "$fn" ]; then
    echo "could not find $fn"
    exit 1
fi

ext="jpg"

# use the same extension for the output and temp files
output="output.$ext"
temp="temp.$ext"
expected=$(md5sum "$fn" | cut -d" " -f1)

if [ ! -f "$catprog" ]; then
    echo "could not find $catprog"
    exit 1
fi

chmod +x $catprog

# remove left overs
rm -f $temp $output

time while true; do

    # run test program
    echo "run $i"
    $catprog $fn > $temp

    # compare sum and stop script if they differ
    sum=$(md5sum "$temp" | cut -d" " -f1)
    if [ "$sum" != "$expected" ]; then
        echo "Incorrect md5sum @ run $i"
        echo "Expected: $expected ($fn)"
        echo "Actual  : $sum ($temp)"
        exit 1
    fi
done

echo "No differences found in $i iterations"
