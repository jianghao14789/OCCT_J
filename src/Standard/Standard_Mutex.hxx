// Created on: 2005-04-10
// Created by: Andrey BETENEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Standard_Mutex_HeaderFile
#define _Standard_Mutex_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_ErrorHandler.hxx>
#include <NCollection_Shared.hxx>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#endif

/**
 * @brief Mutex: a class to synchronize access to shared data.
 *
 * 互斥量（Mutex）：用于同步对共享数据的访问的类
 *
 * 这是一个简单的封装，包装了操作系统提供的工具
 * 用于从同一进程中的多个线程同步访问共享数据
 *
 * 当前实现非常简单明了；
 * 在 UNIX/Linux 上就是 POSIX pthread 库的包装，
 * 在 Windows NT 上是 CRITICAL_SECTION 的包装。
 * 它不提供任何高级功能，如从同一线程内递归调用同一互斥量
 * （这样的调用会冻结执行）。
 *
 * 注意该类的所有方法都是内联的，以保持最高性能。
 * 这意味着使用互斥量的库可能需要直接链接到线程库。
 *
 * 该类的典型使用方式如下：
 * - 在全局作用域创建 Standard_Mutex 类的实例
 *   （尽可能，或作为你的类的字段）
 * - 进入临界区时，使用该互斥量创建 Standard_Mutex::Sentry 类的实例
 *
 * 注意该类提供一个特定于 Open CASCADE 的功能：
 * 当信号被抬起并转换为 OCCT 异常时安全地解锁互斥量
 * （注意当前在 UNIX 和 Linux 上的实现，C longjumps 被用于此目的，
 * 因此类的析构函数不会自动被调用）。
 *
 * 要使用此功能，在 Lock() 或成功的 TryLock() 后调用 RegisterCallback()，
 * 在 Unlock() 之前调用 UnregisterCallback()（或使用 Sentry 类）。
 */

class Standard_Mutex : public Standard_ErrorHandler::Callback {
public:
    /**
     * @brief Simple sentry class providing convenient interface to mutex.
     *
     * 简单的哨兵类，为互斥量提供便利的接口
     *
     * 在其构造函数和析构函数中提供自动锁定和解锁互斥量，
     * 从而确保即使从保护的代码中抬起异常或信号，
     * 互斥量也能正确解锁。
     *
     * 进入临界区时创建该类的实例。
     * 这是一个 RAII（资源获取即初始化）模式的实现。
     */
    class Sentry {
    public:
        //! Constructor - initializes the sentry object by reference to a
        //! mutex (which must be initialized) and locks the mutex immediately
        //! 构造函数 - 通过对互斥量的引用初始化哨兵对象，并立即锁定互斥量
        Sentry(Standard_Mutex& theMutex) : myMutex(&theMutex) {
            Lock();
        }

        //! Constructor - initializes the sentry object by pointer to a
        //! mutex and locks the mutex if its pointer is not NULL
        //! 构造函数 - 通过互斥量指针初始化哨兵对象
        //! 如果指针不为 NULL，则锁定互斥量
        Sentry(Standard_Mutex* theMutex) : myMutex(theMutex) {
            if (myMutex != NULL) {
                Lock();
            }
        }
        //! Destructor - unlocks the mutex if already locked.
        //! 析构函数 - 如果已锁定，则解锁互斥量
        //! 即使在异常的情况下也会被调用，确保互斥量总是被释放
        ~Sentry() {
            if (myMutex != NULL) {
                Unlock();
            }
        }

    private:
        //! Lock the mutex
        //! 锁定互斥量
        void Lock() {
            myMutex->Lock();
            myMutex->RegisterCallback();
        }

        //! Unlock the mutex
        //! 解锁互斥量
        void Unlock() {
            myMutex->UnregisterCallback();
            myMutex->Unlock();
        }

        //! This method should not be called (prohibited).
        //! 此方法不应被调用（被禁止）- 禁止拷贝
        Sentry(const Sentry&);
        //! This method should not be called (prohibited).
        //! 此方法不应被调用（被禁止） - 禁止赋值
        Sentry& operator=(const Sentry&);

    private:
        Standard_Mutex* myMutex; //!< 指向互斥量的指针
    };

public:
    //! Constructor: creates a mutex object and initializes it.
    //! It is strongly recommended that mutexes were created as
    //! static objects whenever possible.
    //! 构造函数：创建并初始化互斥量对象
    //! 强烈建议尽可能将互斥量创建为静态对象
    Standard_EXPORT Standard_Mutex();

    //! Destructor: destroys the mutex object
    //! 析构函数：销毁互斥量对象
    Standard_EXPORT ~Standard_Mutex();

    //! Method to lock the mutex; waits until the mutex is released
    //! by other threads, locks it and then returns
    //! 锁定互斥量的方法；等待直到互斥量被其他线程释放
    //! 然后锁定它并返回（阻塞调用）
    Standard_EXPORT void Lock();

    //! Method to test the mutex; if the mutex is not hold by other thread,
    //! locks it and returns True; otherwise returns False without waiting
    //! mutex to be released.
    //! 测试互斥量的方法；如果互斥量未被其他线程持有，
    //! 则锁定它并返回 True；否则立即返回 False 而不等待
    //! 这是一个非阻塞的尝试获取互斥量
    Standard_EXPORT Standard_Boolean TryLock();

    //! Method to unlock the mutex; releases it to other users
    //! 解锁互斥量的方法；将其释放给其他用户
    //! 必须成对调用 Lock() 或成功的 TryLock()
    void Unlock();

private:
    //! Callback method to unlock the mutex if OCC exception or signal is raised
    //! 回调方法：如果抬起 OCC 异常或信号，则解锁互斥量
    //! 这是 OCCT 特有的功能，确保异常安全
    Standard_EXPORT virtual void DestroyCallback() Standard_OVERRIDE;

    //! This method should not be called (prohibited).
    //! 此方法不应被调用（被禁止）- 禁止拷贝
    Standard_Mutex(const Standard_Mutex&);
    //! This method should not be called (prohibited).
    //! 此方法不应被调用（被禁止）- 禁止赋值
    Standard_Mutex& operator=(const Standard_Mutex&);

private:
    // 互斥量的平台相关实现
    // Windows 使用 CRITICAL_SECTION，其他平台使用 pthread_mutex_t
#if (defined(_WIN32) || defined(__WIN32__))
    CRITICAL_SECTION myMutex; //!< Windows 互斥量
#else
    pthread_mutex_t myMutex; //!< POSIX 线程互斥量
#endif
};

//! 指向 Standard_Mutex 的共享指针（通过引用计数管理）
typedef NCollection_Shared<Standard_Mutex> Standard_HMutex;

// Implementation of the method Unlock is inline, since it is
// just a shortcut to system function
// Unlock 方法的实现是内联的，因为它只是系统函数的简单包装
inline void Standard_Mutex::Unlock() {
#if (defined(_WIN32) || defined(__WIN32__))
    LeaveCriticalSection(&myMutex); //!< Windows 释放临界区
#else
    pthread_mutex_unlock(&myMutex); //!< POSIX 线程解锁
#endif
}

#endif /* _Standard_Mutex_HeaderFile */
