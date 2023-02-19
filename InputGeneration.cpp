/******************************************************************************************
*******************************************************************************************
********************** Online Interval Scheduling with Predictions ************************
*******************************************************************************************
*******************************************************************************************/


// InputGeneratio.cpp: Code for input generation, that is, converting The Standard Workload Format (SWF) files from "Parallel Workloads Archive" into intervals.
// Synthetic inputs are supported too, but they are not used in the paper.
//

#include <climits>
#include "InputGeneration.h"

/* generate a uniform input file with n intervals, with intervals having
   uniform lengths between "interval_min_length" and "interval_max_length".
   The start length is then selected uniformly between 0 and "graph_size" - length
   The output file will be recorded in the same folder.    */
string generate_uniform_input(int interval_min_length, int interval_max_length, int n_numbers, int graph_size)
{
    double a = (int)interval_min_length;
    double b = (int)interval_max_length;

    string file_name = "Uniform_";
    file_name += to_string((int)a) + "_";
    file_name += to_string((int)b) + "_";
    file_name += to_string((int)graph_size) + "_";
    file_name += to_string(n_numbers) + ".txt";
    cout << "genrating: " << file_name << endl;

    ofstream myFile;
    myFile.open(file_name);

    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(a, b);

    for (int i = 0; i < n_numbers; i++)
    {
        int nextlen = distr(generator);
        myFile << nextlen << "\t";
        int nextst = 0;
        if (nextlen != graph_size)
            nextst = rand() % (graph_size - nextlen);
        myFile << nextst << endl;
    }
    myFile.close();
    return file_name;
}

/* generate a truncated normal input file with n intervals, with intervals having
   normal length with the given mean and std deviation. The normal lengths are truncated to be in [interval_min_length, interval_max_length]
   The start length is then selected uniformly between 0 and "graph_size" - length
   The output file will be recorded in the same folder.    */

   /* string generate_normal_input(double interval_length_mean, double interval_length_std_dev, int interval_min_length, int interval_max_length, int n_numbers, int graph_size)
{
    double mu = interval_length_mean; // 100.0;
    double sigma = interval_length_std_dev; // 10.0;
    double a = (int) interval_min_length; // 1.0;
    double b = (int) interval_max_length; //  10000.0;
    int seed = 123456789; //used for the truncated normal distribution

    ofstream myfile;
    string file_name = "Normal_";
    file_name += to_string((int)mu) + "_";
    file_name += to_string((int)sigma)+ "_";
    file_name += to_string((int)a) + "_";
    file_name += to_string((int)b) + "_";
    file_name += to_string(n_numbers) + ".txt";
    cout << "genrating: " << file_name << endl;
    myfile.open(file_name);
    for (int i = 0; i < n_numbers; i++)
    {
        int nextlen = (int)truncated_normal_ab_sample(mu, sigma, a, b, seed);
        myfile << nextlen << "\t";

        int nextst = 0;
        if (nextlen != graph_size)
            nextst = rand() % (graph_size - nextlen);
        myfile << nextst << endl;
    }
    myfile.close();
    return file_name;
}*/

/* generate "n_numbers" random intervals(with uniform and normal lengths) recorded on different files.
the range for the interval lengths and mean/std-dev is fixed inside this method.
*/
void generate_random_input_files(int n_numbers, int graph_size)
{
    time_t t;
    srand((unsigned)time(&t));     /* Intializes random number generator */

    int interval_min_length = 1;
    int interval_max_length = graph_size / 100;

    generate_uniform_input(interval_min_length, interval_max_length, n_numbers, graph_size);

    /*
    double  interval_length_mean = 100;
    double  interval_length_std_dev = 10;
    generate_normal_input(interval_length_mean, interval_length_std_dev, interval_min_length, interval_max_length, n_numbers, graph_size);*/
}

/*
read and record a set of intervals from a given .swf file name (e.g., "allBenchmark/Intel-NetbatchB-2012-1.swf").
see https://www.cs.huji.ac.il/labs/parallel/workload/swf.html for details on swf file format
The output will be text with "_abbr" added to the name with one interval on each line; the first entry indicates
the length, and the second indicates the start of the interval.
For example, the output is recorded at "allBenchmark/Intel-NetbatchB-2012-1_abbr.txt"  */

void generate_input_from_benchmark(string fileName)
{
    ifstream* swf = new ifstream(fileName);
    if (swf->fail())
    {
        cout << "Error: cannot find file " << fileName << endl;
        return;
    }
    cout << "Processing " << fileName << " ..." << endl;
    string myFileName = "Input/" + fileName.substr(10, fileName.length()-14) + "_abbr.txt";
    ofstream myFile (myFileName);

    if (myFile.fail())
    {
        cout << "Error: cannot create file " << myFileName << endl;
        return;
    }
    string line;
    int i = 0;
    myFile << "; interval lengths generated from: " << fileName << endl;
    myFile << "; the first field is the interval length and the second is start time\n";
    myFile << "; statistical data included at the end of the file\n;\n";
    int sumLength = 0;
    int minLength = INT_MAX;
    int maxLength = INT_MIN;
    int minStartTime = INT_MAX;
    int maxFinishTime = INT_MIN;
    int nIntervals = 0;
    while (getline(*swf, line))
    {
        std::istringstream iss(line);
        if (line[0] != ';') // ; is used for commenting out a line
        { 
            int garbage, submit_time, wait_time, run_time;
            iss >> garbage; // job number; we don't need it
            iss >> submit_time; // submit time; we don't need it
            iss >> wait_time; // start = submit + wait time
            iss >> run_time; // job length
            int start_time = submit_time + wait_time;
            if (run_time > 0 && start_time >= 0)
            {
                sumLength += run_time;
                minLength = min(minLength, run_time);
                maxLength = max(maxLength, run_time);
                minStartTime = min(minStartTime, start_time);
                maxFinishTime = max(maxFinishTime, start_time + run_time);
                nIntervals++;

                myFile << run_time << "\t" << start_time << endl;
            }
        }
        i++;
    }
    myFile << ";\n; number of intervals: " << nIntervals << endl;
    myFile << "; average length: " << (double)sumLength / nIntervals << endl;
    myFile << "; min length: " << minLength << endl;
    myFile << "; max length: " << maxLength << endl;
    myFile << "; time range (indicating graph size): (" << minStartTime << "," << maxFinishTime << ")" << endl;
    myFile.close();
    cout << "the generated file is in: " << myFileName << endl << endl;
    swf->close();
}

/* process all files listed in "allBenchmark/directory.txt", and create abbreviated interval files in a textual format
the outputs will be textual with the same names, with "_abbr" added.
in an output file, each interval is tored in a line, with length first and start_time next.
*/
void process_files()
{
    string directory = "Benchmark/directory.txt";
    ifstream* allFiles = new ifstream(directory);
    if (allFiles->fail())
    {
        cout << "Error: the file Benchmark/directory.txt is missing\n";
        return;
    }
    cout << "\n\nopened " << directory << "\n";
    string line;
    while (getline(*allFiles, line))
    {
        if (line.size()>=1)
	{
	        line.pop_back();
		line = "Benchmark/" + line;
                generate_input_from_benchmark(line);
	}
    }
}
