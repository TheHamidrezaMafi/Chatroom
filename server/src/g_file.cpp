#include "g_file.hpp"

G_file::G_file(string _addr_groups,string _addr_users, string _wrapper):
addr_groups(_addr_groups),addr_users(_addr_users), wrapper(_wrapper)
{
}

void G_file::add_user_to_gp(Group* group,User* user){
    lock_guard<mutex> guard(write_mtx_users);
    file.open(addr_users, ios::out | ios::app);
    file<<user->name<<wrapper<<group->name<<"\n";
    file.close();
    group->usernumber++;
}
vector<string>* G_file::users_in_group(Group* group){
    lock_guard<mutex> guard(write_mtx_users);
    file.open(addr_users, ios::in);
    string line;
    vector<string>* messages;
    vector<string>* users = new vector<string>();
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(messages->at(1)==group->name)
        users->push_back(messages->at(0));
    }  
    file.close();
    return users;
}
void G_file::add_g(Group* group)
{
    lock_guard<mutex> guard(write_mtx_groups);
    file.open(addr_groups, ios::out| ios::app);
    file<<group->name<<wrapper<<group->desc<<"\n";
    file.close();
}

vector<Group*>* G_file::get_groups()
{
    vector<Group*>* groups = new vector<Group*>();
    lock_guard<mutex> guard(write_mtx_groups);
    file.open(addr_groups, ios::in);
    string line;
    vector<string>* messages;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        groups->push_back(new Group(messages->at(0), messages->at(1)));
    }  
    file.close();
    return groups;
}
vector<string>* G_file::get_user_groups(User* user){
    lock_guard<mutex> guard(write_mtx_users);
    vector<string>* gps = new vector<string>();
    file.open(addr_users, ios::in);
    string line;
    vector<string>* messages;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(user->name == messages->at(0))
        gps->push_back(messages->at(1));
    }  
    file.close();
    return gps;
}