#!/bin/bash

rm -f ./virTool

g++ main.cpp virTool.cpp -o virTool -lvirt

echo 'build success'
