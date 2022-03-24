#!/bin/bash
# usage: ./create_targz.sh ./lab2 cw02

readonly NAME=PiwowarskiDariusz
mkdir $NAME
cp -r $1 ./$NAME/$2
tar -czvf $NAME-$2.tar.gz $NAME/$2