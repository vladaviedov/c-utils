#/bin/sh
# usage: repack.sh out in_list
for sublib in "${@:2}"; do
	ar -t $sublib | sed --expression='s/^/build\/obj\//' | xargs ar rvsc $1
done
