#!/bin/bash

i=1
echo "================== input ======================"
while [ $i -le $1 ]; do echo $RANDOM; i=$(($i+1)); done | tee input
echo "============= execution result ================"
cat input | ./a.out | tee your_output
cat input | sort -n > standard_output
echo "====== differences from correct result ======="
diff your_output standard_output
echo "These are results for n=$1"
