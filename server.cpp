#include<iostream>
#include<fstream>
#include<vector>
#include<thread>
#include"support.h"
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<time.h>

#define MAXLINE 1024
#define LOGIN 1
#define SEARCH 2
#define LOGOUT 3
#define RCMADJ 4
#define RCMDEAL 5
#define RETADJ 6
#define INSERT 7
#define DROP   8
#define UPDATE 9
#define CLNAME 10
#define UPDTINF 11
#define GETARCD 12
#define GETBRCD 13
#define GETRRCD 14
#define BRWAPL 15
#define APTAPL 16
#define RETAPL 17
#define RCMAPL 18
#define COMMENT 19
#define SIGNIN 20
	
#define MESSAGE (char*)"%d\n%s\n"
#define ACPERR (char*)"accpet socket error: %s errno :%d\n"

using namespace std;

struct{
	int listenfd;
	struct sockaddr_in sockaddr;
}server;

void updating(){
	//每到0点更新数据库
	time_t timep;
	char stime[64];
	while(true){
		time(&timep);
		strftime(stime, sizeof(stime), "%H:%M:%S",localtime(&timep));
		//cout<<stime<<endl;
		if(!strcmp(stime,(char*)"00:00:00")){
			//更新数据库内容
			ifstream commands;
			commands.open("table_update.txt",ios::out);
			string buff;
			while(getline(commands,buff)){
				char combuff[256];
				vector<vector<string> > result;
				sprintf(combuff,buff.c_str(),stime);
				string command(combuff);
				cout<<command<<endl;
				//server(command,result);
				//给超时的人发消息
			}
			commands.close();
		}
		sleep(1);
	}
}

void initializing(){
	//建默认表
	ifstream commands;
	commands.open("table_structure.txt",ios::out);
	string command;
	while(getline(commands,command)){
		cout<<command<<endl;
		//server(command);
	}
	commands.close();
	thread th(updating);
	th.detach();
}

void starting(){//启动服务器
	initializing();

	memset(&server.sockaddr,0,sizeof(sockaddr));
 
	server.sockaddr.sin_family = AF_INET;
 	server.sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 	server.sockaddr.sin_port = htons(10004);
 
 	server.listenfd = socket(AF_INET,SOCK_STREAM,0);

 	bind(server.listenfd,(struct sockaddr *) &server.sockaddr,sizeof(server.sockaddr));
 
 	listen(server.listenfd,1024);

	printf("已开机...\n");
}

bool login(char* inf,int fd);

void listening(){
	int connfd;
	char buff[MAXLINE];
	int n;
	while(true){
 		if((connfd = accept(server.listenfd,(struct sockaddr*)NULL,NULL))==-1){
			printf(ACPERR,strerror(errno),errno);
 			continue;
 		}
 		n = recv(connfd,buff,MAXLINE,0);
 		buff[n] = '\0';
		if(atoi(buff)==LOGIN){
			login(buff,connfd);
		}else if(atoi(buff)==SIGNIN){
			//注册
		}else{
			char msg[MAXLINE];
			sprintf(msg,MESSAGE,0,(char*)"Please Login First!");
			send(connfd,msg,strlen(msg),0);
		}
 	}
}

void ending(){//关闭服务器
	close(server.listenfd);
}

void logout(int fd){
	char msg[MAXLINE];
	sprintf(msg,MESSAGE,1,(char*)"");
	send(fd,msg,strlen(msg),0);
	cout<<fd<<" logout\n";
	close(fd);
}

class Tempo{
public:
	Tempo(int fd,char* rstr){
		connfd=fd;
		rans=new char[48];
		sprintf(rans,"Token: %s",rstr);
	}
	int getFd(){return connfd;}
	char* getRans(){return rans;}
	void holding(){
		char msg[MAXLINE],buff[MAXLINE];
		int n;
		cout<<"holding..."<<endl;
		while(true){
			n = recv(connfd,buff,MAXLINE,0);
 			buff[n] = '\0';
			n=atoi(strtok(buff,(char*)"\n"));
			if(!strcmp(strtok(NULL,(char*)"\n"),getRans()))
				switch(n){//待补充
					case LOGOUT	:logout(getFd());return;
					case SEARCH	:break;
					case RCMADJ	:break;
					case RCMDEAL	:break;
					case RETADJ	:break;
					case INSERT	:break;
					case DROP	:break;
					case UPDATE	:break;
					case CLNAME	:break;
					case UPDTINF	:break;
					case GETARCD	:break;
					case GETBRCD	:break;
					case GETRRCD	:break;
					case BRWAPL	:break;
					case APTAPL	:break;
					case RETAPL	:break;
					case RCMAPL	:break;
					case COMMENT	:break;
					default	:break;
				}
			else{
				sprintf(msg,MESSAGE,0,(char*)"ERROR IDENTITY!");
				send(getFd(),msg,strlen(msg),0);
				return;
			}
		}
	}
private:
	int connfd;
	char* rans;
};

void thrd(int fd,char* rstr){
	Tempo tp(fd,rstr);
	tp.holding();
}


bool login(char* inf,int fd){
	char msg[MAXLINE],rans[41],buff[256];
	char *acc,*psw;
	int type;
	bool resl=false;
	strtok(inf,(char*)"\n");
	strtok(NULL,(char*)"\n");
	type=atoi(strtok(NULL,(char*)"\t"));
	acc=strtok(NULL,(char*)"\t");
	psw=strtok(NULL,(char*)"\t");//可能会出错

	//查询数据库确认是否登录成功
	ifstream commands;
	vector<vector<string> > result;
	commands.open("login_c.txt",ios::out);
	string command;
	getline(commands,command);

	if(!type)//如果是1则是普通用户，查询普通用户表,否则查询管理员表
		getline(commands,command);
	sprintf(buff,command.c_str(),acc);
	command=buff;
	//server(command,result);
	commands.close();
	//if(result[0][0]==string(acc)){
		ranString(rans);
		sprintf(msg,MESSAGE,1,rans);
		cout<<fd<<" login"<<endl;
		thread th(thrd,fd,rans);
		th.detach();
		send(fd,msg,strlen(msg),0);
	/*}else{
		sprintf(msg,MESSAGE,0,(char*)"");
		send(fd,msg,strlen(msg),0);
	}*/
	return true;
}

int main(int argc,char **argv){
	srand(time(0));
 	starting();
	listening();	
 	ending();
	updating();
}
//g++ server.cpp support.cpp  -lpthread -o server
