#!/bin/bash

rm -f ./virTest

g++ test.cpp -o virTest -lvirt -lpthread

echo 'build success'
