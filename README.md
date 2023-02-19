# IntervalSchedulingWithPrediction
This repo contains the code for the project “Online Interval Scheduling with Predictions”. 

Send inquires to Shahin Kamali (kamalis@yorku.ca)

## Files Description: 

IntervalSchedulingExperiments.cpp: contains the 'main' function. Program execution begins and ends there.
IntervalSchedulingAlgorithms.h/cpp: code for scheduling algorithms like TRUST, TRUSTGREEDY, and OPT.
InputGeneration.h/cpp: code for input generation, that is, converting The Standard Workload Format (SWF) files from "Parallel Workloads Archive" into intervals. Find details about these public benchmarks at https://www.cs.huji.ac.il/labs/parallel/workload/

## Prerequisites
a c++ compiler, e.g., g++ 8.3.0 or higher

## Compile:
To compile the code, run the following:

g++ -o callAdm IntervalSchedulingAlgorithms.h InputGeneration.h IntervalSchedulingAlgorithms.cpp InputGeneration.cpp IntervalSchedulingExperiments.cpp -o intervalPlanning

The outcome will be an executable named “intervalPlanning”

## Running the code:

Simply run the following:

intervalPlanning

You will be prompted to select either input-generation or experiment modes.
- if you choose the input generation, the SWF files specified in the Benchmark/directory.txt will be converted and used to generate plane intervals stored in Input/ folder. For example, "Benchmark/x.swf" will generate "Input/x-abbr.txt."

- if you choose the experiment mode, you are asked to specify the number of steps (values for the error measure). This indicates the number of points on the x-axis in the paper's experiments (there, this value is 1000). Next, you are asked to choose the contribution of False-Positives and False-Negatives to error; enter values between 0 and 1 (in the main paper, we set both to 1). 
The algorithms are run using input files listed at Input/directory.txt. 

## Output:
Each output file contains the payoff of various algorithms, along with measured errors.
The output files will be stored in the Result/ folder. For example, results for "Input/x-abbr.txt" will be at Results/x-abbrFP100FN100-res.txt (the numbers after FP and FN indicate their contribution, scaled by 100).
