#include "timestamp.hpp"

Time_stamp ::Time_stamp(string _time)
:time_s(_time)
{
vector<string>* messages;
messages = split_message(time_s,":",0);
h=stoi(messages->at(0));
m=stoi(messages->at(1));
s=stoi(messages->at(2));
}
Time_stamp ::Time_stamp()
{
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    h = timeinfo->tm_hour;
    m = timeinfo->tm_min;
    s = timeinfo->tm_sec;
    time_s = to_string(h)+":"+to_string(m)+":"+to_string(s);
}
bool Time_stamp::operator==(Time_stamp sec){
    if(this->h == sec.h && this->m == sec.m && this->s == sec.s )
    return true ;
    return false;
}
bool Time_stamp::operator>(Time_stamp sec){
    if(this->h > sec.h)
    return true ;
    if(this->h == sec.h && this->m > sec.m )
    return true ;
    if(this->h == sec.h && this->m == sec.m && this->s > sec.s)
    return true;
    return false;
}
bool Time_stamp::operator<(Time_stamp sec){
    if(this->h < sec.h)
    return true ;
    if(this->h == sec.h && this->m < sec.m )
    return true ;
    if(this->h == sec.h && this->m == sec.m && this->s < sec.s)
    return true;
    return false;
}