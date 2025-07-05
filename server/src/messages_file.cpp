#include "messages_file.hpp"

Messages_file::Messages_file(string _addr_gps,string _addr_pvs,string _addr_buffer,string _addr_blocked, string _wrapper,User_file* _user_file):
addr_gps(_addr_gps),addr_pvs(_addr_pvs),addr_blocked(_addr_blocked),addr_buffer(_addr_buffer),wrapper(_wrapper),user_file(_user_file)
{

}

void Messages_file::add_gps(Group* group,User* user,Time_stamp* timestamp,string msg){
    lock_guard<mutex> guard(write_mtx_gps);
    file.open(addr_gps, ios::out | ios::app);
    file<<group->name<<wrapper<<user->name<<wrapper<<timestamp->time_s<<wrapper<<msg<<"\n";
    file.close();
}

void Messages_file::add_pvs(User* user1,User* user2,Time_stamp* timestamp,string msg){
    lock_guard<mutex> guard(write_mtx_pvs);
    file.open(addr_pvs, ios::out | ios::app);
    file<<user1->name<<wrapper<<user2->name<<wrapper<<timestamp->time_s<<wrapper<<msg<<"\n";
    file.close();
}

vector<string>* Messages_file::get_gp_history(Group* group){
    lock_guard<mutex> guard(write_mtx_gps);
    file.open(addr_gps, ios::in);
    string line;
    vector<string>* messages;
    vector<string>* histroy = new vector<string>();
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(messages->at(0)==group->name)
        histroy->push_back("gp | "+messages->at(0)+" | "+user_file->get_name(messages->at(1))+" : "+messages->at(3)+"   "+messages->at(2));
    }  
    file.close();
    return histroy;
}
vector<string>* Messages_file::get_pv_history(User* user1, string  user2){
    lock_guard<mutex> guard(write_mtx_pvs);
    file.open(addr_pvs, ios::in);
    string line;
    vector<string>* messages;
    vector<string>* histroy = new vector<string>();
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(messages->at(0)==user1->name && messages->at(1)==user2)
        histroy->push_back("PV | you -> "+user_file->get_name(messages->at(1))+" : " + messages->at(3) +"   "+messages->at(2));
        if(messages->at(1)==user1->name && messages->at(0)==user2)
        histroy->push_back("PV | "+user_file->get_name(messages->at(0))+" -> you : " + messages->at(3) +"   "+messages->at(2));
        
    }  
    file.close();
    return histroy;
}
vector<string>* Messages_file::get_friends(User* user){
    lock_guard<mutex> guard(write_mtx_pvs);
    file.open(addr_pvs, ios::in);
    string line;
    vector<string>* messages;
    vector<string>* friends = new vector<string>();
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(messages->at(0)==user->name){
            bool flag = true;
            for(int i = 0 ; i < friends->size(); i++)
                if (friends->at(i) == messages->at(1)){
                    flag = false;
                    break;
                }
            if(flag)
            friends->push_back(messages->at(1));
        }
        
        else if(messages->at(1)==user->name){
            bool flag = true;
            for(int i = 0 ; i < friends->size(); i++)
                if (friends->at(i) == messages->at(0)){
                    flag = false;
                    break;
                }
            if(flag)
            friends->push_back(messages->at(0));
        }
    }  
    file.close();
    return friends;
}
void Messages_file::add_buffer(string reciver , string msg){
    lock_guard<mutex> guard(write_mtx_buffer);
    file.open(addr_buffer, ios::out | ios::app);
    file<<reciver <<wrapper<<msg<<"\n";
    file.close();
}
vector<string>* Messages_file::get_buffer(User* user){
    lock_guard<mutex> guard(write_mtx_buffer);
    file.open(addr_buffer, ios::in);
    string line;
    vector<string>* messages;
    vector<string>* buffer = new vector<string>();
    string tmp;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(messages->at(0)==user->name){
            buffer->push_back(messages->at(1));
            continue;
        }
        tmp = tmp + line +"\n";
    }  
    file.close();
    file.open(addr_buffer, ios::out);
    file<<tmp;
    file.close();
    return buffer;
}
void Messages_file::add_blocked(string user1 , string user2){
    lock_guard<mutex> guard(write_mtx_blocked);
    file.open(addr_buffer, ios::out | ios::app);
    file<<user1<<wrapper<<user2<<"\n";
    file.close();
}
vector<string>* Messages_file::get_blocked(string user){
    lock_guard<mutex> guard(write_mtx_buffer);
    file.open(addr_buffer, ios::in);
    string line;
    vector<string>* messages;
    vector<string>* blocked = new vector<string>();
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(messages->at(0)==user){
            blocked->push_back(messages->at(1));
            continue;
        }
        
    }  
    file.close();
    return blocked;
}
void Messages_file::un_blocked(string user1 , string user2){
    lock_guard<mutex> guard(write_mtx_buffer);
    file.open(addr_buffer, ios::in);
    string line;
    vector<string>* messages;
    vector<string>* buffer = new vector<string>();
    string tmp;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if(messages->at(0)==user1 && messages->at(1)==user2){
            continue;
        }
        tmp = tmp + line +"\n";
    }  
    file.close();
    file.open(addr_buffer, ios::out);
    file<<tmp;
    file.close();
}
bool Messages_file::is_blocked(string user1,string user2){
    lock_guard<mutex> guard(write_mtx_buffer);
    file.open(addr_buffer, ios::in);
    string line;
    vector<string>* messages;
    bool flag = true;
    while(!file.eof())
    {
        getline(file,line);
        if (!line.length()) 
            continue;
        messages = split_message(line, wrapper, 0);
        if((messages->at(0)==user1 && messages->at(1)==user2)||(messages->at(1)==user1 && messages->at(0)==user2) ){
            flag = false;
            break;
        }
        
    }  
    file.close();
    return flag;
}