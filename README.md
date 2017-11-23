# 图书管理系统应用层-UI接口介绍

# 错误类Error
  	string Error::getConnectError();
	//获取连接时出现的错误
	string Error::getSendRecvError();
	//获取发送/接收时出现的错误
	string getLoginError();
	//获取登录时遇到的错误
	
# 其他-注册函数
	bool signIn(char* pwd/*密码*/,char* resl/*返回为用户分配的账号*/);
	//管理员专用
	bool signIn(char* name/*昵称/姓名*/,char* pwd/*密码*/,char* resl/*返回为用户分配的账号*/);
	//用户专用
	
# Users(用户)类

	Users::Users();
  //空构造函数
  
	Users::Users(int tp,char* acc,char* pwd);
  //父类构造函数，内含登录功能，应该不会单独用到0v0
  
	Users::~Users();
  //父类析构函数，内含登出功能
  
	char* Users::getAccounts();
  //获取用户帐号
  
	char* Users::getRtime();
  //获取用户注册时间
  
	bool Users::getLogFlag();
  //检测是否处于登录状态
  
	int Users::getRank();
  //获取用户级别
  
	char* Users::getToken();
  //获取已登录用户的Token
  
	int Users::getType();
  //获取用户类型，0为管理员，1为用户
  
	void Users::setLogFlag();
  //将登录标志置为TRUE
  
	void Users::clearLogFlag();
  //将登录标志置为FALSE
  
	bool Users::search(char* table/*表名*/,char* attribute/*属性名*/,char* value/*属性值*/,char* resl/*查询结果*/);
  //查找函数，查找table表中attribute属性值为value的条目，在返回的结果中，同一条目的不同属性以英文逗号分割，不同条目以Tab符分割
  
  void Users::logout();
  //登出函数，同时被包含在析构函数里
	
	bool Users::updateInf();
  //更新Users对象中的属性值，避免Users对象属性值与数据库内容不一致，Users对象属性包括：帐号，注册日期，用户级别//用户级别，注册日期和帐号一般情况下不会更改，应该用不到
  
	bool Users::getClmnName(char* tName/*表名*/,char* cnames/*返回结果*/);
  //获得tName表的所有列名，返回结果在cnames中，不同列名以英文逗号分割

# Administrators(管理员)类
管理员类public继承用户类

	Administrators::Administrators();
  //管理员类空构造函数
  
	Administrators::Administrators(char* acc/*帐号*/,char* pwd/*密码*/);
  //管理员类构造函数，内含登录功能
	
  //以下函数均返回服务器处理结果，若处理成功为TRUE，否则为FALSE
  
	bool Administrators::recommAdj(char* recommId/*荐购事件ID*/,bool resl/*对荐购事件的判定结果*/);
  //荐购判定，若通过recommId荐购事件，准备开始建构处理，则resl为TRUE，若不通过，则resl为FALSE
  
	bool Administrators::recommDeal(char* recommId,bool resl);
  //荐购处理，若荐购事件recommId处理完毕，且处理成功，则resl写TRUE，若处理失败，则resl写FALSE
  
	bool Administrators::retrnAdj(char* bookId/*被还书目ID*/,bool resl/*审核结果*/);
  //还书审核，若书籍bookId还书成功，书籍无污损，则resl置TRUE，否则resl置FALSE
  
	bool Administrators::usersDeal(char* usersId,int dealId);
  //用户处理，对用户usersId进行dealId处理，如封禁，禁止借书等
  
	bool Administrators::insert(char* tName,char* inf);
  //插入，向tName表插入条目，inf包括条目所有属性值，中间用英文逗号隔开，一层添加一个条目
  
	bool Administrators::drop(char* tName,char* key);
  //删除，删除tName表中主键（ID）值为key的条目
  
	bool Administrators::update(char* tName,char* inf);
  //更新，更新tName表中的某条目，inf的格式为："要更改的条目主属性\t该条目除主属性外的所有其他属性或前n个属性，以逗号隔开"

	bool Administrators::updateInf();
  //upfateInf函数的子类重载，功能与父类相同

# NormalUsers(普通用户)类
普通用户类public继承用户类

	NormalUsers::NormalUsers();
  //空构造函数
  
	NormalUsers::NormalUsers(char* acc/*帐号*/,char* pwd/*密码*/);
  //构造函数，内含登录功能
  
	bool NormalUsers::getArecord(vector<char*>& aRecord);
  //返回该用户预约记录在aRecord中
  
	bool NormalUsers::getBrecord(vector<char*>& bRecord);
  //返回该用户借阅记录在bRecord中
  
	bool NormalUsers::getRrecord(vector<char*>& rRecord);
  //返回该用户荐购记录在rRecord中

	char* NormalUsers::getName();
  //返回用户昵称
  
	int NormalUsers::getBsurplus();
  //返回该用户借阅剩余量
  
	int getAsurplus();
  //返回该用户预约剩余量
	
	bool NormalUsers::borrowApply(char* bookId/*书籍ISBN码*/);
  //借阅申请
  
	bool NormalUsers::appointApply(char* bookId/*书籍ISBN码*/);
  //预约申请
  
	bool NormalUsers::retnApply(char* bookId);
  //还书申请;bookId是单本ID：ISBN-Bno
  
	bool NormalUsers::recommApply(char* inf/*荐购信息*/);
  //荐购申请
  
	bool NormalUsers::comment(char* cmt/*评论内容*/);
  //评论	

	bool updateInf();
  //父类函数重载，会被更新的属性增加：借阅剩余量，预约剩余量，用户昵称
