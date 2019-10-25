## Implemented gDAC branch predictor and compared its performance with other built-in branch predictors in gem5.

-- Download and copy the following files to the host machine
   1 SHIP_replacementpolicy_gem5_patchfile.patch
   2 readme.txt
   3 report.pdf

-- Testing the patch file
The patch file is generated using the gem5-master source directory downloaded from dropbox and our modified gem5-master directory. To patch source with our patch file you need to change the path in the patch file or download the patch file to the path where your local gem5-master source directory is present and run the following command inside the directory:
  * patch -p1 < 1215371417_asgn02_patchfile.patch

-- After applying the patch run the following commands in the gem5-master directory
 1) scons -j4 build/X86/gem5.opt -- To build the simulator

 2) run the following command to test the build.
 Change the argument to '--l2_rpp=LRURP()' to test LRU policy, '--l2_rpp=BRRIPRP()' to test the SRRIP POLICY and '--l2_rpp=SHIPRP()' to test the SHiP replacement policy.
 Change the L2_CACHE_SIZE to 256kB, 1MB, 4MB to test the policies with different L2 sizes to test the performance.

./build/X86/gem5.opt --outdir=tests configs/example/se.py --cpu-type=AtomicSimpleCPU --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=$(L2_CACHE_SIZE) --l2_assoc=16 '--l2_rpp=$(REPLACEMENT_POLICY)' -c $(PATH_TO_BINARY_FILE_IN_BENCHMARK_DIRECTORY)/(MST_opt/BFS_opt) -o $(PATH_TO_benchmark_assign2)/(MST_rMatGraph_WE_5_35000/MST_randLocalGraph_WE_5_100000/BFS_randLocalGraph_J_5_120000/BFS_rMatGraph_J_5_90000)

Miss rate(system.l2.overall_miss_rate::total) value is taken from file stats.txt inside tests directory created by running above command. The calculated values for all policies with different sizes is mentioned in the report.
Note : I changed hit priority to be true and btp for BRRIP to 100 as in case of SRRIP all the blocks need to be inserted with long re-reference interval. Also added necessary comments in the code for easy readability.
