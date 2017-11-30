#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<iostream>
#include"md5.h"

using namespace std;

void ranString(char resl[]){
	int num,remain=40;
	//srand(time(0));
	while(remain--){
		num=rand()%62;//0-9:0-9;10-35:A-Z;36-61:a-z;
		if(num<10) resl[39-remain]=(char)(num+48);
		else if(num<36) resl[39-remain]=(char)(num+55);
		else resl[39-remain]=(char)(num+61);
	}
	resl[40]='\0';
}

