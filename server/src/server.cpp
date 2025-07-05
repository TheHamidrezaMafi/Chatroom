#include "server.hpp"

Server::Server()
{
    unique_id = 0;
    MAX_LEN = 150;
    user_file = new User_file("tables/users.txt", " #");
    g_file = new G_file("tables/groups.txt","tables/groups_users.txt", " #");
    messages_file = new Messages_file("tables/gps_msg.txt","tables/pvs_msg.txt","tables/buffer.txt","tables/blocked.txt", " #",user_file);
    clients.clear();
}

void Server::start_listening()
{
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(-1);
	}

    struct sockaddr_in server;
    server.sin_family = AF_INET;
	server.sin_port = htons(10000);
	server.sin_addr.s_addr = INADDR_ANY;
    if ((bind(server_socket, (struct sockaddr *)&server, sizeof(struct sockaddr_in))) == -1)
	{
		perror("bind");
		exit(-1);
	}

    if ((listen(server_socket, 5)) == -1)
	{
		perror("listen");
		exit(-1);
	}
    multi_print("... Chat Room start listening ...");   
}

void Server::start_accepting()
{
    struct sockaddr_in client;
    int client_socket;
    unsigned int len = sizeof(sockaddr_in);

    while (true)
    {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client, &len)) == -1)
        {
            perror("accept: ");
			exit(-1);
        } 
        unique_id++;
        User_server* user_server = new User_server(unique_id, client_socket);
        thread* client_thread = new thread(handle_client, this, user_server);
        user_server->client_thread = client_thread;
        add_client(user_server);
    }
}

void Server::add_client(User_server* user_server)
{
    lock_guard<mutex> guard(clients_mtx);
	clients[user_server->id] = user_server;
}

void Server::end_connection(int id)
{
    lock_guard<mutex> guard(clients_mtx);
    if (clients[id])
        delete clients[id];
}

void Server::close_connection()
{
    close(server_socket);
}

void Server::multi_print(string str)
{	
	lock_guard<mutex> guard(print_mtx);
	cout<<str<<endl;
}

void Server::send_message(int client_socket, string message)
{
    send(client_socket, &message[0], MAX_LEN, 0);
}

bool Server::try_add_or_get_user(User_server* user_server, string name, string password,bool signin)
{
    if (users.find(name) != users.end())
    {
        if (users[name]->password == password){
            users[name]->user_server = user_server;
            return true;
        }
        else 
            return false;
    }
    if (signin)
    return false;
    else
    {
        users[name] = new User(name, password, user_server);
        user_file->add_user(users[name]);
    }
    return true;
}

bool Server::login_client(User_server* user_server)
{
    while(true)
    {
        char name[MAX_LEN], password[MAX_LEN],login_typ[MAX_LEN];
        int bytes_received = recv(user_server->client_socket, login_typ, sizeof(login_typ), 0);
        if(bytes_received <= 0)
            return false;
        bytes_received = recv(user_server->client_socket, name, sizeof(name), 0);
        if(bytes_received <= 0)
            return false;
        bytes_received = recv(user_server->client_socket, password, sizeof(password), 0);
        if(bytes_received <= 0)
            return false;
        bool signin=false;
        if(string(login_typ)=="signin"){
        signin = true;
        }
        if (!try_add_or_get_user(user_server, name, password,signin))
        {
            string error_message = "Server | Invalid credentials for user " + string(name);
            send_message(user_server->client_socket, error_message);
            multi_print(error_message);
            continue;
        }
        user_server->name = name;
        string welcome_message = "Server | welcome " + string(name);
        send_message(user_server->client_socket, welcome_message);
        send_buffer(users[name]);
        multi_print(welcome_message);
        return true;
    }
}
void Server::send_buffer(User* user){
    vector<string>* buffer = messages_file->get_buffer(user);
    for (int i = 0 ; i < buffer->size();i++){
        send_message(user->user_server->client_socket,buffer->at(i));
    }
}

void Server::handle_client(Server* server, User_server* user_server)
{
    if (server->login_client(user_server))
    {
        char message[server->MAX_LEN];
        int bytes_received;
        while(true)
        { 
            bytes_received = recv(user_server->client_socket, message, sizeof(message), 0);
            if(bytes_received <= 0)
                break; 
            server->multi_print(string(user_server->name) + " : " + string(message));		
            server->do_for_user(server->users[user_server->name], message);
        }
        server->users[user_server->name]->user_server = 0;
    }
    server->end_connection(user_server->id);
}

void Server::send_pv(User_server* sender, User_server* client, string message,Time_stamp* curr)
{
    send_message(client->client_socket, "PV | " + user_file->get_name(sender->name)+ " : " + message +"   "+curr->time_s);
    send_message(sender->client_socket, "Server | You To " + user_file->get_name(client->name)+ " : "+message + "   "+curr->time_s);
}
void Server::send_gp(User_server* sender, Group* gp, string message,Time_stamp* curr)
{
    vector<string>* users_gp = g_file->users_in_group(gp);
    for(int i = 0 ; i < users_gp->size(); i++){
        if(users[users_gp->at(i)]->user_server){
            send_message(users[users_gp->at(i)]->user_server->client_socket,"gp | "+gp->name+" | "+user_file->get_name(sender->name)+" : "+message+"   "+curr->time_s);
        }
        else{
            messages_file->add_buffer(users[users_gp->at(i)]->name,"gp | "+gp->name+" | "+user_file->get_name(sender->name)+" : "+message+"   "+curr->time_s);
        }
    }
}

bool Server::check_user(string name, bool connected)
{
    //get_users_from_file();
    if (users.find(name) == users.end())
        throw "This user doesn't exist";
    if (connected && !users[name]->user_server)
        return false;
        //throw "This user isn't connected";
    return true ;
}
void Server::check_group(string name)
{
    get_groups_from_file();
    if (groups.find(name) == groups.end())
        throw "This group doesn't exist";
    
}

void Server::do_for_user(User* user, string message)
{   

    vector<string>* message_splitted = 0;
    try
    {
        message_splitted = split_message(message, " #");
        check_message_size(message_splitted);
        if (message_splitted->at(0) == "pv")
        {
            check_message_size(message_splitted, 3);
            if (message_splitted->at(2)=="history"){
                vector<string>* history =messages_file->get_pv_history(user,message_splitted->at(1));
                for(int i = 0 ; i < history->size();i++)
                    send_message(user->user_server->client_socket,history->at(i));
            }
            else{
            if(messages_file->is_blocked(user->name,message_splitted->at(1))){
            if(check_user(message_splitted->at(1), true)){
            Time_stamp* curr = new Time_stamp();
            send_pv(user->user_server, users[message_splitted->at(1)]->user_server, message_splitted->at(2),curr);
            messages_file->add_pvs(user,users[message_splitted->at(1)],curr,message_splitted->at(2));
            delete curr;
            }
            else{
            Time_stamp* curr = new Time_stamp();
            messages_file->add_buffer(users[message_splitted->at(1)]->name,"PV | "+user_file->get_name(user->name)+" -> you : " + message_splitted->at(2) +"   "+curr->time_s);
            send_message(user->user_server->client_socket, "message saved in buffer");
            messages_file->add_pvs(user,users[message_splitted->at(1)],curr,message_splitted->at(2));
            delete curr;
            }
            }
            else{
                throw "This user is blocked";
            }
            }
        }else if(message_splitted->at(0) == "changename"){
            user_file->change_name(user,message_splitted->at(1),user_file);
            multi_print("user "+user->name +" name changed to " +user_file->get_name(user->name) );
            send_message(user->user_server->client_socket, "name chaged succusfully");
        }else if(message_splitted->at(0) == "block"){
            check_user(message_splitted->at(1), true);
            messages_file->add_blocked(user->name,message_splitted->at(1));
            multi_print("user "+user->name +" blocked " +message_splitted->at(1) );
            send_message(user->user_server->client_socket, "blocked " + message_splitted->at(1));
        }else if(message_splitted->at(0) == "unblock"){
            check_user(message_splitted->at(1), true);
            messages_file->un_blocked(user->name,message_splitted->at(1));
            multi_print("user "+user->name +" unblocked " +message_splitted->at(1) );
            send_message(user->user_server->client_socket, "unblocked " + message_splitted->at(1));
        }else if(message_splitted->at(0) == "showblocked"){
            multi_print("user "+user->name +" asked for blocked");
            vector<string>* blocked = messages_file->get_blocked(user->name);
            for (int i =0 ;i < blocked->size();i++){
            send_message(user->user_server->client_socket,to_string(i)+". "+blocked->at(i));
            }
        }else if(message_splitted->at(0) == "showgroups"){
            get_groups_from_file();
            multi_print("user "+user->name +" asked for groups");
            vector<string>* gps = g_file->get_user_groups(user);
            for (int i =0 ;i < gps->size();i++){
            vector<string>* users_gp = g_file->users_in_group(groups[gps->at(i)]);
            string strusers;
            for (int i = 0 ; i < users_gp->size();i++)
                strusers=strusers+" - "+user_file->get_name(users_gp->at(i));
            send_message(user->user_server->client_socket,to_string(i)+". "+groups[gps->at(i)]->name+"    info : "+groups[gps->at(i)]->desc+ "    members : "+to_string(users_gp->size())+"\n"+strusers);
            }
        }else if(message_splitted->at(0) == "friends"){
            multi_print("user "+user->name +" asked for friends");
            vector<string>* friends = messages_file->get_friends(user);
            for (int i =0 ;i < friends->size();i++){
            send_message(user->user_server->client_socket,to_string(i)+". "+user_file->get_name(friends->at(i)));
            }
        }else if(message_splitted->at(0) == "gp"){
            if (message_splitted->at(1) == "create"){
                check_message_size(message_splitted, 4);
                g_file->add_g(new Group(message_splitted->at(2),message_splitted->at(3)));
                send_message(user->user_server->client_socket, "group '"+ message_splitted->at(2)+"' created succusfully");
            }
            else if (message_splitted->at(1) == "join"){
                check_message_size(message_splitted, 3);
                check_group(message_splitted->at(2));
                g_file->add_user_to_gp(groups[message_splitted->at(2)],user);
                send_message(user->user_server->client_socket, "you joined group '"+ message_splitted->at(2)+"' succusfully");
                Time_stamp* curr = new Time_stamp();
                send_gp(user->user_server, groups[message_splitted->at(2)] , user_file->get_name(user->name) +" joind!",curr);
            }
            else{
                check_message_size(message_splitted, 3);
                if(message_splitted->at(2)=="history"){
                    vector<string>* history = messages_file->get_gp_history(groups[message_splitted->at(1)]);
                    for(int i = 0 ; i < history->size();i++)
                    send_message(user->user_server->client_socket,history->at(i));

                }
                else{
                check_group(message_splitted->at(1));
                Time_stamp* curr = new Time_stamp();
                send_gp(user->user_server, groups[message_splitted->at(1)] ,message_splitted->at(2),curr);
                messages_file->add_gps(groups[message_splitted->at(1)] ,user,curr,message_splitted->at(2));
                delete curr;
                }
            }
        }else
        throw "The command is not executable";
    }
    catch(const char* msg)
    {   
        send_message(user->user_server->client_socket, "Server | There is a problem ( " + string(msg) + " )");
    }
    if (message_splitted)
    {
        message_splitted->clear();
        delete message_splitted;
    }
}

void Server::get_users_from_file()
{
    vector<User*>* users_db = user_file->get_users();
    for (auto &user:*users_db)
        users[user->name] = user;
}

void Server::get_groups_from_file()
{
    vector<Group*>* groups_db = g_file->get_groups();
    for (auto &group:*groups_db)
        groups[group->name] = group;
}

void Server::delete_users()
{
    for (auto & u : clients)
    {
        delete u.second;
    }
    clients.clear();
    for (auto & u : users)
    {
        delete u.second;
    }
    users.clear();
}

Server::~Server()
{
    delete user_file;
    delete_users();
    close_connection();
}