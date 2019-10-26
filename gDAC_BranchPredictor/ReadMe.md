## Implemented global Divide-and-conquer (gDAC) branch predictor with a combination of ahead pipe-lining, history segmentation and prediction fusion. Also, implemented 1.5 bit counters instead of 2-bit counters for faster access times. Eventually, compared its performance with other built-in branch predictors in gem5.

-- Testing the patch file
The patch file is generated using the gem5-master source directory downloaded from dropbox and our modified gem5-master directory. To patch source with our patch file you need to change the path in the patch file or download the patch file to the path where your local gem5-master source directory is present and run the following command inside the directory:
  * patch -p1 < gDAC_gem5_patchfile.patch

-- After applying the patch run the following commands in the gem5-master directory
 1) scons -j4 build/X86/gem5.opt -- To build the simulator

 2) run the following command to test the build.
 build/X86/gem5.opt --outdir=tests configs/example/se.py --cpu-type=DerivO3CPU --caches  --l2cache --bp-type='gDacBP8K' -c ../../benchmark/bin/queens_opt -o "-c 11"
 Change the argument to '--bp-type=gDacBP8K' to test gDAC policy for 8Kb, '--bp-type=gDacBP32K' to test gDAC policy for 32Kb
 Change the L2_CACHE_SIZE to 256kB, 1MB, 4MB to test the policies with different L2 sizes to test the performance.

./build/X86/gem5.opt --outdir=tests configs/example/se.py --cpu-type=DerivO3CPU --caches --l2cache --bp-type='gDacBP8K' -c $(PATH_TO_BINARY_FILE_IN_BENCHMARK_DIRECTORY)/(MST_opt/BFS_opt/Queens) -o $(PATH_TO_benchmark_inputs)/(R10k.graph/rand-weighted-small.graph/"-c 11")

IPC and branch accuracy calculated using ((total predicted values - incorrect values)/total predicted values) value is taken from file stats.txt inside tests directory created by running above command. The calculated values for all policies with different sizes is mentioned in the report.
