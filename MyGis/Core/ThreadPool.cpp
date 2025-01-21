#include "ThreadPool.h"

namespace lh
{

ThreadPool* ThreadPool::globalInstance()
{
    m_globalMutex.lock();
    if(m_globalInstance == nullptr)
    {
        m_globalInstance = new ThreadPool();
    }
    m_globalMutex.unlock();

    return m_globalInstance;
}

ThreadPool::ThreadPool(int maxThreadCount)
    : m_stop(false)
{
    start(maxThreadCount);
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start(int maxThreadCount)
{
    stop();

    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_stop = false;
    }

    for(int i = 0; i < maxThreadCount; ++i)
    {
        m_threads.emplace_back(&ThreadPool::workThread, this);
    }
}

void ThreadPool::stop()
{
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_stop = true;
        std::queue<std::function<void()>>().swap(m_tasks);
    }

    m_semaphore.release(m_threads.size());

    for(auto& thread : m_threads)
    {
        if(thread.joinable())
        {
            thread.join();
        }
    }
    m_threads.clear();
}

void ThreadPool::workThread()
{
    while(true)
    {
        m_semaphore.acquire();

        std::function<void()> task;

        {
            std::unique_lock<std::mutex> locker(m_mutex);
            if(m_stop)
            {
                break;
            }

            if(m_tasks.empty())
            {
                // 信号量应该与任务数量相同, 跑到这里表示信号量出现问题
                continue;
            }

            task = std::move(m_tasks.front());
            m_tasks.pop();
        }

        try
        {
            task();
        }
        catch (...)
        {
        }
    }
}

std::mutex ThreadPool::m_globalMutex;
ThreadPool* ThreadPool::m_globalInstance = nullptr;

}