#ifndef USER_FILE_H
#define USER_FILE_H

#include "iostream"
#include "user.hpp"
#include <mutex>
#include "helper.hpp"
#include <vector>
#include <fstream>


using namespace std;

class User_file
{
public:
    User_file(string _addr, string _wrapper);
    void change_name(User* user,string newname,User_file* ptr);
    string get_name(string _name);
    string addr;
    fstream file;
    string wrapper;
    void add_user(User* user);
    vector<User*>* get_users();
    mutex write_mtx;
};

#endif