#!/bin/bash

rm -f ./virTool

g++ main.cpp virtDev.cpp virImpl.cpp -o virTool -lvirt -lpthread -ltinyxml2 -I "../third_party/tinyxml2/include" -L "../third_party/tinyxml2/lib"

echo 'build success'
