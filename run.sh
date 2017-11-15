#!/bin/bash
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"

threads=(1 2 4 8 12 16 24 48)
for i in "${threads[@]}"; do
	echo "====== $i threads ======"
	OMP_NUM_THREADS=$i ./d2p < data/$1.txt > /dev/null
done

echo "====== Serial ======"
./d2s < data/$1.txt > /dev/null
