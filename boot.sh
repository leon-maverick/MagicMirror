#!/bin/bash
sleep 10s 
sudo pigpiod
cd /home/pi/Main
./scurl
node Index_Mehrdad.js
kweb   file:///home/pi/Main/aaPortrait.html --display=:0 &
sleep 16s
"xte 'key F11' 'usleep 4500' 'key F5' -x:0"
sleep 5s
#crontab -l  > mycron

#echo "*/1 * * * * bash /home/pi/Main/update.sh  >> /home/pi/update.txt 2>&1" >> mycron
#crontab mycron
#rm mycron

exit
