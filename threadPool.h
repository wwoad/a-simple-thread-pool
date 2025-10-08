#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <variant>
#include <vector>

using std::cout;
using std::endl;

class Task // virtual base class
{
public:
    virtual void run() = 0; // pure virtual function,and son class must rewrite it
    virtual ~Task() = default;
};

class ThreadPool
{
public:
    ThreadPool(int minThread = 4, int maxThread = 12);
    ~ThreadPool();

    // sync addTask
    void addTask(std::shared_ptr<Task> sp);
    void addTask(std::function<void(void)> func);

    // async addTask
    template <typename F, typename... Args>
    auto addTask(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using returnType = typename std::result_of<F(Args...)>::type;
        // 1 创建package_task
        auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        // 2 得到futures
        std::future<returnType> res = task->get_future();
        // 3 任务函数添加到任务队列
        {
            std::lock_guard<std::mutex> locker(m_queMutex);
            m_taskQue.emplace([task]()
                              { (*task)(); });
        }
        m_condition.notify_one();

        return res;
    }
    void stop();
    int getThreadNum(){return m_maxThread;}

    ThreadPool() = delete;
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

private:
    void manager(void);
    void worker(void);

private:
    std::unique_ptr<std::thread> m_manager;
    std::map<std::thread::id, std::thread> m_worker;
    std::vector<std::thread::id> m_tid; // thread id vector,used to record the thread id

    std::queue<std::function<void(void)>> m_taskQue;

    std::mutex m_queMutex;
    std::mutex m_tidMutex;
    std::condition_variable m_condition;

    std::atomic<int> m_minThread;
    std::atomic<int> m_maxThread;

    std::atomic<bool> m_stop;

    std::atomic<int> m_idleThread;
    std::atomic<int> m_curThread;
    std::atomic<int> m_exitThread;
};

#endif // THREAD_POOL_H
