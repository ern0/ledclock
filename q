#!/bin/bash

PORT=`./detectdevice.sh "$1"`
if [ ! -z ${PORT} ]; then
	miniterm.py ${PORT} 38400 --exit-char 3
fi
