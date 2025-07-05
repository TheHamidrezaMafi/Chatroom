#ifndef MESSAGES_FILE_H
#define MESSAGES_FILE_H

#include "iostream"
#include "user.hpp"
#include <mutex>
#include "helper.hpp"
#include <vector>
#include <fstream>
#include "group.hpp"
#include "timestamp.hpp"
#include "user_file.hpp"

using namespace std;

class Messages_file
{
public:
    Messages_file(string _addr_gps,string _addr_pvs,string _addr_buffer,string _addr_blocked, string _wrapper,User_file* _user_file);
    string addr_gps,addr_pvs,addr_buffer,addr_blocked;
    fstream file;
    string wrapper;
    vector<string>* get_friends(User* user);
    void add_gps(Group* group,User* user,Time_stamp* timestamp,string msg);
    void add_pvs(User* user1,User* user2,Time_stamp* timestamp,string msg);
    void add_buffer(string reciver,string msg);
    void add_blocked(string user1 , string user2);
    void un_blocked(string user1 , string user2);
    bool is_blocked(string user1,string user2);
    vector<string>* get_blocked(string user);
    vector<string>* get_buffer(User* user);
    vector<string>* get_pv_history(User* user1, string user2);
    vector<string>* get_gp_history(Group* group);
    mutex write_mtx_gps,write_mtx_pvs,write_mtx_buffer,write_mtx_blocked;
    User_file* user_file;
};

#endif