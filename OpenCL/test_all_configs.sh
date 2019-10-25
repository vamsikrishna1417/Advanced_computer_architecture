#!/bin/bash

for i in 512 1024 2048 
do
	for j in {1..2}
	do
	if [[ $j == 1 ]]; then
		echo "******'TESTING THE NORMAL KERNEL FOR MATRIX SIZE" $i "'******"
		echo ""
		g++ -std=c++11 main.cpp -lOpenCL -DSIZE=$i -DTILE_SIZE=8
		./a.out
	else
		echo ""
		echo "******'TESTING THE TILED KERNEL FOR MATRIX SIZE" $i "WITH TILE SIZE 8'******"
		echo ""
		g++ -std=c++11 main.cpp -lOpenCL -DSIZE=$i -DENABLE_TILE_KERNEL -DTILE_SIZE=8
		./a.out
		echo ""
		echo "******'TESTING THE TILED KERNEL FOR MATRIX SIZE" $i "WITH TILE SIZE 16'******"
		echo ""
		g++ -std=c++11 main.cpp -lOpenCL -DSIZE=$i -DENABLE_TILE_KERNEL -DTILE_SIZE=16
		./a.out
	fi
done
done