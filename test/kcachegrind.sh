#!/bin/bash

# https://stackoverflow.com/questions/23786152/how-to-interpret-results-from-kcachegrind
# Run this from the project root directory

./build.sh debug_perf static noclean norun
cd build/Debug && valgrind --tool=callgrind --callgrind-out-file=callgrind_out.txt ./testing_cuda_with_cmake
python3 ../../test/gprof2dot.py -f callgrind callgrind_out.txt | dot -Tsvg -o callgrind_out.svg
kcachegrind