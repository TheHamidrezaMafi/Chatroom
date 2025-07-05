#ifndef G_FILE_H
#define G_FILE_H

#include "iostream"
#include "user.hpp"
#include <mutex>
#include "helper.hpp"
#include <vector>
#include <fstream>
#include "group.hpp"

using namespace std;

class G_file
{
public:
    G_file(string _addr_groups,string _addr_users, string _wrapper);
    string addr_groups,addr_users;
    vector<string>* get_user_groups(User* user);
    fstream file;
    string wrapper;
    void add_g(Group* group);
    void add_user_to_gp(Group* group,User* user);
    vector<string>* users_in_group(Group* group);
    vector<Group*>* get_groups();
    mutex write_mtx_groups,write_mtx_users;
};

#endif