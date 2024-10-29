#include"ThreadPool.h"
#include"Thread.h"
#include<iostream>
#include<mutex>
#include<string>
#include<functional>

const int TASK_MAX_SIZE  = 20;

ThreadPool::ThreadPool()
	:tasks_max_size(TASK_MAX_SIZE)
	,queue_task_size(0)
	,thread_size(4)
{ }

//����
ThreadPool::~ThreadPool()
{
}

//���ܺ���
void ThreadPool::runThreadPool(int ThreadSize)//�����̳߳�
{
	this->thread_size = ThreadSize;

	for (int i = 0; i < ThreadSize; i++)
	{
		//�����̶߳��󣬽�ִ���������󶨸��̶߳���
		auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::doTask, this));
		//���½����̶߳�������߳�����
		this->vector_thread.emplace_back(std::move(ptr));
	}

	for (int i = 0; i < ThreadSize; i++)
	{
		//�߳�ִ������
		this->vector_thread[i]->doTask();
	}
}

void ThreadPool::doTask()//ִ������
{
	for (;;)
	{
		std::shared_ptr<Task> task_ptr;
		
		{
			std::unique_lock<std::mutex> lock(this->mtx);
			this->que_not_empty.wait(lock,[&]()->bool {return queue_task.size() > 0; });
			task_ptr = std::move(queue_task.front());
			this->queue_task.pop();
			this->queue_task_size--;
			this->que_not_empty.notify_all();
		}
		if (task_ptr != nullptr)
		{
			//ִ������
			task_ptr->runTask();
		}
	}
}

//�ύ����
void ThreadPool::commitTask(std::shared_ptr<Task> Ptask)
{
	
	std::unique_lock<std::mutex> lock(this->mtx);

	if (!this->que_not_full.wait_for(lock, std::chrono::seconds(1),
		[this]() ->bool{return queue_task.size() <  tasks_max_size; }))
	{
		std::cout <<"the task commit fail" << std::endl;
		return;
	}

	queue_task.emplace(Ptask);
	this->queue_task_size++;
	std::cout << " task conmmit successful " << std::endl;
	this->que_not_empty.notify_all();
}