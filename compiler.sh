#!/bin/bash

GCC="$(which gcc)"
CLANG="$(which clang)"

if [ "$CLANG" != "" ] && [ -x "$CLANG" ]; then
	echo clang
elif [ "$GCC" != "" ] && [ -x "$GCC" ]; then
	echo gcc
elif [ "$CC" != "" ]; then
	echo $CC
else
	echo cc
fi

