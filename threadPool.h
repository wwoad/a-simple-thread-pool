#pragma once
#include<iostream>
#include<string>
#include<memory>
#include<condition_variable>
#include<vector>
#include<queue>
#include"Thread.h"
#include"Task.h"


class ThreadPool
{
public:
	//构造函数
	ThreadPool();
	//禁用列表
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	//析构
	~ThreadPool();

	//功能函数
	//运行线程池
	void runThreadPool(int ThreadSize = 4);
	//放入任务接口
	void commitTask(std::shared_ptr<Task> Ptask);
private:
	//执行任务
	void doTask();

private:
	//线程池
	std::vector<std::unique_ptr<Thread>>  vector_thread;
	//任务队列
	std::queue<std::shared_ptr<Task>>  queue_task;
	//线程数量
	int thread_size;
	//任务队列大小
	int queue_task_size;
	//任务最大容量
	int tasks_max_size;
	//条件变量
	std::condition_variable que_not_full;
	std::condition_variable que_not_empty;
	//互斥锁
	std::mutex mtx;
};
