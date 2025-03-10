#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run test program until input and output differ. Input is a figure.
#
# Dependencies:
# - viu (https://github.com/atanunq/viu)
# - convert (ImageMagick)
# - md5 or md5sum
# ------------------------------------------------------------------------------
catprog="./ccat"

# set view to true so that this scripts only exits once the failed
# figure is displayable with viu
view=false

fn=$1 # figure filename
if [ -z "$fn" ]; then
    echo "usage: $0 <figure>"
    exit 1
elif [ ! -f "$fn" ]; then
    echo "could not find $fn"
    exit 1
fi

nit=$2 # maximum number of iterations
if [ -z "$nit" ]; then
    nit=100
elif ([ "$nit" -gt 0 ] && [ "$nit" -le 10000 ]) 2> /dev/null; then
    : # valid value
else
    echo "$nit is not a valid number in range = (0;1000]"
    exit 1
fi

# check input is a figure file
ext=$(file --extension "$fn" | cut -d: -f2 | tr -d " ")
if [ "$ext" != "jpeg/jpg/jpe/jfif" ] \
&& [ "$ext" != "png" ] \
&& [ "$ext" != "gif" ]; then
    echo "$fn is not a supported figure type"
    exit 1
fi

if [ "$ext" == "jpeg/jpg/jpe/jfif" ]; then
    ext="jpg"
fi

# use the same extension for the output and temp files
output="output.$ext"
temp="temp.$ext"

if [ ! -f "$catprog" ]; then
    echo "could not find $catprog"
    exit 1
fi

# remove left overs
rm -f $temp $output

# calculate expected MD5 sum
if [ "$(uname -s)" = Darwin* ] \
|| [ "$(uname -s)" = *BSD* ];  then
    use_md5=true
    expected=$(md5 -q "$fn")
else
    expected=$(md5sum "$fn" | cut -d" " -f1)
fi
i=0
time while [ "$i" -lt "$nit" ]; do
    i=$(expr $i + 1)

    # run test program
    echo "run $i"
    $catprog $fn > $temp

    # compare sum and stop script if they differ
    if [ "${use_md5}" = true ]; then
        sum=$(md5 -q "$temp")
    else
        sum=$(md5sum "$temp" | cut -d" " -f1)
    fi
    if [ true ] && [ "$sum" != "$expected" ]; then
        if (which convert && viu temp.jpg) > /dev/null 2> /dev/null; then
            convert $fn $temp +append $output 2> /dev/null
            viu $output
            viewed=true
        fi
        echo "Incorrect md5sum @ run $i"
        echo "Expected: $expected ($fn)"
        echo "Actual  : $sum ($temp)"
        if [ "$view" = false ] || [ "$viewed" = true ]; then
        	exit 1
        fi
    fi
done

if [ "$viewed" = true ]; then
	exit 1
fi

echo "No differences found in $i iterations"
