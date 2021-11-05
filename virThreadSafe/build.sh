#!/bin/bash

rm -f ./virTest
rm -f ./virMonitor

g++ test.cpp -o virTest -lvirt -lpthread
g++ monitor.cpp -o virMonitor -lvirt

echo 'build success'
