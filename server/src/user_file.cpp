#include "user_file.hpp"

User_file::User_file(string _addr, string _wrapper):
addr(_addr), wrapper(_wrapper)
{
}

void User_file::add_user(User* user)
{
    lock_guard<mutex> guard(write_mtx);
    file.open(addr, ios::out |ios::app);
    file<<user->name<<wrapper<<user->password<<wrapper<<user->changeble_name<<"\n";
    file.close();
}
void User_file::change_name(User* user,string newname,User_file* ptr){
    lock_guard<mutex> guard(write_mtx);
    file.open(addr, ios::in);
    vector<string>* messages;
    string line;
    string tmp;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if (messages->at(0)==user->name)
        continue;
        tmp = tmp+line+"\n";

    }
    file.close();
    file.open(addr, ios::out);
    file<<tmp;
    file.close();
    user->changeble_name=newname;
    file.open(addr, ios::out |ios::app);
    file<<user->name<<wrapper<<user->password<<wrapper<<user->changeble_name<<"\n";
    file.close();
}

string User_file::get_name(string _name){
lock_guard<mutex> guard(write_mtx);
    file.open(addr, ios::in);
    vector<string>* messages;
    string line;
    string out;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if (messages->at(0)==_name)
            out= messages->at(2);

    }
    file.close();
    return out+"("+_name+")";
}
vector<User*>* User_file::get_users()
{
    vector<User*>* users = new vector<User*>();
    lock_guard<mutex> guard(write_mtx);
    file.open(addr, ios::in);
    string line;
    vector<string>* messages;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        users->push_back(new User(messages->at(0), messages->at(1)));
    }  
    file.close();
    return users;
}