#pragma once

#include <mutex>
#include <condition_variable>

class Semaphore
{
public:
    explicit Semaphore(int n = 0);
    ~Semaphore();

    void release(int n = 1);
    void acquire(int n = 1);

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    int m_number;
};
