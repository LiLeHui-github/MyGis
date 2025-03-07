﻿#pragma once

#include <mutex>
#include <condition_variable>

namespace lh
{

class ReadWriteLocker
{
public:
    // 读锁, 读取共享, 有写锁阻塞等待解锁
    void readLock();
    // 写锁, 有读锁阻塞等待解锁
    void writeLock();

    void readUnLock();
    void writeUnLock();

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;

    // 读取共享数量
    int m_sharedWriteCount = 0;

    // 是否有写入
    bool m_hasWrite = false;
};

class ReadGuard
{
public:
    explicit ReadGuard(ReadWriteLocker& lock)
        : m_lock(lock)
    {
        m_lock.readLock();
    }

    ~ReadGuard()
    {
        m_lock.readUnLock();
    }

    ReadGuard(const ReadGuard&) = delete;
    ReadGuard& operator=(const ReadGuard&) = delete;

private:
    ReadWriteLocker& m_lock;
};

class WriteGuard
{
public:
    explicit WriteGuard(ReadWriteLocker& lock)
        : m_lock(lock)
    {
        m_lock.writeLock();
    }

    ~WriteGuard()
    {
        m_lock.writeUnLock();
    }

    WriteGuard(const WriteGuard&) = delete;
    WriteGuard& operator=(const WriteGuard&) = delete;

private:
    ReadWriteLocker& m_lock;
};

}