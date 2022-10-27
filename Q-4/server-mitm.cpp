#include <bits/stdc++.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define MAXDATASIZE 1000 // max number of bytes we can get at once
#define MAXDATASIZE 1000 // max number of clients that can be conncected to client
#define PORT "3490"		 // the port users will be connecting to
#define BACKLOG 10		 // how many pending connections queue will hold
using namespace std;
typedef struct data1{
	int id;
	int destid;
	int sourcefd;
	string status;
	int destfd;
	string logFilename;
} data1;
vector<data1> v;
int c = 1;
long long int g, a;
string extractFromLastLine(char buf[])
{
	string temp;
	int i;
	for(i = strlen(buf) - 1; buf[i] != '\n'; i--){
		temp.push_back(buf[i]);
	}
	buf[i] = '\0';
	reverse(temp.begin(), temp.end());
	cout << temp;
	return temp;
}
void copy(string &s, char buf[], int choice)
{
	int i;
	if (choice == 0)
	{
		for (i = 0; buf[i] != '\0'; i++)
			s.push_back(buf[i]);
	}
	else
	{
		for (i = 0; s[i] != '\0'; i++)
			buf[i] = s[i];
		buf[i] = '\0';
	}
}
bool substring(char buf[], string s)
{
	for(int i = 0; i < s.size(); i++){
		if(buf[i] == '\0' || buf[i] != s[i]) return false;
	}
	return true;
}
void sendMessage(char buf[], int idx, int sockfd, int choice)
{
	if (send(sockfd, buf, strlen(buf), 0) == -1)
	{
        if((choice == 2 || choice == 3) && v[idx].destid != -1){
            v[v[idx].destid].status = "FREE";
            v[v[idx].destid].destid=-1;
			v[idx].destid = -1;
        } 
        if((choice == 1 || choice == 3) && v[idx].destid != -1) v[idx].status = "FREE";
		if(choice == 0) v[idx].destid = -1;
		v[idx].id = -1;
		perror("send");
		pthread_exit(NULL);
	}
}
void writeToLogFile(string s, int idx)
{
	cout << "D-97-logFile: " << v[idx].logFilename << "\n";
	fstream file1;
	file1.open(v[idx].logFilename, ios::out | ios::app);
	if(file1.is_open()){
		file1 << s << "\n";
	}
}
void createLogFile(int idx)
{
	static bool first = true, flag = false;
	fstream file;
	if(v[idx].logFilename == ""){
		string filename = "Log-" + to_string(min(idx, v[idx].destid)) + "-"+to_string(max(idx, v[idx].destid)) + ".txt";
		v[idx].logFilename = v[v[idx].destid].logFilename = filename;
		flag = true;
	}
	else first = false;
	if(first && flag){
		file.open(v[idx].logFilename, ios::out);
		file.close();
		first = false;
	}
	file.open(v[idx].logFilename, ios::out | ios::app);
	if(file.is_open()){
		file << "\n\n";
		file.close();
	}
}
void *solve(void *p)
{
	data1 *ptr = (data1 *)p;
	int idx = ptr->id;
	string s_idx = "Client-" + to_string(idx) + ":\n";
	char buf[MAXDATASIZE];
	string intro = "Server:\nHello Client\nEnter <list> to see the list of other clients connected to server" + to_string(g)+ " "+to_string(a);
	copy(intro, buf, 1);
	// strcpy(buf, "Server:\nHello Client\nEnter <list> to see the list of other clients connected to server");
	if (send(ptr->sourcefd, buf, strlen(buf), 0) == -1)
		perror("send");
	printf("%s\n", buf);
	int numbytes, c;
	string str;
    bool firstMessage = false;
	while (1)
	{
		numbytes = recv(v[idx].sourcefd, buf, MAXDATASIZE - 1, 0);
		if (numbytes == -1)
		{
			if (v[idx].destid != -1)
			{
				v[v[idx].destid].destid=-1;
				v[v[idx].destid].status = "FREE";
			}
			v[idx].destid = -1;
			v[idx].id = -1;
			perror("recv");
			break;
		}
		if (numbytes == 0)
		{
			if (v[idx].destid != -1 && v[v[idx].destid].id!=-1)
			{
				if(v[v[idx].destid].status == "PENDING-A")
				strcpy(buf, "Server:\n***Requesting Client was disconnected unexpectedly***");
				else if(v[v[idx].destid].status == "BUSY")
				strcpy(buf, "Server:\n***Chat has ended***S");
				else if(v[v[idx].destid].status == "PENDING-R")
				strcpy(buf, "Server:\n***Requested Client was disconnected unexpectedly***");
				v[v[idx].destid].status = "FREE";
				
			}
			v[idx].destid = -1;
			v[idx].id = -1;
			printf("Socket disconnected\n");
			break;
		}
		buf[numbytes] = '\0';
		cout << pthread_self() << "-" << idx << " " << buf << " " << numbytes << "\n";
		if (v[idx].status == "FREE")
		{
			if (!strcmp(buf, "close"))
			{
				sendMessage(buf, idx, v[idx].sourcefd, 0);
				v[idx].destid = -1;
				v[idx].id = -1;
				close(v[idx].sourcefd);
				pthread_exit(NULL);
			}
			else if (!strcmp(buf, "list"))
			{
				int i, n = v.size();
				string s = "Server:\n";
				for (i = 0; i < n; i++)
				{
					if (v[i].id != -1 && v[idx].id != v[i].id)
						s = s + "Client-" + to_string(v[i].id) + " Status:" + v[i].status + "\n";
					else if (v[idx].id == v[i].id)
						s = s + "Client-" + to_string(v[i].id) + " Status:" + v[i].status + " (YOU)\n";
				}
				s = s + "Enter <connect <client-id>> to connect to the corresponding client";
				copy(s, buf, 1);
                sendMessage(buf, idx, v[idx].sourcefd, 2);
                continue;
			}
			else
			{
				string s;
				int i, k, chk=0,t;
				copy(s,buf,0);
				if (s.substr(0, 7) != "connect")
					chk=1;
				else
				{
					v[idx].status = "PENDING-R";
					for (i = 7; s[i] != '\0'; i++)
						if (s[i] != ' ')
							break;
					if(s[i]!='\0')
					k = stoi(s.substr(i));
					else
					chk=1;
					for(;s[i]!='\n';i++)
						if('0'> s[i] || s[i] >'9')
						{
							chk=1;
							break;
						}
					if(chk==1)
					{
						v[idx].status = "FREE";
					}
					else if(k>=v.size() || v[k].id==-1)
					{
						v[idx].status = "FREE";	
						strcpy(buf, "Server:\nInvalid Client-Id");
					}
					else if (k==idx || v[k].status == "FREE")
					{
						v[k].status = "PENDING-A";
						v[k].destid = idx;
						v[idx].destid = k;
						v[k].destfd = v[idx].sourcefd;
						v[idx].destfd = v[k].sourcefd;
						createLogFile(idx);
						writeToLogFile(s_idx + s, idx);
						if (k != idx)
						{
							string temp;
							while(s.size() >= 0 && s.back() != '\n'){
								temp.push_back(s.back());
								s.pop_back();
							}
							reverse(temp.begin(), temp.end());
							s="Server:\nWould you like to chat with Client-"+to_string(idx)+"?\nEnter <yes> to accept and <no> to deny the request\n" + temp;
							writeToLogFile(s, idx);
							copy(s,buf,1);
                            sendMessage(buf, idx, v[idx].destfd, 0);
                            // sendMITM(buf, idx);
                            if(!firstMessage) firstMessage = true;
							continue;
						}
						v[idx].status = "BUSY";
						strcpy(buf,"Server:\nYou can now chat with the client");
						writeToLogFile("Server:\nYou can now chat with the client", idx);
					}
					else
					{
						v[idx].status = "FREE";	
						v[idx].destid=-1;
						strcpy(buf, "Server:\nClient is busy at the moment");
						writeToLogFile("Server:\nYou can now chat with the client", idx);
					}
				}
				if(chk==1)
				{
					cout << "I am Free" << "\n";
					strcpy(buf, "Server:\nInvalid Command");
					// writeToLogFile("Server:\nInvalid CommandS", idx);
				}
			}
            sendMessage(buf, idx, v[idx].sourcefd, 2);
		}
		else if(v[idx].status=="PENDING-A")
		{
			string temp;
			copy(temp, buf, 0);
			writeToLogFile(s_idx + temp, idx);
			if(substring(buf,"yes"))
			{
				temp = extractFromLastLine(buf);
				strcpy(buf,"Server:\nYou can now chat with the client");
                sendMessage(buf, idx, v[idx].sourcefd, 2);
				v[idx].status="BUSY";
				v[v[idx].destid].status="BUSY";
				temp = "Server:\nClient has accepted your connection request\nYou can now chat with the client\n" + temp;
				copy(temp, buf, 1);
				writeToLogFile(temp, idx);
                // sendMITM(buf, idx);
			}
			else
			{
				v[idx].status="FREE";
				v[v[idx].destid].status="FREE";
				v[v[idx].destid].destid=-1;
				v[idx].destid = -1;
				strcpy(buf,"Server:\nClient has denied your connection request");
				writeToLogFile("Server:\nClient has denied your connection request", idx);
			}
            sendMessage(buf, idx, v[idx].destfd, 2);
		}
		else if(v[idx].status=="PENDING-R")
		{
			strcpy(buf,"Server:\nConnection request pending\nPlease wait!");
			writeToLogFile("Server:\nConnection request pending\nPlease wait!", idx);
            sendMessage(buf, idx, v[idx].sourcefd, 3);
		}
		else
		{
			cout << 101 << "\n";
			string temp;
			copy(temp, buf, 0);
			writeToLogFile(s_idx + temp, idx);
			// str = "Client-" + to_string(idx) + ":\n";
			if (!strcmp(buf, "good bye"))
			{
				sleep(1);
				numbytes = recv(v[idx].sourcefd, buf, MAXDATASIZE - 1, 0);
				buf[numbytes] = '\0';
				// writeToLogFile(s_idx + temp, idx);
				v[idx].status = "FREE";
				v[v[idx].destid].status = "FREE";
				writeToLogFile("\n\n", idx);
				v[idx].logFilename = v[v[idx].destid].logFilename = "";
				cout << "FREE\n";
				printf("D-309-buf: %s\n", buf);
				if(v[idx].destid==idx)
				{
					v[idx].destid = -1;
					continue;
				}
			}
            sendMessage(buf, idx, v[idx].destfd, 2);
            // sendMITM(buf, idx);
		}
        if(!firstMessage) firstMessage = true;
	}
	v[idx].destid = -1;
	v[idx].id = -1;
	close(v[idx].sourcefd);
	return NULL;
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
int main(void)
{
	// r
	cout << "Enter the value of g: ";
	cin >> g;
	cout << "Enter the value of a: ";
	cin >> a;
	// r
	char buf[MAXDATASIZE];
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	// struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("Server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("Server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (p == NULL)
	{
		fprintf(stderr, "Server: failed to bind\n");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}
	printf("Server: Waiting for connections...\n");
	while (1)
	{ // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("Server: got connection from %s\n", s);
		pthread_t tid;
		data1 pthread_data;
		pthread_data.id = v.size();
		pthread_data.destid = -1;
		pthread_data.sourcefd = new_fd;
		pthread_data.status = "FREE";
		pthread_data.destfd = -1;
		pthread_data.logFilename = "";
		v.push_back(pthread_data);
		pthread_create(&tid, NULL, solve, &pthread_data);
	}
	return 0;
}
