#ifndef USER_SERVER_H
#define USER_SERVER_H
#include <string>
#include <thread>
#include <unistd.h>
using namespace std;

class User_server
{
public:
    User_server(int _id, int _client_socket);
    ~User_server();
    
    int id;
    thread* client_thread;
    int client_socket;
    string name;
};

#endif