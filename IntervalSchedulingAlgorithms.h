/******************************************************************************************
*******************************************************************************************
********************** Online Interval Scheduling with Predictions ************************
*******************************************************************************************
*******************************************************************************************/


// IntervalSchedulingAlgorithms.h : header for scheduling algorithms like TRUST, TRUSTGREEDY, and OPT.
//
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

using namespace std;
#include <set>
#include <sstream>

typedef pair<int, int> interval;

class IntervalSchedulingAlgorithms
{
private:

    /* Given an array v formed by sorted endpoints of a set of intervals that form a feasible solution
(i.e., v[2k, 2k+1] are endpoints of one interval), and a "newInterval" output the following:
 - if newInterval intersects two or more intervals in v, then return -1
 - if newInterval does not intersect any interval in v, then return -2
 - otherwise, return the beginning index of an interval in v that newInterval may replace
 */
    int vectorIntersect(vector<int> v, interval newInterval, bool track);

public:
    // given a set of intervals, return those that form an optimal solution 
    set<interval> getOpt(set<interval> a);

    /* given a set I of n intervals, and a prediction about intervals that will appear(optHat),
    return the payoff of Trust */
    int Trust(interval* I, int n, set<interval> optHat);


    /* given a set I of n intervals, and a prediction about intervals (optHat),
    return the payoff of TrustGreedy; track is a boolearn for reporting details.*/
    int TrustGreedy(interval* I, int n, set<interval> optHat, bool track);
   

};
