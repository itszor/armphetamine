#!/bin/bash
x="16o16i 10000000 " + `cat dhry/startaddr` + " + p"
dc -e $x
sed s/STARTADDR/0x$x/g < dhry-tmpl.txt > dhry.txt
