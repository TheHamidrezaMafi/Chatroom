#ifndef USER_H
#define USER_H

#include "user_server.hpp"
#include <map>
using namespace std;

class User
{
public:
    User(string name, string password, User_server* _user_server=0);
    
    User_server* user_server;
    string name;
    string password;
    string changeble_name;
};

#endif