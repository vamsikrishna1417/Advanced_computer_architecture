#!/bin/bash

rm -rf random_read_output.txt

make clean
make

for i in {5..22}
do
  for j in {0..20}
  do
    taskset 0x01 ./random_read_mountain_tester $i &>> log_file.txt
  done

  awk '{ total += $1; count++ } END { print total/count >> "random_read_output.txt" }' log_file.txt

  rm -rf log_file.txt

done
