/******************************************************************************************
*******************************************************************************************
********************** Online Interval Scheduling with Predictions ************************
*******************************************************************************************
*******************************************************************************************/


// IntervalSchedulingAlgorithms.cpp : code for scheduling algorithms like TRUST, TRUSTGREEDY, and OPT.
//

#include <random>
#include <algorithm>
#include <climits>

#include "IntervalSchedulingAlgorithms.h"

// given a set of intervals, return those that form an optimal solution 
set<interval> IntervalSchedulingAlgorithms::getOpt(set<interval> a)
{
    set<interval>::iterator itr;

    int cutPoint = 0; // we can only accept intervals who start at or after this point
    set<interval> opt;

    // iterate over all intervals (sorted by their ending time)
    for (itr = a.begin(); itr != a.end(); itr++)
    {
        interval nextInterval = *itr;
        int start = nextInterval.second; //the start of the next interval
        int end = nextInterval.first; //the end of the next interval

        if (start >= cutPoint) //we are edge-disjoint, equalities are OK
        {
            // accepting nextInterval greedily
            opt.insert(nextInterval);
            cutPoint = end;
        }
    }
    return opt;
}



/* Given a sorted vector v formed by sorted endpoints of a set of intervals that form a feasible solution
(i.e., v[2k, 2k+1] are endpoints of one interval), and a "newInterval" output the following:
 - if newInterval intersects two or more intervals in v, then return -1
 - if newInterval does not intersect any interval in v, then return -2
 - otherwise, return the beginning index of an interval in v that newInterval may replace
 */
int IntervalSchedulingAlgorithms::vectorIntersect(vector<int> v, interval newInterval, bool track)
{
    // add padding indices at the beginning and end of v to avoid edge cases
    v.insert(v.begin(), 2, 0); 
    v.insert(v.end(), 2, INT_MAX);

    std::vector<int>::iterator it;
    if (track)
    {
        cout << "the vector is: \n";
        for (it = v.begin(); it < v.end(); it++)
            std::cout << ' ' << *it;
        cout << "\n_____________" << endl;
    }

    double eps = 0.1;
    int start = newInterval.second;
    int end = newInterval.first;
    if (track)
    {
        cout << "start , end\t" << start << "\t" << end << endl;
        cout << "_____________" << endl;
    }
    auto S = lower_bound(v.begin(), v.end(), start + eps) - v.begin() - 1; // value at index S is <= start

    auto U = lower_bound(v.begin(), v.end(), end - eps) - v.begin(); // value at index U is >=end
    if (track)
    {
        cout << "S, V[S]: " << S << "\t" << v[S] << endl;
        cout << "U, V[U]: " << U << "\t" << v[U] << endl;
        cout << "_____________" << endl;
    }

    int res = -3; // -3 indicates the result is not set yet
    int d = U - S;
    if (track)
        cout << "d is: " << d << endl;
    if (d == 1) // 0 intervals between S and U
    {
        if (S % 2 == 1)
        {
            if (track)
                cout << "new interval is not intersecting any one\n";
            res = -2;;
        }
        else
        {
            if (track)
                cout << "new interval is entirely inside " << v[S] << "," << v[U] << endl;
            res = S - 2; // -2 is due to the first dummy (0's) added at the beginning
        }
    }
    else
        if (d == 2)
        {
            if (S % 2 == 1)
            {
                if (track)
                    cout << "new interval intersects (and may contain) a " << v[S + 1] << "," << v[S + 2] << endl;
                res = S + 1 - 2;
            }
            else
            {
                if (track)
                    cout << "new interval intersects (and may contain) b " << v[S] << "," << v[S + 1] << endl;
                res = S - 2;
            }
        }
        else
            if (d == 3) // 2 intervals between S and U
            {
                if (S % 2 == 1)
                {
                    if (track)
                        cout << "new interval entirely contains " << v[S + 1] << "," << v[S + 2] << endl;
                    res = S + 1 - 2;
                }
                else
                {
                    if (track)
                        cout << "new intervan intersects both " << v[S] << "," << v[S + 1] << " and" << v[S + 2] << "," << v[S + 3] << endl;
                    res = -1;
                }
            }
            else
                res = -1; // intersecting more than two intervals

    return res;
}




/* given a set I of n intervals, and a prediction about intervals (optHat),
return the payoff of Trust */
int IntervalSchedulingAlgorithms::Trust(interval* I, int n, set<interval> optHat)
{
    int payOff = 0;
    vector<int> redd{}; //  endpoints of accepted intervals

    for (int i = 0; i < n; i++)
    {
        interval cc = I[i];
        int iRed = vectorIntersect(redd, cc, 0); // does the new interval intersect one of the previously accepted intervals?
        if (iRed == -2) //only continue if cc does not intersect any interval in red (any accepted interval)
        {
            auto contains = optHat.find(cc);
            if (contains != optHat.end())
            {
                redd.insert(std::upper_bound(redd.begin(), redd.end(), cc.first), cc.first);
                redd.insert(std::upper_bound(redd.begin(), redd.end(), cc.second), cc.second);
                payOff++;
            }
        }
    }
    return payOff;
}


/* given a set I of n intervals, and a prediction about intervals (optHat),
return the payoff of TrustGreedy; track is a boolearn for reporting details.*/
int IntervalSchedulingAlgorithms::TrustGreedy(interval* I, int n, set<interval> optHat, bool track)
{
    IntervalSchedulingAlgorithms algLibrary;

    int payOff = 0;
    vector<int> redd{}; //  endpoints of intervals in the planned opt that have been accepted; initially empty
    vector<int> white{}; //  endpoints of intervals in the planned opt that has been planned but not arrived yet; initially all opt
    set<interval>::iterator itr;
    for (itr = optHat.begin(); itr != optHat.end(); itr++)
    {
        interval cc = *itr;
        white.push_back(cc.second);
        white.push_back(cc.first);
    }

    for (int i = 0; i < n; i++)
    {
        if (track)
        {
            cout << "\nwhite is: ";
            for (auto j : white)
                std::cout << j << " ";
            cout << endl;
            cout << "red is: ";
            for (auto j : redd)
                std::cout << j << " ";
            cout << endl;
        }

        interval cc = I[i];

        if (cc.first == cc.second)
        {
            // accept an interval of length 0.
            payOff++;
            continue;
        }
        else {
            if (track)
                cout << "cc is: " << cc.second << "," << cc.first << endl;
            int iRed = algLibrary.vectorIntersect(redd, cc, 0); // does the new interval intersect one of the previously accepted intervals?
            if (track)
                cout << "intersect red is: " << iRed << endl;
            if (iRed == -2) //only continue if cc does not intersect any interval in red (any accepted interval)
            {
                int iWhite = algLibrary.vectorIntersect(white, cc, 0); // does the new interval intersect one of the previously accepted intervals?
                if (track)
                    cout << "intersect white is: " << iWhite << endl; // iWhite.second << "," << iWhite.first << endl;
                if (iWhite == -1)
                {
                    if (track)
                        cout << "intersects >= 2 in white; continue\n";
                }
                else
                {
                    payOff++;
                    // we intersect one or 0 in white; cc must be accepted
                    redd.insert(upper_bound(redd.begin(), redd.end(), cc.first), cc.first);
                    redd.insert(upper_bound(redd.begin(), redd.end(), cc.second), cc.second);

                    if (iWhite == -2)
                    {
                        if (track)
                            cout << "does not intersect white, add to redd\n";
                    }
                    else
                    {
                        // remove iwhite from white and add cc to redd
                        white.erase(white.begin() + iWhite, white.begin() + iWhite + 2);
                    }
                }

            }
            else
            {
                if (track)
                    cout << "intersects red; continue\n";
            }
        }

    }
    return payOff;
}
