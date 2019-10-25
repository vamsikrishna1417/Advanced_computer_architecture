#!/bin/bash

rm -rf gem5_linear_write.txt

for i in {2..18}
do

./build/X86/gem5.opt --outdir=tests configs/example/se.py --cpu-type=DerivO3CPU --caches --l1i_size=32kB --l1d_size=64kB --l2cache --l2_size=256kB -c /home/vamsi/STUDY/CA2/cse520-lanka-vamsi_assgn01/linear_write_mountain_tester --options="$i"

value=$(<tests/stats.txt)
echo $value | cut -d " " -f 7 >> gem5_linear_write.txt

done

gnuplot -e "set terminal png size 400,300;set output 'timegraph-gem5.png';plot 'gem5_linear_write.txt'with boxes; "
