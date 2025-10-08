
#include "threadpool.h"

ThreadPool::ThreadPool(int minThread, int maxThread)
    : m_minThread(minThread), m_maxThread(maxThread), m_stop(false), m_idleThread(minThread), m_curThread(minThread), m_exitThread(0)
{
    // create manager thread
    m_manager = std::make_unique<std::thread>(&ThreadPool::manager, this);
    // craete work thread
    for (int i = 0; i < minThread; i++)
    {
        std::thread t(&ThreadPool::worker, this);
        m_worker.insert(std::make_pair(t.get_id(), std::move(t))); // add work thread input work map
    }
}

ThreadPool::~ThreadPool()
{
    m_stop = true;
    m_condition.notify_all();

    for (auto &it : m_worker)
    {
        if (it.second.joinable())
        {
            it.second.join();
            std::cout << " -------线程: " << it.first << "已退出--------" << endl;
        }
    }

    if (m_manager->joinable())
    {
        std::cout << " -------管理线程: " << m_manager->get_id() << "已退出--------" << endl;
        m_manager->join();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ThreadPool::addTask(std::shared_ptr<Task> sp)
{
    std::function<void()> task = [sp]()
    { sp->run(); };

    {
        std::lock_guard<std::mutex> locker(m_queMutex);
        m_taskQue.emplace(task);
    }
    m_condition.notify_one();
}

void ThreadPool::addTask(std::function<void(void)> func)
{
    {
        std::lock_guard<std::mutex> locker(m_queMutex);
        m_taskQue.emplace(func);
    }
    m_condition.notify_one();
}

void ThreadPool::stop()
{
    m_stop.store(true);
}

void ThreadPool::manager(void)
{
    while (!m_stop.load())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        int idle = m_idleThread;
        int cur = m_curThread;

        if (idle > (cur / 2) && cur > m_minThread)
        {
            m_exitThread.store(2);
            m_condition.notify_all();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            {
                std::lock_guard<std::mutex> locker(m_tidMutex);
                for (auto id : m_tid)
                {
                    auto it = m_worker.find(id);
                    if (it != m_worker.end())
                    {
                        std::cout << "------线程" << (*it).first << "被销毁" << endl;
                        (*it).second.join();
                        m_worker.erase(it);
                    }
                }
                m_tid.clear();
            }
        }
        else if (idle == 0 && cur < m_maxThread)
        {
            std::thread t(&ThreadPool::worker, this);
            m_worker.insert(std::make_pair(t.get_id(), std::move(t))); // add work thread input work map
            std::cout << "-------创建一个线程--------" << endl;
            m_curThread++;
            m_idleThread++;
        }
    }
}

void ThreadPool::worker(void)
{
    while (!m_stop.load())
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> locker(m_queMutex);

            while (m_taskQue.empty() && !m_stop.load())
            {
                m_condition.wait(locker);
                if (m_exitThread.load() > 0)
                {
                    m_curThread--;
                    m_idleThread--;
                    m_exitThread--;
                    std::cout << "-------线程退出,ID:" << std::this_thread::get_id() << endl;
                    std::lock_guard<std::mutex> lck(m_tidMutex);
                    m_tid.emplace_back(std::this_thread::get_id());
                    return;
                }
            }
            if (!m_taskQue.empty())
            {
                task = std::move(m_taskQue.front());
                m_taskQue.pop();
            }
        }
        if (task)
        {
            m_idleThread--;
            task();
            m_idleThread++;
        }
    }
}
