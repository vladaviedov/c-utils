#/bin/sh
# usage: repack.sh build_dir out in_list
for sublib in "${@:3}"; do
	ar -t $sublib | sed --expression="s~^~$1/obj/~" | xargs ar rvsc $2
done
