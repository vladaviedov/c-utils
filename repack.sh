#!/bin/sh
# usage: repack.sh build_dir out in_list
build_dir=$1
out=$2
shift 2

for sublib in $@; do
	name=$(basename $sublib .a)
	ar -t $sublib | sed -e "s~^~$build_dir/obj/~" | xargs ar rvsc $out
done
