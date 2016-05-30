#!/bin/bash
sudo -u pi epiphany-browser -a --profile ~/.config  file:///home/pi/Main/aaPortrait.html --display=:0 &
sleep 15s;
xte "key F11" -x:0 
exit
