#include "ReadWriteLocker.h"

namespace lh
{

void ReadWriteLocker::readLock()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    // 检测是否有写入
    m_cv.wait(lock, [this]()
    {
        return !m_hasWrite;
    });

    ++m_sharedWriteCount;
}

void ReadWriteLocker::writeLock()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    // 检测是否有读取或有写入
    m_cv.wait(lock, [this]()
    {
        return m_sharedWriteCount == 0 && !m_hasWrite;
    });

    m_hasWrite = true;
}

void ReadWriteLocker::readUnLock()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    --m_sharedWriteCount;
    if(m_sharedWriteCount == 0)
    {
        m_cv.notify_all();
    }
}

void ReadWriteLocker::writeUnLock()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_hasWrite = false;
    m_cv.notify_all();
}

}