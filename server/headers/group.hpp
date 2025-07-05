#ifndef GROUP_H
#define GROUP_H

#include "user_server.hpp"
#include <map>

using namespace std;

class Group
{
public:
    Group(string name, string desc);
    
    int usernumber;
    string name;
    string desc;
};

#endif