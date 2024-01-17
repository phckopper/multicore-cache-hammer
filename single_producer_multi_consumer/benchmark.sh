#!/bin/bash

make all

if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi

param1=13
param2=14
param3=15

echo "Run,Elapsed Time (ns)" > elapsed_times.csv

for ((i=1; i<=10000; i++))
do
    elapsed_time=$(./benchmark $param1 $param2 $param3 | grep -oP 'Benchmark took \K\d+')

    echo "$i,$elapsed_time" >> elapsed_times.csv
done

echo "Execution complete. Elapsed times saved to elapsed_times.csv"

