#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<winsock2.h>
#include<windows.h>

#include"connect.h"

#define CONERR (char*)"connect error %s errno: %d\n"
#define SENERR (char*)"send mes error %s errno: %d\n"
#define MAXLINE 1024
#define MSGLEN  256
#define SERVER_ADDR (char*)"127.0.0.1"
#define SERVER_PORT 10004
#define MESSAGE (char*)"%d\n%s\n%s"

using namespace std;
struct{
WSADATA wsd;
SOCKET sClient;
struct sockaddr_in server;
struct hostent *host = NULL;
unsigned short port;
}connection;

bool s_connecting(){
	char err[MSGLEN];

    if (WSAStartup(MAKEWORD(2 , 2) , &connection.wsd) != 0) {

        //sprintf(err,"Winsock    鍒濆鍖栧け璐?\n");

        printf("Winsock    鍒濆鍖栧け璐?\n");

        Error.setConnectError(err);

        return false;

    }

    connection.sClient = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);

    if (connection.sClient == INVALID_SOCKET) {

        //sprintf(err,"socket() 澶辫触: %d\n" , WSAGetLastError());

        printf("socket() 澶辫触: %d\n" , WSAGetLastError());

        Error.setConnectError(err);

        return false;

    }

    connection.server.sin_family = AF_INET;

    connection.port = SERVER_PORT;

    connection.server.sin_port = htons(connection.port);

    connection.server.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(connection.sClient , (struct sockaddr*)&connection.server ,sizeof(connection.server)) == SOCKET_ERROR) {

        //sprintf(err,"connect() 澶辫触: %d\n" , WSAGetLastError());

        printf("connect() 澶辫触: %d\n" , WSAGetLastError());

        return false;

    }

    return true;
}

bool s_sending(int rqt,char* token,char* msg){
	char mssg[MAXLINE];
	char err[MSGLEN];
	memset(mssg,0,MAXLINE);
	sprintf(mssg,MESSAGE,rqt,token,msg);
	if((send(connection.sClient,mssg,strlen(mssg),0)) < 0){
 		sprintf(err,"send() 澶辫触: %d\n",WSAGetLastError());
		Error.setSendRecvError(err);
 		return false;
 	}
	return true;
}

bool s_recving(char* buff){
	int n;

    char err[MAXLINE];
	n = recv(connection.sClient,buff,MAXLINE,0);
 	buff[n] = '\0';
	if(n>0)return true;
	else if (n == SOCKET_ERROR){

        sprintf(err,"recv() 澶辫触: %d\n" , WSAGetLastError());

        Error.setSendRecvError(err);

            return false;

	}
}

void s_close(){
	closesocket(connection.sClient);

	WSACleanup();    //娓呯悊
}
