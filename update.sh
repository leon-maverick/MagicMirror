#!/bin/bash
cd /home/pi/Main
./scurl
node Index_Mehrdad.js
filename="change.txt"
while IFS='' read -r line || [[ -n "$line" ]]; do 
	echo "Text read from file: $line"
	lili="$line"
done < "$filename"
if [ "$lili" -eq "0" ] 
	then 
		echo "no change"
else 
	echo "Content changed"
	>"change.txt"
	echo -n "0" >> "change.txt"
	WID=`xdotool search --onlyvisible --name "Kweb" | head -1`
	xdotool windowactivate $WID
	xdotool key alt+r

fi	
sleep 5s
exit
