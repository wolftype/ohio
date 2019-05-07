#!/bin/bash

echo Compiling $1

DIRECTORY=`dirname $1`
TARGET=`basename $1|cut -d'.' -f1 | sed -e "s|/|_|g"`

echo Directory is $DIRECTORY
echo Target is $TARGET

mkdir build
mkdir build/bin
rm -rf build/bin/$TARGET
cd build

#INCLUDE="-I../hana/include/ -I../versor/ -I."
#clang++ -v -std=c++14 -stdlib=libc++ $INCLUDE $1 -o build/bin/$TARGET

RUN_CMAKE=0;

for i
  do
    case $i in
    -c | --cmake)
    RUN_CMAKE=1
    ;;
    esac
done

if [ $RUN_CMAKE = 1 ]; then

  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
fi

make VERBOSE=1-- $TARGET

echo "disabling echo and canonical input in shell"
#tput civis
stty -echo -icanon
./bin/$TARGET
echo "re-enabling echo and canonical input in shell"
stty echo icanon
#tput cnorm
