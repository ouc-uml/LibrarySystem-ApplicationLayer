# 网络通讯层
&raquo;客户端的网络通讯层分为两部分，一是用于建立连接，发送数据和接收数据的基础部分，二十用于完成具体请求和获取请求结果的应用部分。
&raquo;服务器端的网络通讯层也分为两部分，一是用于响应用户请求的功能部分，二是服务器初始化部分。


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
	

# 服务器端：
# 服务器初始化部分
	struct{
		int listenfd;//为当前监听到的客户端连接分配的套接字
		struct sockaddr_in sockaddr;//包含了socket连接初始化的其他内容，如端口号等。
	}svr;
&raquo;struct svr结构体，用于服务器端连接的初始化。

	void initializing();
&raquo;用于数据库表的初始化。

	void starting();
&raquo;启动服务器，并为服务器的send,recv函数添加超时时限（1分钟）。

	bool sending(int fd,int resl,char* msg);
&raquo;向客户端发送请求响应，int fd为发送响应的目标客户端连接套接字，int resl为响应内容，0为成功，非0则为失败，具体失败响应数的设置见客户端函数注释；char* msg为具体响应内容，如客户端请求的数据等。

	bool login(char* inf,int fd);
&raquo;登录，在接收到登录请求时，开启登录功能，char* inf为登录请求的内容，int fd为发送登录请求的客户端连接的套接字。返回bool型登录结果。

	void listening();
&raquo;监听函数。
	
	void ending();
&raquo;关闭服务器。

# 功能部分
	string toString(vector<vector<string> > vec);
&raquo;用于将二维vector数组转化为字符串，vector<vector<string> > vec为待转换的vector数组，返回转换结果。

&raquo;用于完成用户功能的用户类：

	class User{
	public:
		User(int fd,string name);
		int getFd();
		string getName();
		void logout(int fd);
		void holding();
		bool cg_pwd(char* str);
		bool get_time();
		bool sqls_noresl(char* command);
		bool sqls_resl(char* command);
	private:
		int connfd;//套接字
		string name;//账号
	};
	
	User::	User(int fd,string name);
&raquo;构造函数，int fd为用户连接的套接字，string name为用户账号。

	void User::logout(int fd);
&raquo;用户注销，int fd为需要注销的连接套接字。

	bool User::cg_pwd(char* str);
&raquo;更改密码，char* str为更改密码所需信息（即从客户端接收到的信息），返回更改结果。

	bool User::get_time();
&raquo;获取服务器时间，返回执行结果（成功与否）。

	bool User::sqls_noresl(char* command);
&raquo;无返回结果集的数据库指令执行，char* command为需要执行的数据库指令，返回执行结果（成功与否）。

	bool User::sqls_resl(char* command);
&raquo;有返回结果集的数据库指令执行，char* command为需要执行的数据库指令，返回执行结果（成功与否）。
&raquo;以上功能型函数，除toString函数外，均直接将响应结果通过套接字connfd标志的socket连接发送给客户端。

	void User::holding();
&raquo;用于保持与客户端之间的连接，并持续接收客户端请求的函数。
	
