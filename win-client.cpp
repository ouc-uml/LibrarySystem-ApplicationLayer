#include<iostream>
#include<vector>
//#include"md5.h"
#include<stdio.h>
#include<winsock2.h>
#include<windows.h>
#include<stdlib.h>
//#include<netinet/in.h>
#include<errno.h>
#include<string.h>
//#include<arpa/inet.h>
#include<unistd.h>

#include"appliLayer.h"

#pragma  comment(lib,"ws2_32.lib")

using namespace std;

bool s_connecting(){
	char err[MSGLEN];
    if (WSAStartup(MAKEWORD(2 , 2) , &connection.wsd) != 0) {
        //sprintf(err,"Winsock    初始化失败!\n");
        printf("Winsock    初始化失败!\n");
        Error.setConnectError(err);
        return false;
    }
    connection.sClient = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (connection.sClient == INVALID_SOCKET) {
        //sprintf(err,"socket() 失败: %d\n" , WSAGetLastError());
        printf("socket() 失败: %d\n" , WSAGetLastError());
        Error.setConnectError(err);
        return false;
    }
    connection.server.sin_family = AF_INET;
    connection.port = SERVER_PORT;
    connection.server.sin_port = htons(connection.port);
    connection.server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    if (connect(connection.sClient , (struct sockaddr*)&connection.server ,sizeof(connection.server)) == SOCKET_ERROR) {
        //sprintf(err,"connect() 失败: %d\n" , WSAGetLastError());
        printf("connect() 失败: %d\n" , WSAGetLastError());
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
 		sprintf(err,"send() 失败: %d\n",WSAGetLastError());
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
        sprintf(err,"recv() 失败: %d\n" , WSAGetLastError());
        Error.setSendRecvError(err);
            return false;
	}
}

void s_close(){
	closesocket(connection.sClient);
	WSACleanup();    //清理
}

bool Users::getClmnName(char* tName,char* cnames){
	char msg[MAXLINE],buff[MAXLINE];
	memset(msg,0,MAXLINE);
	sprintf(msg,"%s",tName);
	if(s_sending(CLNAME,token,msg)){
		while(!s_recving(buff));
		strtok(buff,(char*)"\n");
		if(atoi(buff)){
			cnames=strtok(NULL,(char*)"\n");
			return true;
		}else return false;
	}
}

Users::Users(){
	accounts=NULL;
	if_login=false;
	rtime=NULL;
	rank=-1;
	type=-1;
}

Users::Users(int tp,char* accounts,char* password){
	char msg[MAXLINE],buff[MAXLINE];
	char* err;
	s_connecting();
	memset(msg,0,MAXLINE);
	memset(buff,0,MAXLINE);
	sprintf(msg,LGNMSG,getType(),accounts,password);
	this->type=tp;
	if(s_sending(LOGIN,(char*)"",msg)){
		while(!s_recving(buff));//{t--;sleep(1);}
		if(atoi(buff)>0){
			strtok(buff,(char*)"\n");
			this->accounts=accounts;
			sprintf(this->token,"%s\0",strtok(NULL,(char*)"\n"));
			this->if_login=true;
		}else{
			this->if_login=false;
			Error.setLoginError("登录失败：用户名或密码有误！\n");
		}
	}else{
			Error.setLoginError("登录失败：网络出错\n");
	}
}

Users::~Users(){
	if(if_login)logout();
	s_close();

}

bool Users::search(char* table/*表名*/,char* attribute/*属性名*/,char* value/*属性值*/,char* resl/*查询结果*/){
	char msg[MAXLINE],buff[MAXLINE];
	memset(msg,0,MAXLINE);
	sprintf(msg,SRCMSG,table,attribute,value);
	if(s_sending(SEARCH,token,msg)){
		while(!s_recving(buff));
		strtok(buff,(char*)"\n");
		if(atoi(buff)){
			resl=strtok(NULL,(char*)"\n");
			return true;
		}else return false;
	}
}

void Users::logout(){
	char msg[MAXLINE],buff[MAXLINE];
	memset(msg,0,MAXLINE);
	cout<<token<<endl;
	while(if_login){
		if(s_sending(LOGOUT,token,(char*)"")){
			while(!s_recving(buff));
			if(atoi(buff)>0){
				this->if_login=false;
				break;
			}
		}
	}
}

bool NormalUsers::updateInf(){
	char buff[MAXLINE],cNames[MAXLINE];
	char *cName,*cValue;
	vector<char*> clmNames;
	if(s_sending(UPDTINF,getToken(),(char*)"")){
		while(!s_recving(buff));
		if(atoi(buff)){
			getClmnName(TABLE_USER,cNames);
			cName=strtok(cNames,SPACER);
			while(cName){
				clmNames.push_back(cName);
				cName=strtok(NULL,SPACER);
			}
			strtok(buff,(char*)"\n");
			for(int i=0;i<clmNames.size();i++){
				cValue=strtok(NULL,SPACER);
				if(cValue){
					if(!strcmp(clmNames[i],CRANK)){
						setRank(atoi(cValue));
					}else if(!strcmp(clmNames[i],CNAME)){
						this->name=cValue;
					}else if(!strcmp(clmNames[i],CBPLUS)){
						this->Bsurplus=atoi(cValue);
					}else if(!strcmp(clmNames[i],CAPLUS)){
						this->Asurplus=atoi(cValue);
					}
				}else printf("Selection Results Error!\n");
			}
			return true;
		}else return false;
	}
}

bool Administrators::updateInf(){
	char buff[MAXLINE],cNames[MAXLINE];
	char *cName,*cValue;
	vector<char*> clmNames;
	if(s_sending(UPDTINF,getToken(),(char*)"")){
		while(!s_recving(buff));
		if(atoi(buff)){
			getClmnName(TABLE_ADMIN,cNames);
			cName=strtok(cNames,SPACER);
			while(cName){
				clmNames.push_back(cName);
				cName=strtok(NULL,SPACER);
			}
			strtok(buff,(char*)"\n");
			for(int i=0;i<clmNames.size();i++){
				cValue=strtok(NULL,SPACER);
				if(cValue){
					if(!strcmp(clmNames[i],CRANK)){
						setRank(atoi(cValue));
					}
				}else printf("Selection Results Error!\n");
			}
			return true;
		}else return false;
	}
}

bool Administrators::recommAdj(char* recommId,bool resl){
	char msg[MAXLINE],buff[MAXLINE];
	if(resl){
		sprintf(msg,"%s\t%d",recommId,1);
		if(s_sending(RCMADJ,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
	}else{
		sprintf(msg,"%s\t%d",recommId,0);
		if(s_sending(RCMADJ,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
	}
}//错误信息？

bool Administrators::recommDeal(char* recommId,bool resl){
	char msg[MAXLINE],buff[MAXLINE];
	if(resl){
		sprintf(msg,"%s\t%d",recommId,1);
		if(s_sending(RCMDEAL,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
	}else{
		sprintf(msg,"%s\t%d",recommId,0);
		if(s_sending(RCMDEAL,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
	}
}

bool Administrators::retrnAdj(char* bookId,bool resl){
	char msg[MAXLINE],buff[MAXLINE];
	if(resl){
		sprintf(msg,"%s\t%d",bookId,1);
		if(s_sending(RETADJ,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
	}else{
		sprintf(msg,"%s\t%d",bookId,0);
		if(s_sending(RETADJ,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
	}
}

bool Administrators::insert(char* tName,char* inf){//inf是要插入条目的信息，其中信息为NULL就是空字符串
		char msg[MAXLINE],buff[MAXLINE];
		sprintf(msg,"table: %s\tinf: %s",tName,inf);
		if(s_sending(INSERT,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}

bool Administrators::update(char* tName,char* inf){//inf是要修改的内容，为一个条目的所有属性或者是前n个属性
		char msg[MAXLINE],buff[MAXLINE];
		sprintf(msg,"table: %s\tinf: %s",tName,inf);
		if(s_sending(UPDATE,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}
//如果有的属性不需要更改呢？

bool Administrators::drop(char* tName,char* key){//删除主键值为Key的条目
		char msg[MAXLINE],buff[MAXLINE];
		sprintf(msg,"table: %s\tkey: %s",tName,key);
		if(s_sending(DROP,getToken(),msg)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}

bool NormalUsers::getArecord(vector<char*>& aRecord){
		char buff[MAXLINE];
		char* item;
		if(s_sending(GETARCD,getToken(),(char*)"")){
			while(!s_recving(buff));
			if(atoi(buff)){
				strtok(buff,(char*)"\n");
				while(item=strtok(NULL,(char*)"\t")) aRecord.push_back(item);//\t用于隔开不同条目
				return true;
			}else return false;
		}
}

bool NormalUsers::getBrecord(vector<char*>& bRecord){
		char buff[MAXLINE];
		char* item;
		if(s_sending(GETBRCD,getToken(),(char*)"")){
			while(!s_recving(buff));
			if(atoi(buff)){
				strtok(buff,(char*)"\n");
				while(item=strtok(NULL,(char*)"\t")) bRecord.push_back(item);//\t用于隔开不同条目
				return true;
			}else return false;
		}
}

bool NormalUsers::getRrecord(vector<char*>& rRecord){
		char buff[MAXLINE];
		char* item;
		if(s_sending(GETRRCD,getToken(),(char*)"")){
			while(!s_recving(buff));
			if(atoi(buff)){
				strtok(buff,(char*)"\n");
				while(item=strtok(NULL,(char*)"\t")) rRecord.push_back(item);//\t用于隔开不同条目
				return true;
			}else return false;
		}
}

bool NormalUsers::borrowApply(char* bookId){
		char buff[MAXLINE];
		if(s_sending(BRWAPL,getToken(),bookId)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}

bool NormalUsers::appointApply(char* bookId){
		char buff[MAXLINE];
		if(s_sending(APTAPL,getToken(),bookId)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}

bool NormalUsers::retnApply(char* bookId){
		char buff[MAXLINE];
		if(s_sending(RETAPL,getToken(),bookId)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}

bool NormalUsers::recommApply(char* inf){
		char buff[MAXLINE];
		if(s_sending(RCMAPL,getToken(),inf)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}

bool NormalUsers::comment(char* cmt){
		char buff[MAXLINE];
		if(s_sending(COMMENT,getToken(),cmt)){
			while(!s_recving(buff));
			if(atoi(buff)) return true;
			else return false;
		}
}

bool signIn(char *pwd,char* resl){
	char buff[MAXLINE],msg[MAXLINE];
	sprintf(msg,"0\t%s",pwd);
	if(s_sending(SIGNIN,(char*)"",msg)){
		while(!s_recving(buff));
		if(atoi(buff)){
			strtok(buff,(char*)"\n");
			resl=strtok(NULL,(char*)"\n");
			return true;
		}
		else return false;
	}
}

bool signIn(char* name,char *pwd,char* resl){
	char buff[MAXLINE],msg[MAXLINE];
	sprintf(msg,"1\t%s\t%s",name,pwd);
	if(s_sending(SIGNIN,(char*)"",msg)){
		while(!s_recving(buff));
		if(atoi(buff)){
			strtok(buff,(char*)"\n");
			resl=strtok(NULL,(char*)"\n");
			return true;
		}
		else return false;
	}
}

int main(int argc,char **argv){
	Administrators *ad=new Administrators((char*)"ad",(char*)"123456");
	if(ad->getLogFlag())delete ad;
 	return 0;
}


