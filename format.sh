#!/bin/bash

# usage: ./format.sh

find . -type d -name "build" -prune -o -type f -iname "*.h" -print -o -iname "*.cpp" -print | xargs clang-format -i -style=file

