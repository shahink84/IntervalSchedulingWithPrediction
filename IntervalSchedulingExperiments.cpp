/******************************************************************************************
*******************************************************************************************
********************** Online Interval Scheduling with Predictions ************************
*******************************************************************************************
*******************************************************************************************/

// IntervalAdmissionWithPredictions.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <random>
#include <algorithm>
#include "InputGeneration.h"
#include "IntervalSchedulingAlgorithms.h"


// given a file stream "inputFile", read k intervals and store them in a
void readKIntervals(ifstream* inputFile, interval* a, int k)
{
    string line;
    int i = 0;
    while (getline(*inputFile, line) && i < k)
    {
        std::istringstream iss(line);
        if (line[0] != ';') // ; is used for commenting out a line
        {
            int length, start;
            *inputFile >> length;
            *inputFile >> start;
            if (length > 0) // negative and 0 length happens sometime due to the inconsistencties in the input file; we skip these intervals 
            {
                a[i].first = start + length; // first is the end of the interval (for sorting purposes defined this way)
                a[i].second = start; // second is the beginning of the interval
                i++;
            }
        }

    }

}

// copy k intervals from set a to set b
void copyIntervals(interval* b, interval* a, int start_index, int end_index)
{
    for (int i = start_index; i < end_index; i++)
        b[i] = a[i];
}


// print a given set of intervals (used only for testing) 
void print(set<interval> a)
{
    set<interval>::iterator itr;
    for (itr = a.begin(); itr != a.end(); itr++)
    {
        interval cc = *itr;
        cout << cc.second << "\t" << cc.first << " z" << endl;;
    }
}

// given a file name (in abvr form stored in Input), returns the number of intervals stored in the file. 
int get_no_intervals(string fileName)
{
    int number_of_lines = 0;
    std::string line;
    std::ifstream myfile(fileName);

    while (std::getline(myfile, line))
        ++number_of_lines;
    number_of_lines -= 10;
    return number_of_lines;
}

// read intervals from fileName and 
void scheduling_experiment(string fileName, int nSteps, double fpRatio, double fnRatio)
{
    ifstream* inputFile = new ifstream(fileName);
    if (inputFile->fail())
    {
        cout << "Error: cannot open " << fileName << "\n";
        return;
    }

    ofstream resFile;
    string str = "Results/" + fileName.substr(6, fileName.length() - 10);
    str += "FP" + to_string((int)fpRatio*100) + "FN" + to_string((int)fnRatio*100);
    resFile.open(str + "-res.txt");
    if (resFile.fail())
    {
        cout << "Error: cannot open " << str << "\n";
        return;
    }
    int n = get_no_intervals(fileName)/2;
    interval* I = new interval[n]; // input sequence
    interval* IHat = new interval[n]; //used for forming prediction sequence
    interval* buffer = new interval[n]; //used for forming prediction sequence
    interval* Icopy = new interval[n];

    IntervalSchedulingAlgorithms algLibrary;

    resFile << "dist\t eta-scaled\tOpt\tTrust\tGreedy\tTrustGreedy\t eta-unslaced\n";
    
    interval* fileSeq = new interval[2*n]; // all intervals in the input file
    readKIntervals(inputFile, fileSeq, 2*n);
    random_shuffle(&fileSeq[0], &fileSeq[2*n]); // shuffle the input file content
    
    for (int i = 0; i < n; i++)
        I[i] = fileSeq[i];
    
    for (int i = 0; i < n; i++)
        buffer[i] = fileSeq[i + n];

    //readKIntervals(inputFile, I, n);
    //readKIntervals(inputFile, buffer, n);
//    copyIntervals(Icopy, I, 0, n);
    for (int step = 0; step <= nSteps; step++)
    {
        cout << "\33[2K \r  step " << step << " out of " << nSteps; // << endl;
        copyIntervals(Icopy, I, 0, n);

        // shuffle Icopy
        random_shuffle(&Icopy[0], &Icopy[n]);

        // at step i, remove i intervals from the the input and add i intervals to form the prediction
        int dist = step * ((double)n / nSteps);

        // initiate prefix of IHat to be the same as I 
        // the n - dist copied intervals will be true positives; the dist none-copied intervals will be false negatives
        int fn = fnRatio* dist;
        copyIntervals(IHat, Icopy, 0, n - fn);
        
        // complete IHat by reading its suffix from the input file; the dist copied intervals will be false positives
        int fp = fpRatio * dist;
        copyIntervals(&IHat[n - dist], buffer, 0, fp);

        set<interval> errorSet(&Icopy[n - fn], &Icopy[n]); // initiate with false negatives
        for (int i = 0; i < fp; i++)
            errorSet.insert(IHat[n - fp + i]); // false positives
        set<interval> errorOpt = algLibrary.getOpt(errorSet); // used for computing eta
        set<interval> inputSet(&I[0], &I[n]);
        set<interval> optI = algLibrary.getOpt(inputSet); // used as a baseline for scaling error

        set<interval> hatSet(&IHat[0], &IHat[n-fn+fp]); // the optimal solution for prediction; used in Trust, TrustGreedy
        set<interval> optHat = algLibrary.getOpt(hatSet);

        double eta_scaled = (double)errorOpt.size() / (double)optI.size();

        int trustPayoff = algLibrary.Trust(I, n, optHat);
        int greedyPayoff = algLibrary.TrustGreedy(I, n, set<interval>(), 0); // greedy is indeed GreedyTrust with an empty prediction
        int trustGreedyPayoff = algLibrary.TrustGreedy(I, n, optHat, 0);

        resFile << dist <<  "\t" << double((int)(eta_scaled * 1000)) / 1000 << "\t";
        resFile << optI.size() << "\t" << trustPayoff << "\t" << greedyPayoff << "\t" << trustGreedyPayoff << "\t";
        resFile << (double)errorOpt.size() << endl;
    }
    cout << endl;
    inputFile->close();
    resFile.close();
     
    delete[] fileSeq;
    delete[] I;
    delete[] IHat;
    delete[] Icopy;
    delete[] buffer;
}

int main()
{
    cout << "welcome to interval scheduling with predictions\n";
    int mode = 1;
    while (mode == 1 || mode == 0)
    {
        cout << "\n\nenter the program mode: \n \t 0 for generating new input files \n \t 1 for running algorithms on existing files\n\t any other number to quit \n";
        cin >> mode;
        if (mode == 0)
        {
            cout << "we are at input generation mode. \nI assume the desired raw files in swf format are listed at Benchmark/directory.txt";
            process_files();
            cout << "\nfile generation is complete";
        }
        else if (mode == 1)
        {
            cout << "testing algorithms. \nI assume the desired input files format are listed at Input/directory.txt\n";
            cout << "enter the number of steps (differe error values):\n";
            int nSteps; 
            cin >> nSteps;
            double fpRatio, fnRatio;
            cout << "enter the false-positive weight (0.00 for no fp, 1.00 for max fp):\n";
            cin >> fpRatio;
            cout << "enter the false-negative weight (0.00 for no fn, 1.00 for max fn):\n";
            cin >> fnRatio;

            string directory = "Input/directory.txt";
            ifstream* allFiles = new ifstream(directory);
            if (allFiles->fail())
            {
                cout << "Error: the file Input/directory.txt is missing\n";
                return 0;
            }
            cout << "\n\nopened " << directory << "\n";
            string line;
            while (getline(*allFiles, line))
            {
	      line = "Input/" + line;
	      if (line.size()>=1)
	      {
	        line.pop_back();
                cout << "processing " << line << "\n";
                scheduling_experiment(line, nSteps, fpRatio, fnRatio);
	      }
            }
        }
    }
    return 0;
}

