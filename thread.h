#pragma once
#include<iostream>
#include<string>
#include<functional>


class Thread
{
public:
	using DoTask = std::function<void()>;
public:
	//����
	Thread(DoTask DoTask);
	//����
	~Thread();
	//ִ������
	void doTask();
private:
	DoTask do_task;
};