#!/bin/bash

inputfiles=( $@ )

function getname()
{
    ponyname=$(basename $1 | sed "s/\.c//g" | sed "s/-/_/g")
}
function getreferences()
{
    for i in ${inputfiles[@]}; do
        getname $i
        echo "    ${2}${ponyname}_$1,"
    done
    echo "    0"
}
function getquotes()
{
    for i in ${inputfiles[@]}; do
        master=$(awk 'NR==2 {print $3}' $i)
        grep "_${master}\[" quotes.c -q
        if [[  $? -ne 0 ]]; then
            master="mute"
        fi
        getname $i
        echo "    ${2}${1}_${master},"
    done
    echo "    0"
}
function Cfile
{
    echo "/* Auto Generated file DO NOT edit */"
    echo ""
    echo "#include \"quotes.h\""
    for i in ${inputfiles[@]}; do
        getname $i

        echo "extern char ${ponyname}_topbottom;"
        echo "extern char ${ponyname}_name[];"
        echo "extern unsigned short ${ponyname}_balloon_width;"
        echo "extern unsigned short ${ponyname}_balloon_top;"
        echo "extern unsigned short ${ponyname}_balloon_bottom;"
        echo "extern unsigned short ${ponyname}_width;"
        echo "extern unsigned short ${ponyname}_height;"
        echo "extern unsigned char ${ponyname}_pony[];"
    done
    echo "char* allponies_topbottom[]={"
    getreferences topbottom '&'
    echo "};"
    echo "char* allponies_name[]={"
    getreferences name
    echo "};"
    echo "unsigned short* allponies_balloon_width[]={"
    getreferences  balloon_width '&'
    echo "};"
    echo "unsigned short* allponies_balloon_top[]={"
    getreferences  balloon_top '&'
    echo "};"
    echo "unsigned short* allponies_balloon_bottom[]={"
    getreferences balloon_bottom '&'
    echo "};"
    echo "unsigned short* allponies_width[]={"
    getreferences width '&'
    echo "};"
    echo "unsigned short* allponies_height[]={"
    getreferences height '&'
    echo "};"
    echo "unsigned char* allponies_pony[]={"
    getreferences pony
    echo "};"
    echo "unsigned char** allponies_quotes[]={"
    getquotes "quotes"
    echo "};"
    echo "unsigned short* allponies_numquotes[]={"
    getquotes "numquotes" '&'
    echo "};"
    echo "int numPonies = ${#inputfiles[@]};"
}
function Hfile
{
    echo "/* Auto Generated file DO NOT edit */"
    echo ""
    echo "extern char* allponies_topbottom[];"
    echo "extern char* allponies_name[];"
    echo "extern unsigned short* allponies_balloon_width[];"
    echo "extern unsigned short* allponies_balloon_top[];"
    echo "extern unsigned short* allponies_balloon_bottom[];"
    echo "extern unsigned short* allponies_width[];"
    echo "extern unsigned short* allponies_height[];"
    echo "extern unsigned char* allponies_pony[];"
    echo "extern unsigned char** allponies_quotes[];"
    echo "extern unsigned short* allponies_numquotes[];"
    echo "extern int numPonies;"
}
Cfile > ponies.c
Hfile > ponies.h
