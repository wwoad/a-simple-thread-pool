#include"ThreadPool.h"
#include"Thread.h"
#include<iostream>
#include<mutex>
#include<string>


//����
Thread::Thread(DoTask DoTask)
	:do_task(DoTask)
{

}
//����
Thread::~Thread() 
{ }
//ִ������
void Thread::doTask()
{
	//�����߳�ִ������
	std::thread t(do_task);
	t.detach();
}