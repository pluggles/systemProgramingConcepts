#!/bin/bash
make clean 
make
./A1Part1
./A1Part2 >output.txt <<EOF
ABW
AFG
BRA
ZMB
IOT
ZWE
DKB
000
EOF
bash
./