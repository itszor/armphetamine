#!/bin/bash
x=`arm-linux-readelf armtest --syms|grep testfunc|cut -b 10-16`
y=`arm-linux-readelf armtest --syms|grep done|cut -b 10-16`
sed s/start-addr/0xc$x/g < testtmpl.txt > testtmplb.txt
sed s/end-addr/0xc$y/g < testtmplb.txt > test.txt
rm -f testtmplb.txt

