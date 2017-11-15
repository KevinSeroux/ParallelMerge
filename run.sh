#!/bin/bash
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"

threads=(1 2 4 8 12 16 24 48)
echo "POUR N=$1" 1>&2
for i in "${threads[@]}"; do
	echo "====== $i threads ======" 1>&2
	OMP_NUM_THREADS=$i ./d2p < data/$1.txt > /dev/null
done

echo "====== Serial ======" 1>&2
./d2s < data/$1.txt > /dev/null
echo "" 1>&2
