#include "Semaphore.h"

namespace lh
{

Semaphore::Semaphore(size_t n)
    : m_number(n)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::release(size_t n)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_number += n;
    m_cv.notify_all();
}

void Semaphore::acquire(size_t n)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_number < n)
    {
        m_cv.wait(lock);
    }
    m_number -= n;
}

}