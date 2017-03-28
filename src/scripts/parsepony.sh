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

balloontop=$(echo "$ponyfile" | grepandremove "BALLOON TOP: " | head -n1)
balloonbottom=$(echo "$ponyfile" | grepandremove "BALLOON BOTTOM: " | head -n1)
width=$(echo "$ponyfile" | grepandremove "WIDTH: " | head -n1)
height=$(echo "$ponyfile" | grepandremove "HEIGHT: " | head -n1)
linenr=$(echo "$ponyfile" | grep -n '$$$' | cut -f1 -d: | tail -n1)
pony=$(echo "$ponyfile" | tail -n +$((linenr+1)) | sed '/\$balloon.*\$/d' | sed 's/\n/\\n/g' | head -n $height)
master=$(echo "$ponyfile" | grepandremove "MASTER: " | head -n1)
if [[ -z "${master}" ]]; then
    master="${justfilename%.*}"
fi

oldifs=$IFS
IFS=$'\n'
talkline=( $(echo "$pony" | grep -o -E -n '.*\$\\\$|.*\$/') )
tlmax=0
tlmin=999999

for i in ${talkline[@]}; do
    tlinenr=$(echo $i | cut -f1 -d:)
    if [[ $tlmin -ge $tlinenr ]]; then
        tlmin=$tlinenr
    fi
    if [[ $tlmax -le $tlinenr ]]; then
        tlmax=$tlinenr
    fi
done

IFS=$oldifs
if [[ $tlmin -eq 1 ]]; then
    location=0
    balloonmin=$(echo "${pony}" | head -n 1)
else
    location=1
    balloonmin=$(echo "${pony}" | head -n $tlmax | tail -n 1)
fi
balloonmin=$(echo "$balloonmin" | grep  -E -o  '.*\$\\\$|.*\$/\$' | wc -c)

echo "/* Auto Generated file DO NOT edit*/"
echo "/* Master ${master} */"
echo "char ${ponyname}_topbottom                = $location;"
echo "char ${ponyname}_name[]                   = \"${ponyname}\";"
echo "unsigned short ${ponyname}_balloon_width  = $balloonmin;"
echo "unsigned short ${ponyname}_balloon_top    = $balloontop;"
echo "unsigned short ${ponyname}_balloon_bottom = $balloonbottom;"
echo "unsigned short ${ponyname}_width          = $width;"
echo "unsigned short ${ponyname}_height         = $height;"

echo "unsigned char ${ponyname}_pony[] = {"
echo "$pony" | sed 's#\$\\\$#╲#g' | sed 's#\$/\$#╱#g' |  xxd -i
echo " ,0"
echo "};"
