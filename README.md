# 网络通讯层
	&raquo;客户端的网络通讯层分为两部分，一是用于建立连接，发送数据和接收数据的基础部分，二十用于完成具体请求和获取请求结果的应用部分。
	&raquo;服务器端的网络通讯层也分为两部分，一是用于响应用户请求的功能部分，二是服务器初始化部分。

<hr>
# 客户端：
# 基础部分
	struct{
		WSADATA wsd;//存放windows socket初始化信息.
		SOCKET sClient;//存放连接套接字
		struct sockaddr_in server;//存放服务器IP地址
		struct hostent *host = NULL;//域名和网络地址结构体，不使用
		unsigned short port;//连接端口号
	}connection;
	&raquo;struct connection结构体，用于Socket连接的初始化。
	
	bool s_connecting();
	&raquo;用于与服务器建立连接，并返回连接建立的结果。
	
	int s_sending(char* msg,int n);
	&raquo;用于向服务器发送信息，char* msg为需要发送的内容字符串，最大长度为65535字节，int n为请求类型，请求分为以下几种：
		#define LOGIN 1         //登陆
		#define REGISTER 2      //注册
		#define PASSWORD 3      //改密码
		#define TIME 4          //服务器精确到秒的时间，格式如"2017-12-12 12:00:00"
		#define SQL1 5          //不带动态数组的sql
		#define SQL2 6          //带动态数组的sql
		#define LOGOUT 7		//注销
		#define MULTI 8			//批量上传
	
	int s_recving(char* buff);
	&raquo;接收服务器的返回信息，放入char* buff字符串中。buff字符串的最大长度为65535字节。

	void s_close();
	&raquo;关闭连接。
	
# 应用部分
	vector<vector<string> > toVector(char* str);
	&raquo;将字符串转换为二维vector数组，char* str为带转换字符串，返回转换结果。
	
	int server(string command);
	&raquo;请求不需要返回结果集的数据库操作，string command为请求的数据库操作语句，返回执行结果：成功返回0,语法错误返回-1,没有权限返回-2
	
	int server(string command,vector<vector<string> >&result);
	&raquo;请求需要返回结果集的数据库操作，string command为请求的数据库操作语句，vector<vector<string> >&result为返回的结果集，返回执行结果同上。
	
	int login(string id,string password);
	&raquo;登录，string id为登录账号，string password为登录密码，会在函数内部对密码进行Hash，返回登录结果：0登陆成功,-1用户名密码不匹配,-3网络错误
	
	int signIn(string id,string password);
	&raquo;注册，string id为注册账号，string password为密码，在函数内部对密码进行Hash，返回登录结果：0登陆成功,-1重名;-3网络错误

	int change_pwd(string old_pwd,string new_pwd);
	&raquo;更改密码，string old_pwd为原始密码，string new_pwd为新密码，返回更改结果：0更改成功,-1重名,-3网络错误

	int logout();
	&raquo;注销。

	string get_ntime();
	&raquo;获取服务器时间，格式如："2017-12-12 12:00:00"。
	
	
