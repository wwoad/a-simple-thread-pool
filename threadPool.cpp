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

//析构
ThreadPool::~ThreadPool()
{
}

//功能函数
void ThreadPool::runThreadPool(int ThreadSize)//运行线程池
{
	this->thread_size = ThreadSize;

	for (int i = 0; i < ThreadSize; i++)
	{
		//创建线程对象，将执行任务函数绑定给线程对象
		auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::doTask, this));
		//将新建的线程对象插入线程容器
		this->vector_thread.emplace_back(std::move(ptr));
	}

	for (int i = 0; i < ThreadSize; i++)
	{
		//线程执行任务
		this->vector_thread[i]->doTask();
	}
}

void ThreadPool::doTask()//执行任务
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
			//执行任务
			task_ptr->runTask();
		}
	}
}

//提交任务
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