#!/bin/sh

apk_file=$1
if [ $# -eq 0 ]; then
  echo "Arg 1 must be the path to the apk file"
  exit 1
fi

unzip "$apk_file" -d gdps

# clean unnecessary assets
rm -f ./gdps/lib/armeabi-v7a/libeveryplay.so
rm -rf ./gdps/assets/fonts/

# setup directories
cp -r ./gdps/lib/armeabi-v7a/. ./app/src/full/jniLibs/armeabi-v7a/
cp -r ./gdps/assets/. ./app/src/full/gjAssets

rm -rf gdps
