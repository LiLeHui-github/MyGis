#pragma once

#include <mutex>
#include <condition_variable>

namespace lh
{

class Semaphore
{
public:
    explicit Semaphore(size_t n = 0);
    ~Semaphore();

    void release(size_t n = 1);
    void acquire(size_t n = 1);

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    size_t m_number;
};

}