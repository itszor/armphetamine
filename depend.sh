#!/bin/sh
DIR="$1"
shift 1

case "$DIR" in
"" | ".")
    gcc -MM "$@"
    ;;
*)
    echo -n $DIR/
    gcc -MM "$@"
    ;;
esac
