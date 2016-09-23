#!/bin/bash

filename=$1
justfilename=$(basename "$1")
ponyname=${justfilename%.*}
ponyfile=$(cat $filename)

function grepandremove()
{
    grep "$1" | sed "s/${1}//g"
}

echo "$ponyfile" | grepandremove "BALLOON: " | grep top &>/dev/null
location=$?
balloontop=$(echo "$ponyfile" | grepandremove "BALLOON TOP: ")
balloonbottom=$(echo "$ponyfile" | grepandremove "BALLOON BOTTOM: ")
width=$(echo "$ponyfile" | grepandremove "WIDTH: ")
height=$(echo "$ponyfile" | grepandremove "HEIGHT: ")
linenr=$(echo "$ponyfile" | grep -n '$$$' | cut -f1 -d: | tail -n1)
pony=$(echo "$ponyfile" | tail -n +$((linenr+2)) | sed 's/\n/\\n/g')


echo "char ${ponyname}_topbottom                = $location;"
echo "unsigned short ${ponyname}_balloon_top    = $balloontop;"
echo "unsigned short ${ponyname}_balloon_bottom = $balloonbottom;"
echo "unsigned short ${ponyname}_width          = $width;"
echo "unsigned short ${ponyname}_height         = $height;"

echo "char ${ponyname}_pony[] = {"
echo "$pony" | xxd -i
echo "};"

