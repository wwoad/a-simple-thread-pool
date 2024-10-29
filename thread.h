#pragma once
#include<iostream>
#include<string>
#include<functional>


class Thread
{
public:
	using DoTask = std::function<void()>;
public:
	//构造
	Thread(DoTask DoTask);
	//析构
	~Thread();
	//执行任务
	void doTask();
private:
	DoTask do_task;
};