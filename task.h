#pragma once
#pragma once
#include<iostream>
#include<string>

class Task
{
public:
	//¹¹Ôì
	Task();
	//Îö¹¹
	virtual ~Task();
	virtual void runTask() = 0;
};