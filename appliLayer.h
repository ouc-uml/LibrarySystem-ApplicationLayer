#include<vector>
#include<string>

#define MAXLINE 1024
#define MSGLEN  256
#define SERVER_ADDR (char*)"127.0.0.1"
#define SERVER_PORT 10004
#define MESSAGE (char*)"%d\nToken: %s\nMSG: %s"
#define LGNMSG (char*)"%d\t%s\t%s"
#define CONERR (char*)"connect error %s errno: %d\n"
#define SENERR (char*)"send mes error %s errno: %d\n"
#define SRCMSG (char*)"table: %s\tattribute: %s\tvalue: %s"
#define SPACER (char*)","

#define TABLE_USER (char*)"users"
#define TABLE_ADMIN (char*)"administrations"
#define CRANK (char*)"rank"
#define CNAME (char*)"name"
#define CAPLUS (char*)"a_surplus"
#define CBPLUS (char*)"b_surplus"

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

using namespace std;

class {
public:
	Error(){
		connectError="";
		sendError="";
	}
	string getConnectError(){return connectError;}
	string getSendError(){return sendError;}
	string getLoginError(){return loginError;}
	void setLoginError(char* err){loginError=string(err);}
	void setConnectError(char* err){connectError=string(err);}
	void setSendError(){sendError=string(err);}
private:
	string connectError;
	string sendError;
	string loginError;
}Error;


struct{
	char *servInetAddr =SERVER_ADDR;// "127.0.0.1";
	int socketfd;
	struct sockaddr_in sockaddr;
 	char recvline[MAXLINE], acc[MAXLINE],pwd[MAXLINE];
 	int n;
}connection;

bool s_connecting();
bool s_sending(int rqt,char* token,char* msg);
bool s_recving(char* buff);
void s_close();
bool signIn(char* pwd,char* resl);//管理员专用
bool signIn(char* name,char* pwd,char* resl);//用户专用

class Users{
public:
	Users();
	Users(int tp,char* acc,char* pwd);//相当于登录
	~Users();
	char* getAccounts(){return accounts;}
	char* getRtime(){return rtime;}
	bool getLogFlag(){return if_login;}
	int getRank(){return rank;}
	char* getToken(){return token;}
	int getType(){return type;}
	void setRank(int rk){rank=rk;}
	void setLogFlag(){if_login=true;}
	//void setType(int typ){type=typ;}//0-管理员;1-普通用户
	void clearLogFlag(){if_login=false;}
	bool search(char* table/*表名*/,char* attribute/*属性名*/,char* value/*属性值*/,char* resl/*查询结果*/);
	void logout();
	
	bool updateInf(){return true;}
	bool getClmnName(char* tName,char* cnames);
	
private:
	char* accounts;//帐号
	char token[41] ;
	bool if_login;
	char* rtime;//注册日期
	int rank;
	int type;
};

class Administrators:public Users{
public:
	Administrators(){Users();}
	Administrators(char* acc,char* pwd):Users(0,acc,pwd){
		//setType(0);
		//Users(acc,pwd);
		//updateInf();
	}//相当于登录
	
	bool recommAdj(char* recommId,bool resl);//荐购判定
	bool recommDeal(char* recommId,bool resl);//返回荐购处理的结果
	bool retrnAdj(char* bookId,bool resl);//还书审核
	bool usersDeal(char* usersId,int dealId);//用户处理
	bool insert(char* tName,char* inf);//插入
	bool drop(char* tName,char* key);//删除
	bool update(char* tName,char* inf);//更新

	bool updateInf();
};

class NormalUsers:public Users{
public:
	NormalUsers(){Users();}
	NormalUsers(char* acc,char* pwd):Users(1,acc,pwd){
		//setType(1);
		//Users(acc,pwd);
		//updateInf();
	}//相当于登录
	//Gets 包括属性中没有需要临时查找的内容

	bool getArecord(vector<char*>& aRecord);//返回预约记录
	bool getBrecord(vector<char*>& bRecord);//返回借阅记录
	bool getRrecord(vector<char*>& rRecord);//返回荐购记录

	char* getName(){return name;}
	int getBsurplus(){return Bsurplus;}//返回借阅剩余量
	int getAsurplus(){return Asurplus;}//返回预约剩余量
	
	bool borrowApply(char* bookId);//借阅申请
	bool appointApply(char* bookId);//预约申请
	bool retnApply(char* bookId);//还书申请;bookId是单本ID
	bool recommApply(char* inf);//荐购申请
	//bool losrepoApply();//挂失申请
	bool comment(char* cmt);//评论	

	bool updateInf();
private:
	char* name;
	int Bsurplus;//借阅剩余量
	int Asurplus;//预约剩余量
};
