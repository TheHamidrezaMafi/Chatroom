#ifndef CLIENT_H
#define CLIENT_H

#include "iostream"
#include <sys/types.h>
#include <sys/socket.h>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <netinet/in.h>
#include <cstring>
using namespace std;

class Client
{
public:
    Client();
    void start_connecting();
    void start_communicating();
    void close_connection();
    void login();
    void check_equal(char* ch,string str);
    static void send_handler(Client* client);
    static void recv_handler(Client* client);
    void multi_print(string message, bool you=true);
    ~Client();

    thread* send_thread;
    thread* recv_thread;
    int MAX_LEN;
    bool logged_in;
    bool exited;
    char* name;
    char* password;
    char* password2;
    mutex print_mtx,cin_mtx;
    int client_socket;
};

#endif