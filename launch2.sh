#!/bin/bash
# yeah , this one works for udev, but should compile the bin in daemon mode

PROG="usbdongle"
PID=`ps -e | grep $PROG | head -1 | awk '{ print $1 }'`
echo $PID
kill -n 10 $PID
