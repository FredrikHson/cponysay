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
    echo 'unsigned char mute[]="Zecora! Help me, I am mute!";'
}
function Hfile
{
    echo "/* Auto Generated file DO NOT edit*/"
    echo ""
    for quote in ${files[@]}; do
        echo "extern unsigned char quote_$(basename ${quote} | sed 's/\./_/')[];"
    done
    echo 'extern unsigned char mute[];'
}
Cfile
Cfile > quotes.c
Hfile
Hfile > quotes.h
