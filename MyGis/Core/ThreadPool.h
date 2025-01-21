#pragma once

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "MyGis/Core/Semaphore.h"

namespace lh
{

class ThreadPool
{
public:
    static ThreadPool* globalInstance();

    explicit ThreadPool(int maxThreadCount = std::thread::hardware_concurrency());
    ~ThreadPool();

    void start(int maxThreadCount = std::thread::hardware_concurrency());
    void stop();

    template <typename Function, typename ... Args>
    auto submit(Function&& func, Args&& ... args) -> std::future<decltype(func(args...))>
    {
        using RetType = decltype(func(args...));

        auto task = std::make_shared< std::packaged_task<RetType()> >(std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
        auto future = task->get_future();

        {
            std::unique_lock<std::mutex> locker(m_mutex);
            if(m_stop)
            {
                throw std::runtime_error("Cannot submit task to stopped ThreadPool");
            }

            m_tasks.emplace([task]()
            {
                (*task)();
            });
        }

        m_semaphore.release();

        return future;
    }

private:
    void workThread();

private:
    Semaphore m_semaphore;
    std::mutex m_mutex;

    bool m_stop;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;

    static std::mutex m_globalMutex;
    static ThreadPool* m_globalInstance;
};

}