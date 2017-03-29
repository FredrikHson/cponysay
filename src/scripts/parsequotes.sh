#!/bin/bash

files=$(for i in "$@"; do echo $i;done | sort -V)
function Cfile
{
    echo "/* Auto Generated file DO NOT edit*/"
    echo ""
    for quote in ${files[@]}; do
        echo "unsigned char quote_$(basename ${quote} | sed 's/\./_/')[]={"
        cat ${quote} | sed 's#\$\\\$#╲#g' | sed 's#\$/\$#╱#g' |  xxd -i
        echo " ,0"
        echo "};"
    done
    counter=$(
    for i in ${files[@]}; do
        basename $i | sed 's/\..*//'
    done | uniq -c
    )
    oldifs=$IFS
    IFS=$'\n'
    for i in ${counter[@]}; do
        number=$(echo $(echo $i | awk '{print $1}'))
        name=$(echo $(echo $i | awk '{print $2}'))
        echo "unsigned char *quotes_${name}[$number]={"
        for (( j = 0; j < $number; j++ )); do
            echo -n "    quote_${name}_$j"
            if [[ j -ne $number-1 ]]; then
                echo ","
            fi
        done
        echo ""
        echo "};"
        echo "unsigned short numquotes_${name}=$number;"
    done
    IFS=$oldifs
    echo 'unsigned char mute[]="Zecora! Help me, I am mute!";'
}
function Hfile
{
    echo "/* Auto Generated file DO NOT edit*/"
    echo ""
    uniqueponies=$(
    for i in ${files[@]}; do
        basename $i | sed 's/\..*//'
    done | uniq
    )
    for name in ${uniqueponies[@]}; do
        echo "extern unsigned char *quotes_${name}[];"
    done
    for name in ${uniqueponies[@]}; do
        echo "unsigned short numquotes_${name};"
    done
    echo 'extern unsigned char mute[];'
}
Cfile > quotes.c
Hfile > quotes.h
