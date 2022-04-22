#!/bin/bash

rm -f ./virTool

g++ main.cpp virTool.cpp virImpl.cpp -o virTool -lvirt -lvirt-qemu -lpthread -ltinyxml2 -I "../third_party/tinyxml2/include" -L "../third_party/tinyxml2/lib"

echo 'build success'
