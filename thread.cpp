#include"ThreadPool.h"
#include"Thread.h"
#include<iostream>
#include<mutex>
#include<string>


//构造
Thread::Thread(DoTask DoTask)
	:do_task(DoTask)
{

}
//析构
Thread::~Thread() 
{ }
//执行任务
void Thread::doTask()
{
	//创建线程执行任务
	std::thread t(do_task);
	t.detach();
}