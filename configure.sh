#!/bin/sh

set -e

[ -d _build ] || mkdir _build
cd _build
cmake ..
