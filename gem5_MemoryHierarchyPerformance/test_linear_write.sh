#!/bin/bash

rm -rf linear_write_output.txt

make clean
make

for i in {1..22}
do
  for j in {0..20}
  do
    taskset 0x01 ./linear_write_mountain_tester $i &>> log_file.txt
  done

  awk '{ total += $1; count++ } END { print total/count >> "linear_write_output.txt" }' log_file.txt

  rm -rf log_file.txt

done
