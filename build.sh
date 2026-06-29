#!/bin/sh

[ -s _build ] || { echo "Run ./configure.sh first"; exit 1; }
exec cmake --build _build $*
