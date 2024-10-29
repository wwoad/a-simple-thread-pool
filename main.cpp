#include <iostream>
#include <memory>
#include "threadpool.h" 

class Task1 :public Task
{
public:
	Task1(){ }
	~Task1(){ }
	virtual void runTask();
private:
};

void Task1::runTask()
{
	std::cout << " start task " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << " end task " << std::endl;
}
int main()
{
	ThreadPool tp;
	tp.runThreadPool(4);
	tp.commitTask(std::make_shared<Task1>());
	tp.commitTask(std::make_shared<Task1>());
	tp.commitTask(std::make_shared<Task1>());
	tp.commitTask(std::make_shared<Task1>());
	tp.commitTask(std::make_shared<Task1>());
	tp.commitTask(std::make_shared<Task1>());
	tp.commitTask(std::make_shared<Task1>());
	tp.commitTask(std::make_shared<Task1>());
	system("puase");
	return 0;
}