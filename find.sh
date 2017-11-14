#! /bin/sh

for lib in $(find . -name \*.o) ; do echo $lib ; nm $lib | grep main | grep -v " U " ; done
