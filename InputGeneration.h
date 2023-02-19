/******************************************************************************************
*******************************************************************************************
********************** Online Interval Scheduling with Predictions ************************
*******************************************************************************************
*******************************************************************************************/


// InputGeneration.h: Header file for input generation functions, that is, methods used to convert The Standard Workload Format (SWF) files from "Parallel Workloads Archive" into intervals.
// Synthetic inputs are supported too, but they are not used in the paper.

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <unordered_map>
#include <utility>
#include <time.h>
#include <random>
#include <iostream>
#include <fstream>
#include <string>

// #include "truncated_normal.hpp"
#include <set>
#include <sstream>

using namespace std;

/* generate a uniform input file with n intervals, with intervals having
   uniform lengths between "interval_min_length" and "interval_max_length".
   The start length is then selected uniformly between 0 and "graph_size" - length
   The output file will be recorded in the same folder.    */
string generate_uniform_input(int interval_min_length, int interval_max_length, int n_numbers, int graph_size);

/* generate a truncated normal input file with n intervals, with intervals having
   normal length with the given mean and std deviation. The normal lengths are truncated to be in [interval_min_length, interval_max_length]
   The start length is then selected uniformly between 0 and "graph_size" - length
   The output file will be recorded in the same folder.    */

// string generate_normal_input(double interval_length_mean, double interval_length_std_dev, int interval_min_length, int interval_max_length, int n_numbers, int graph_size);

/* generate "n_numbers" random intervals(with uniform and normal lengths) recorded on different files.
the range for the interval lengths and mean/std-dev is fixed inside this method.
*/
void generate_random_input_files(int n_numbers, int graph_size);

/*
read and record a set of intervals from a given .swf file name (e.g., "allBenchmark/Intel-NetbatchB-2012-1.swf").
see https://www.cs.huji.ac.il/labs/parallel/workload/swf.html for details on swf file format
The output will be text with "_abbr" added to the name with one interval on each line; the first entry indicates
the length, and the second indicates the start of the interval.
For example, the output is recorded at "allBenchmark/Intel-NetbatchB-2012-1_abbr.txt"  */
void generate_input_from_benchmark(string fileName);

/* process all files listed in "allBenchmark/directory.txt", and create abbreviated interval files in a textual format
the outputs will be textual with the same names, with "_abbr" added.
in an output file, each interval is tored in a line, with length first and start_time next.
*/
void process_files();
