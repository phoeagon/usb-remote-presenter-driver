#!/bin/bash
# for very strange reason this doesn't work when udev invoke this

#set -x 
#xhost local:phoeagon
#export DISPLAY=:0.0

sudo -u phoeagon notify-send "launch.sh executed"
sudo -u phoeagon ./usbdongle 
