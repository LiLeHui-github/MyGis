#include "Semaphore.h"

namespace lh
{

Semaphore::Semaphore(int n)
    : m_number(n)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::release(int n)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_number += n;
    m_cv.notify_all();
}

void Semaphore::acquire(int n)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_number < n)
    {
        m_cv.wait(lock);
    }
    m_number -= n;
}

}