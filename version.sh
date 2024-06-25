#!/bin/sh
# usage: version.sh module version_tag
set -e
cd $(dirname -- $0)
skipcheck=.version-switch

# Reset all to latest
if [ "$1" = "reset" ] && [ -f "$skipcheck" ]; then
	git restore --source=master --worktree --staged -- .
	rm "$skipcheck"
	echo "version: reset complete"
	exit 0
fi

# Only proceed if the working tree is clean (or previously modified)
if [ ! -f "$skipcheck" ] && [ -n "$(git status --porcelain)" ]; then
	echo "Your working tree has changes!"
	echo "This script is destructive and will not run in a dirty tree."
	exit 1
fi

touch "$skipcheck"
if [ "$2" = "latest" ]; then
	git restore --source=master --worktree --staged -- $1
	git clean -fd $1
	echo "version: checked out $1 from master"
else
	git restore --source=$1/$2 --worktree --staged -- $1
	git clean -fd $1
	echo "version: checked out $1 from $1/$2"
fi
