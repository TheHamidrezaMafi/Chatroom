#include "user.hpp"

User::User(string _name, string _password, User_server* _user_server):
user_server(_user_server), name(_name), password(_password),changeble_name(_name)
{
}