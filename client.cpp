#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <bits/stdc++.h>
using namespace std;
#define PORT "3490"		 // the port client will be connecting to
#define MAXDATASIZE 1000 // max number of bytes we can get at once
using namespace std;
// get sockaddr, IPv4 or IPv6:
typedef struct data1
{
	int sockfd;
} data1;
int status, check;
string stat = "FREE";
long long int g = -1, a, B, x;
string key, client_id, from = "server";
string extractFromLastLine(char buf[])
{
	string temp;
	int i;
	for(i = strlen(buf) - 1; buf[i] != '\n'; i--){
		temp.push_back(buf[i]);
	}
	buf[i] = '\0';
	reverse(temp.begin(), temp.end());
	// cout << "D-35-" << temp << "\n";
	return temp;
}
vector<int> RC4(vector<int> &ip, vector<int>&K, int &K_len)
{
    int S[256], T[256];
    // Initialization
    int i;
    for(i = 0; i < 256; i++){
        S[i] = i;
        T[i] = K[i%K_len];
    }
    int j = 0;
    for(i = 0;i < 256; i++){
        j = (j + S[i] + T[i])%256;
        int temp = S[i];
        S[i]  = S[j];
        S[j] = temp;
    }

    // Stream generation
    i = 0, j = 0;
    vector<int>K1;
    for(int t = 0; t < ip.size(); t++){
        i = (i + 1)%256;
        j = (j + S[i])%256;
        int temp = S[i];
        S[i] = S[j];
        S[j] = temp;
        temp = (S[i] + S[j])%256;
        K1.push_back(S[temp]);
    }
    cout << "\n";
    // Encrypting using XOR
    vector<int>op;
    for(int i=0;i<ip.size();i++){
        int p = K1[i]^ip[i];
        op.push_back(p);
    }
    return op;
}
void RC4_Cipher(char buf[])
{
	vector<int>ip, K;
	int K_len, i;
	for(int i = 0; buf[i] != '\0'; i++){
		ip.push_back(buf[i]);
	}
	for(int i = 0; i < key.size(); i++){
		K.push_back(key[i]);
	}
	K_len = K.size();
	vector<int>op = RC4(ip, K, K_len);
	for(i = 0; buf[i] != '\0'; i++){
		buf[i] = op[i];
	}
	buf[i] = '\0';
}
bool substring(char buf[], string s)
{
	for(int i = 0; i < s.size(); i++){
		if(buf[i] == '\0' || buf[i] != s[i]) return false;
	}
	return true;
}
long long int power(long long int B, long long int x, long long int a)
{
	long long int ans = 1;
	while(x--){
		ans = (ans*B)%a;
	}
	return ans;
}
void *solve(void *p)
{
	char buf[MAXDATASIZE];
	int numbytes;
	data1 *ptr = (data1 *)p;
	string s = " good bye", temp;
	int i, n;
	numbytes = recv(ptr->sockfd, buf, MAXDATASIZE - 1, 0);
	buf[numbytes] = '\0';
	for(i = 86; buf[i]!=' '; i++){
		temp.push_back(buf[i]);
	}
	// cout << "D-120-" << temp << "\n";
	g = stoll(temp);
	temp = "";
	for(i++; buf[i]!='\0'; i++){
		temp.push_back(buf[i]);
	}
	a = stoll(temp);
	temp = "";
	buf[86] = '\0';
	printf("\n%s\n", buf);
	cout << "\nClient:\n";
	while (1)
	{
		numbytes = recv(ptr->sockfd, buf, MAXDATASIZE - 1, 0);
		if (numbytes == -1)
		{
			perror("recv");
			break;
		}
		if (numbytes == 0)
		{
			printf("Socket disconnected*\n");
			break;
		}
		buf[numbytes] = '\0';
		if(stat == "BUSY"){
			RC4_Cipher(buf);
			// printf("D-147-%s\n", buf);
			if(buf[numbytes - 1] == 'S') from = "server";
			else from = "client";
		}
		numbytes--;
		buf[numbytes] = '\0';
		if(from == "server")
		{
			if (check == 0 && substring(buf, "Server:\nYou can now chat with the client"))
			{
				if(stat == "PENDING-R")
				{
					B = stoll(extractFromLastLine(buf));
					key = to_string(power(B, x, a));
				}
				else extractFromLastLine(buf);
				stat = "BUSY";
				check = 1;
			}
			else if (check == 0 && substring(buf, "Server:\nWould")){
				printf("%s\n", buf);
				for(int i = 44; buf[i] != '?'; i++){
					client_id.push_back(buf[i]);
				}
				B = stoll(extractFromLastLine(buf));
				stat = "PENDING-A";
			}
			else if (check == 1 && buf[0] == 'S')
			{
				check = 0;
				stat = "FREE";
				client_id = "";
			}	
		}
		else if (check == 1)
		{
			for (i = 0; buf[i] != '\0'; i++)
				if (buf[i] == ':')
					break;
			i++;
			if (numbytes - i == s.size())
			{
				for (; s[i] != '\0'; i++)
					if (s[i] != buf[i])
						break;
				if (s[i] == '\0')
					check = 0, stat = "FREE", client_id = "";
			}
		}
		if(from == "client") cout << "Client-" << client_id << ":\n";
		printf("\n%s\n", buf);
		cout << "\nClient:\n";
	}
	status = 1;
	pthread_exit(NULL);
}
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
void choosePrivateKey(char buf[])
{
	x = rand();
	string A = to_string(power(g, x, a));
	int n = strlen(buf), i;
	buf[n] = '\n';
	n++;
	for(i = 0; i < A.size(); i++){
		buf[i + n] = A[i];
	}
	buf[i + n] = '\0';
	// printf("D-220-%s\n", buf);
}
int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	if (argc != 2)
	{
		fprintf(stderr, "Usage: Client hostname\n");
		exit(1);
	}
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("Client: socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("Client: connect");
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	status = 0;
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("Client: connecting to %s\n", s);
	freeaddrinfo(servinfo); // all done with this structure
	data1 pthread_data;
	pthread_data.sockfd = sockfd;
	pthread_t tid;
	pthread_create(&tid, NULL, solve, &pthread_data);
	while (1)
	{
		if (status == 1)
			break;
		scanf(" %[^\n]s", buf);
		// printf("\n");
		// cout << "--\n";
		printf("------------------\n");
		//cout << check << "\n";
		if (check==0 && stat == "FREE" && substring(buf, "connect"))
		{
			int i;
			for(i = 7; buf[i] != ' '; i++);
			string temp;
			for(; '0' <= buf[i] && buf[i] <= '9'; i++) temp.push_back(buf[i]);
			client_id = temp;
			stat = "PENDING-R";
			choosePrivateKey(buf);
		}
		else if (check == 0 && stat == "PENDING-A" && !strcmp(buf, "yes"))
		{
			choosePrivateKey(buf);
			key = to_string(power(B, x, a));
		}
		else if (check==1 && stat == "BUSY" && !strcmp(buf, "good bye"))
		{
			cout << "\nServer:\n***Chat has ended***\n\nClient:\n";
			check = 0;
			stat = "FREE";
		}
		else if (check == 1 && stat == "BUSY")
		{
			int n = strlen(buf);
			buf[n] = 'C';
			buf[n + 1] = '\0';
			RC4_Cipher(buf);
			for(int i = 0; i < strlen(buf); i++) cout << buf[i] << " ";
			cout << "\n";
		}
		else if ((!strcmp(buf, "close") && !check) || status)
		{
			//cout << "please leave ra\n";
			if (send(sockfd, buf, strlen(buf), 0) == -1)
			{
				perror("send");
				exit(1);
			}
			break;
		}
		if (send(sockfd, buf, strlen(buf), 0) == -1)
		{
			perror("send");
			exit(1);
		}
	}
	close(sockfd);
	return 0;
}
