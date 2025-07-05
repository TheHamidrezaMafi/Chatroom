#include "client.hpp"

Client::Client()
{
    MAX_LEN = 150;
	logged_in = false;
    name = new char[MAX_LEN];
	exited = false;
	password = new char[MAX_LEN];
	password2 = new char[MAX_LEN];
}

void Client::start_connecting()
{
	if ((client_socket=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket: ");
		exit(-1);
	}

    struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_port=htons(10000);
	client.sin_addr.s_addr=INADDR_ANY;
    if ((connect(client_socket, (struct sockaddr *)&client, sizeof(struct sockaddr_in))) == -1)
	{
		perror("connect: ");
		exit(-1);
	}
}

void Client::login()
{
	while (true)
	{
		cout <<"1.sign in --- 2.sign up : ";
		int inp;
		cin>>inp;
		cin.ignore();
		if(inp == 1){
		send(client_socket, "signin", MAX_LEN, 0);
		cout<<"Enter your name : ";
		cin.getline(name, MAX_LEN);
		send(client_socket, name, MAX_LEN, 0);
		cout<<"Enter your password : ";
		cin.getline(password, MAX_LEN);
		send(client_socket, password, MAX_LEN, 0);
		multi_print("Your login request has been sent!", false);
		char answer[MAX_LEN];
		int bytes_received = recv(client_socket, answer, sizeof(answer), 0);
		if(bytes_received <= 0)
			continue;
		multi_print(answer, false);
		if (string(answer) == "Server | welcome " + string(name))
		 	break;
		}
		else if(inp == 2){
		cout<<"Enter your name : ";
		cin.getline(name, MAX_LEN);
		cout<<"Enter your password : ";
		cin.getline(password, MAX_LEN);
		cout<<"confirm your password : ";
		cin.getline(password2, MAX_LEN);
		if(string(password)!=string(password2)){
		cout << "passwords dont match"<<endl;
		continue;
		}
		send(client_socket, "signup", MAX_LEN, 0);
		send(client_socket, name, MAX_LEN, 0);
		send(client_socket, password, MAX_LEN, 0);
		char answer[MAX_LEN];
		int bytes_received = recv(client_socket, answer, sizeof(answer), 0);
		if(bytes_received <= 0)
			continue;
		multi_print(answer, false);
		if (string(answer) == "Server | welcome " + string(name))
		 	break;
		}

	}
}

void Client::start_communicating()
{
	login();
    send_thread = new thread(send_handler, this);
    recv_thread = new thread(recv_handler, this);	
	if (send_thread->joinable())
		send_thread->join();
	if (recv_thread->joinable())
		recv_thread->join();
	
}

void Client::multi_print(string message, bool you)
{
	lock_guard<mutex> guard(print_mtx);
	if (message.length())
		cout<<"\33[2K \r"<<message<<endl;
	if (you)
		cout<<"\33[2K \r"<<"You : ";
}
void Client::check_equal(char* ch,string str){
for(int i = 0 ; i < 150; i++){
	if(i < str.size())
	*(ch+i)=str[i];
	else
	*(ch+i)=0;
}
}
void Client::send_handler(Client* client)
{
	while(true)
	{

		client->multi_print("");
		char str[client->MAX_LEN];
		cin.getline(str,client->MAX_LEN);
		if (string(str) == "#exit")
		{
			client->exited = true;
			client->recv_thread->detach();
			close(client->client_socket);
			return;
		}
		else if (string(str) == "#menu")
		{
			client->multi_print("1. change name\n2. show groups\n3. friends\n4. blocked\n5. exit\nmenu : ",false);
			int menuinput;
			cin >> menuinput;
			cin.ignore();
			if(menuinput == 1){
				client->multi_print("Enter your new name: ",true);
				string nstr ;
				cin >>nstr;
				cin.ignore();
				nstr = ("#changename #" + nstr);
				send(client->client_socket, &nstr[0], client->MAX_LEN ,0);
			}
			else if(menuinput == 2){
				string nstr = "#showgroups";
				client->check_equal(str,nstr);
				send(client->client_socket,str, sizeof(str) ,0);
			}
			else if(menuinput == 3){
				send(client->client_socket, "#friends", client->MAX_LEN ,0);
			}
			else if(menuinput == 4){
				send(client->client_socket, "#showblocked", client->MAX_LEN ,0);
			}
			else if(menuinput == 5){
				client->exited = true;
				client->recv_thread->detach();
				close(client->client_socket);
				return;
			}
		}
		else
		send(client->client_socket, str, sizeof(str),0);
	}	
}

void Client::recv_handler(Client* client)
{
	while(!client->exited)
	{
		char message[client->MAX_LEN];
		int bytes_received = recv(client->client_socket, message, sizeof(message), 0);
		if(bytes_received <= 0)
			continue;
		client->multi_print(message);
		fflush(stdout);
	}  
}

void Client::close_connection()
{
	if (send_thread)
	{
		if (send_thread->joinable())
		{
			send_thread->detach();
			delete send_thread;
		}
		send_thread = 0;
	}
	if (recv_thread)
	{
		if (recv_thread->joinable())
		{
			recv_thread->detach();
			delete recv_thread;
		}
		recv_thread = 0;
	}
    close(client_socket);
	multi_print("=== The socket turned off ===", false);
}

Client::~Client()
{
	close_connection();
	delete [] name;
	delete [] password;
}
/*
#menu
#block #username
#unblock #username
#friends
#showgroups
#gp #create #gp name #info
#gp #gp name #history
#gp #join #gp name 
#gp #go name #message
#pv #username #message
#pv #username #history
#changename #newname
#showblocked



*/