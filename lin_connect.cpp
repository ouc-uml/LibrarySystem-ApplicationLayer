#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netinet/in.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>

#include"connect.h"

#define CONERR (char*)"connect error %s errno: %d\n"
#define SENERR (char*)"send mes error %s errno: %d\n"
#define MAXLINE 1024
#define MSGLEN  256
#define SERVER_ADDR (char*)"127.0.0.1"
#define SERVER_PORT 10004
#define MESSAGE (char*)"%d\nToken: %s\nMSG: %s"

using namespace std;
struct{
	char *servInetAddr =SERVER_ADDR;// "127.0.0.1";
	int socketfd;
	struct sockaddr_in sockaddr;
 	char recvline[MAXLINE], acc[MAXLINE],pwd[MAXLINE];
 	int n;
}connection;

bool s_connecting(){
	char err[MSGLEN];
	connection.socketfd = socket(AF_INET,SOCK_STREAM,0);
 	memset(&connection.sockaddr,0,sizeof(connection.sockaddr));
 	connection.sockaddr.sin_family = AF_INET;
 	connection.sockaddr.sin_port = htons(SERVER_PORT);
 	inet_pton(AF_INET,connection.servInetAddr,&connection.sockaddr.sin_addr);
 	if((connect(connection.socketfd,(struct sockaddr*)&connection.sockaddr,sizeof(connection.sockaddr))) < 0 ){
 		sprintf(err,CONERR,strerror(errno),errno);
		printf(CONERR,strerror(errno),errno);
		Error.setConnectError(err);
 		return false;
 	}
	return true;
}

bool s_sending(int rqt,char* token,char* msg){
	char mssg[MAXLINE];
	char err[MSGLEN];
	memset(mssg,0,MAXLINE);
	sprintf(mssg,MESSAGE,rqt,token,msg);
	if((send(connection.socketfd,mssg,strlen(mssg),0)) < 0){
 		sprintf(err,SENERR,strerror(errno),errno);
		printf(SENERR,strerror(errno),errno);
		Error.setSendError(err);
 		return false;
 	}
	return true;
}

bool s_recving(char* buff){
	int n;
	n = recv(connection.socketfd,buff,MAXLINE,0);
 	buff[n] = '\0';
	if(n>0)return true;
	else return false;
}

void s_close(){
	close(connection.socketfd);
}
