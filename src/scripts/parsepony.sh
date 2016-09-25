#!/bin/bash

filename=$1
if [[ -z "$2" ]]; then
    prefix=""
else
    prefix=${2}_
fi
justfilename=$(basename "$1")
ponyname=${prefix}${justfilename%.*}
ponyname=$(echo ${ponyname} | sed "s/-/_/g")
ponyfile=$(cat $filename)

function grepandremove()
{
    grep "$1" | sed "s/${1}//g"
}

echo "$ponyfile" | grepandremove "BALLOON: " | grep top &>/dev/null
location=$?
balloontop=$(echo "$ponyfile" | grepandremove "BALLOON TOP: " | head -n1)
balloonbottom=$(echo "$ponyfile" | grepandremove "BALLOON BOTTOM: " | head -n1)
width=$(echo "$ponyfile" | grepandremove "WIDTH: " | head -n1)
height=$(echo "$ponyfile" | grepandremove "HEIGHT: " | head -n1)
linenr=$(echo "$ponyfile" | grep -n '$$$' | cut -f1 -d: | tail -n1)
pony=$(echo "$ponyfile" | tail -n +$((linenr+2)) | sed 's/\n/\\n/g')

echo "/* Auto Generated file DO NOT edit*/"
echo "char ${ponyname}_topbottom                = $location;"
echo "char ${ponyname}_name[]                   = \"${ponyname}\";"
echo "unsigned short ${ponyname}_balloon_top    = $balloontop;"
echo "unsigned short ${ponyname}_balloon_bottom = $balloonbottom;"
echo "unsigned short ${ponyname}_width          = $width;"
echo "unsigned short ${ponyname}_height         = $height;"

echo "unsigned char ${ponyname}_pony[] = {"
echo "$pony" | xxd -i
echo "};"
