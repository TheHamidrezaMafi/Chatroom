#ifndef SERVER_H
#define SERVER_h

#include "iostream"
#include <mutex>
#include "user_server.hpp"
#include <map>
#include "user_file.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include "helper.hpp"
#include "user.hpp"
#include "g_file.hpp"
#include "group.hpp"
#include "timestamp.hpp"
#include "messages_file.hpp"

using namespace std;

class Server
{
public:
    Server();
    void start_listening();
    void start_accepting();
    void close_connection();
    void end_connection(int id);
    static void handle_client(Server* server, User_server* user_server);
    void multi_print(string str);
    void add_client(User_server* client);
    void send_message(int socket, string message);
    void do_for_user(User* user, string message);
    bool try_add_or_get_user(User_server* user_server, string name, string password,bool signin);
    void send_pv(User_server* sender, User_server* client, string mes,Time_stamp* curr);
    void send_gp(User_server* sender, Group* gp, string mes,Time_stamp* curr);
    void send_buffer(User* user);
    bool login_client(User_server* user_server);
    bool check_user(string name, bool connected=false);
    void check_group(string name);
    void get_users_from_file();
    void get_groups_from_file();
    int get_online_clients_count();
    void delete_users();
    ~Server();

    int unique_id;
    int MAX_LEN;
    int server_socket;
    User_file* user_file;
    G_file* g_file;
    Messages_file* messages_file;
    mutex print_mtx, clients_mtx;
    map<string, User*> users;
    map<string,Group*> groups;
    map<int, User_server*> clients;
};

#endif