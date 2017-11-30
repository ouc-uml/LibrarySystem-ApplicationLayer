#include"error.h"
using namespace std;

extern Er Error;

bool s_connecting();
bool s_sending(int rqt,char* token,char* msg);
bool s_recving(char* buff);
void s_close();
