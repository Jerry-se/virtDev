#!/bin/bash

rm -f ./virTool

g++ main.cpp virtDev.cpp virImpl.cpp -o virTool -lvirt -lpthread

echo 'build success'
