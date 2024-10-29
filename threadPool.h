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
	//���캯��
	ThreadPool();
	//�����б�
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	//����
	~ThreadPool();

	//���ܺ���
	//�����̳߳�
	void runThreadPool(int ThreadSize = 4);
	//��������ӿ�
	void commitTask(std::shared_ptr<Task> Ptask);
private:
	//ִ������
	void doTask();

private:
	//�̳߳�
	std::vector<std::unique_ptr<Thread>>  vector_thread;
	//�������
	std::queue<std::shared_ptr<Task>>  queue_task;
	//�߳�����
	int thread_size;
	//������д�С
	int queue_task_size;
	//�����������
	int tasks_max_size;
	//��������
	std::condition_variable que_not_full;
	std::condition_variable que_not_empty;
	//������
	std::mutex mtx;
};
