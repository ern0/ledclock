#!/bin/bash

NUM=$1
if [ -z $NUM ]; then
	NUM=1
fi

if [ "$(uname)" == "Darwin" ]; then
	ls -1 /dev/tty.* | grep -v Bluetooth | head -n $NUM | tail -n 1
else
	ls -1 /dev/ttyUSB? | head -n $NUM | tail -n 1
fi
