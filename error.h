#include<string>
using namespace std;
class Er{
public:
	Er(){
		connectError="";
		sendError="";
	}
	string getConnectError(){return connectError;}
	string getSendError(){return sendError;}
	string getLoginError(){return loginError;}
	void setLoginError(char* err){loginError=string(err);}
	void setConnectError(char* err){connectError=string(err);}
	void setSendError(char* err){sendError=string(err);}
private:
	string connectError;
	string sendError;
	string loginError;
}; 
