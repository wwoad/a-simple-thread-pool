#pragma once
#pragma once
#include<iostream>
#include<string>

class Task
{
public:
	//����
	Task();
	//����
	virtual ~Task();
	virtual void runTask() = 0;
};