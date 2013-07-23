/* Found this on a forum or mailing list somewhere.
Calls feh with random image if a directory is specified. 
Allows you to browse all files in a directory in a random order
*/

#!/bin/bash

shopt -s nullglob

if [[ ! -f $1 ]]; then
  echo "$0: first argument is not a file" >&2
	exit 1
fi

file=$(basename -- "$1")
dir=$(dirname -- "$1")
arr=()
shift

cd -- "$dir"

for i in *; do
	[[ -f $i ]] || continue
	arr+=("$i")
	[[ $i == $file ]] && c=$((${#arr[@]} - 1))
done

exec feh "$@" -- "${arr[@]:c}" "${arr[@]:0:c}"
