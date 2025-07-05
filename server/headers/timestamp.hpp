#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include "iostream"
#include "user.hpp"
#include <mutex>
#include "helper.hpp"
#include <vector>
#include <fstream>
#include "group.hpp"
#include <time.h>
using namespace std;

class Time_stamp
{
public:
    Time_stamp(string _time);
    bool operator == (Time_stamp sec);
    bool operator < (Time_stamp sec);
    bool operator > (Time_stamp sec);
    Time_stamp();
    string time_s;
    int s , m , h ;
};

#endif