// Created on: 2007-09-04
// Created by: Andrey BETENEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

//! @file
//! Implementation of some atomic operations (elementary operations
//! with data that cannot be interrupted by parallel threads in the
//! multithread process) on various platforms
//!
//! By the moment, only operations necessary for reference counter
//! in Standard_Transient objects are implemented.
//!
//! This is preferred to use fixed size types "int32_t" / "int64_t" for
//! correct function declarations however we leave "int" assuming it is 32bits for now.
//
// 中文说明：
// 文件用途：原子操作的跨平台实现
//
// 原子操作（Atomic Operations）：
// • 不能被多线程中断的基本操作
// • 在多线程环境中保证数据的一致性和完整性
// • 不需要加锁（mutex）就能保证线程安全
//
// 当前实现的原子操作：
// • 原子自增（Atomic Increment）
// • 原子自减（Atomic Decrement）
// • 原子比较交换（Atomic Compare and Swap, CAS）
//
// 主要用途：
// • 管理 Standard_Transient 对象的引用计数
// • 确保引用计数在多线程环境下的正确性
//
// 平台支持：
// • GCC（Linux、Unix）
// • Windows（MSVC、Intel Compiler）
// • macOS（Apple）
// • Android
// • 其他平台（有降级实现）
//
// 注意：
// • 使用 int 类型（假设为 32 位），而不是 int32_t
// • 涉及 volatile 指针的操作（防止编译器优化）
// • 不同平台有不同的底层实现机制

#ifndef _Standard_Atomic_HeaderFile
#define _Standard_Atomic_HeaderFile

//! Increments atomically integer variable pointed by theValue
//! and returns resulting incremented value.
//
// 中文说明：
// 原子自增操作
//
// 功能：
// • 以原子方式对整数变量进行 +1 操作
// • 操作过程不会被其他线程中断
// • 立即返回自增后的值
//
// 参数：
// • theValue：指向要自增的整数变量的指针
//            必须使用 volatile 修饰，禁止编译器优化
//
// 返回值：
// • 自增后的值（即原值 + 1）
//
// 应用场景：
// • 引用计数器递增
// • 其他需要原子操作的计数器
//
// 线程安全性：
// • 完全线程安全，无需额外加锁
// • 即使多个线程同时调用也不会产生数据竞争
//
// 示例：
//   volatile int refCount = 0;
//   int newValue = Standard_Atomic_Increment(&refCount);  // 返回 1
inline int Standard_Atomic_Increment(volatile int* theValue);

//! Decrements atomically integer variable pointed by theValue
//! and returns resulting decremented value.
//
// 中文说明：
// 原子自减操作
//
// 功能：
// • 以原子方式对整数变量进行 -1 操作
// • 操作过程不会被其他线程中断
// • 立即返回自减后的值
//
// 参数：
// • theValue：指向要自减的整数变量的指针
//            必须使用 volatile 修饰，禁止编译器优化
//
// 返回值：
// • 自减后的值（即原值 - 1）
//
// 应用场景：
// • 引用计数器递减
// • 释放资源时的计数器减少
// • 其他需要原子操作的计数器
//
// 线程安全性：
// • 完全线程安全，无需额外加锁
// • 即使多个线程同时调用也不会产生数据竞争
//
// 示例：
//   volatile int refCount = 10;
//   int newValue = Standard_Atomic_Decrement(&refCount);  // 返回 9
inline int Standard_Atomic_Decrement(volatile int* theValue);

//! Perform an atomic compare and swap.
//! That is, if the current value of *theValue is theOldValue, then write theNewValue into *theValue.
//! @param theValue    pointer to variable to modify
//! @param theOldValue expected value to perform modification
//! @param theNewValue new value to set in case if *theValue was equal to theOldValue
//! @return TRUE if theNewValue has been set to *theValue
//
// 中文说明：
// 原子比较交换操作（Compare And Swap, CAS）
//
// 功能：
// • 这是最重要的原子操作，很多其他原子操作都基于它实现
// • 如果当前值等于期望值，则修改为新值
// • 整个过程是原子的，不会被中断
//
// 参数：
// • theValue：指向要修改的变量的指针
//
// • theOldValue：期望的当前值（旧值）
//               如果 *theValue == theOldValue，才执行交换
//
// • theNewValue：新值
//               如果条件满足，将 *theValue 设置为此值
//
// 返回值：
// • TRUE：交换成功（说明 *theValue 确实等于 theOldValue，且已更新为 theNewValue）
// • FALSE：交换失败（说明 *theValue 不等于 theOldValue，未进行修改）
//
// 伪代码逻辑：
//   if (*theValue == theOldValue) {
//       *theValue = theNewValue;
//       return true;
//   }
//   return false;
//
// 应用场景：
// • 无锁算法的基础
// • 自旋锁（Spin Lock）的实现
// • 引用计数器的原子更新
// • 状态机的原子转换
//
// 使用示例（引用计数递减）：
//   volatile int refCount = 2;
//   while (!Standard_Atomic_CompareAndSwap(&refCount, 2, 1)) {
//       // 如果失败（值被其他线程改变），重试
//   }
//   // 现在 refCount 的值已安全地从 2 更改为 1
//
// 为什么需要 CAS？
// • 简单的 if + 赋值不是原子的，两步之间可能被其他线程修改
// • CAS 将比较和交换合并为一个不可中断的操作
// • 确保"检查+修改"这两步是同步的
//
// 线程安全性：
// • 完全线程安全
// • 常用于实现无锁数据结构
inline bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue);

// Platform-dependent implementation
// 平台相关的实现 - 根据不同平台选择不同的底层实现

#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) || defined(__EMSCRIPTEN__)
// gcc explicitly defines the macros __GCC_HAVE_SYNC_COMPARE_AND_SWAP_*
// starting with version 4.4+, although built-in functions
// are available since 4.1.x. However unless __GCC_HAVE_SYNC_COMPARE_AND_SWAP_*
// are defined, linking may fail without specifying -march option when
// building for 32bit architecture on 64bit (using -m32 option). To avoid
// making -march mandatory, check for __GCC_HAVE_SYNC_COMPARE_AND_SWAP_* is
// enforced.

// 中文说明：
// GCC（Linux、Unix、Emscripten）平台的实现
//
// 编译器支持情况：
// • GCC 4.4+ 定义了 __GCC_HAVE_SYNC_COMPARE_AND_SWAP_* 宏
// • GCC 4.1+ 已包含 __sync_* 内置函数
// • 不过检查宏的定义是必要的，以避免在某些编译配置下链接失败
//
// 底层机制：
// • 使用 GCC 的内置原子操作函数（Built-in Functions）
// • 这些函数由编译器直接编译为 CPU 原子指令
// • 例如：x86 的 LOCK 前缀、ARM 的 LDREX/STREX 等
//
// GCC 内置函数说明：
// • __sync_add_and_fetch(ptr, value)：原子加法，返回新值
// • __sync_sub_and_fetch(ptr, value)：原子减法，返回新值
// • __sync_val_compare_and_swap(ptr, oldval, newval)：CAS 操作，返回旧值

int Standard_Atomic_Increment(volatile int* theValue)
{
    // __sync_add_and_fetch：原子地将 theValue 加 1，返回新值
    // 参数 1：操作数变量的指针
    // 参数 2：要加的值（这里是 1）
    // 返回值：加法后的值
    return __sync_add_and_fetch(theValue, 1);
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
    // __sync_sub_and_fetch：原子地将 theValue 减 1，返回新值
    // 参数 1：操作数变量的指针
    // 参数 2：要减的值（这里是 1）
    // 返回值：减法后的值
    return __sync_sub_and_fetch(theValue, 1);
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
    // __sync_val_compare_and_swap：CAS 操作
    // 参数 1：要修改的变量指针
    // 参数 2：期望的旧值
    // 参数 3：新值
    // 返回值：操作前的旧值
    //
    // 逻辑：
    // • 如果 *theValue == theOldValue，则设置 *theValue = theNewValue，返回 theOldValue
    // • 否则，不修改 *theValue，返回当前的值
    //
    // 我们通过比较返回值是否等于 theOldValue 来判断是否成功
    return __sync_val_compare_and_swap(theValue, theOldValue, theNewValue) == theOldValue;
}

#elif defined(_WIN32)
// 中文说明：
// Windows（MSVC、Intel 编译器）平台的实现
//
// Windows API：
// • 使用 Windows 提供的 Interlocked* 函数族
// • 这些是系统级的原子操作函数
// • 在内核级别实现，性能很高
//
// 函数说明：
// • _InterlockedIncrement(ptr)：原子自增，返回新值
// • _InterlockedDecrement(ptr)：原子自减，返回新值
// • _InterlockedCompareExchange(dst, exch, cmp)：CAS 操作

extern "C" {
    // 声明 Windows API 函数
    // volatile long* 而不是 int*，因为 Windows API 使用 long 类型

    //! _InterlockedIncrement - Windows 提供的原子自增函数
    //! 参数：指向 long 的指针
    //! 返回值：自增后的值
    long _InterlockedIncrement(volatile long* lpAddend);

    //! _InterlockedDecrement - Windows 提供的原子自减函数
    //! 参数：指向 long 的指针
    //! 返回值：自减后的值
    long _InterlockedDecrement(volatile long* lpAddend);

    //! _InterlockedCompareExchange - Windows 提供的 CAS 函数
    //! 参数 1：目标变量（long 指针）
    //! 参数 2：交换值（新值）
    //! 参数 3：比较值（旧值）
    //! 返回值：修改前的旧值
    long _InterlockedCompareExchange(long volatile* Destination, long Exchange, long Comparand);
}

#if defined(_MSC_VER) && ! defined(__INTEL_COMPILER)
// force intrinsic instead of WinAPI calls
// 中文说明：
// 指导 MSVC 编译器使用内置函数（Intrinsic）而不是真实的函数调用
//
// 优点：
// • 内置函数由编译器直接编译为 CPU 指令
// • 避免了函数调用开销（call/return）
// • 性能更高，代码更快
// • 编译器可以更好地优化
//
// 注意：
// • 仅对 MSVC 有效（不对 Intel Compiler 使用）
// • Intel Compiler 自己管理何时使用内置函数

#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)
#pragma intrinsic (_InterlockedCompareExchange)
#endif

// WinAPI function or MSVC intrinsic
// Note that we safely cast int* to long*, as they have same size and endian-ness
//
// 中文说明：
// 参数类型转换说明
//
// 为什么可以安全地将 int* 转换为 long*？
// • 在 Windows 32/64 位系统中，sizeof(int) == sizeof(long)
// • 两种类型占用相同的内存大小（通常都是 4 字节）
// • 字节序（Endianness）相同，都是小端（Little Endian）
// • 因此转换是安全的，不会造成数据损坏
//
// 为什么 Windows API 使用 long 而我们用 int？
// • Windows API 历史原因，使用 long 作为标准整数类型
// • OCCT 为了跨平台兼容性，使用 int 作为统一的整数类型
// • 通过指针转换来适配 Windows API

int Standard_Atomic_Increment(volatile int* theValue)
{
    // 将 int* 安全地转换为 long*，因为大小相同
    // 调用 Windows API 进行原子自增
    return _InterlockedIncrement(reinterpret_cast<volatile long*>(theValue));
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
    // 将 int* 安全地转换为 long*，因为大小相同
    // 调用 Windows API 进行原子自减
    return _InterlockedDecrement(reinterpret_cast<volatile long*>(theValue));
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
    // 将 int* 安全地转换为 long*，因为大小相同
    // _InterlockedCompareExchange 返回修改前的值
    // 我们判断返回值是否等于 theOldValue 来确定是否成功
    return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(theValue), theNewValue, theOldValue) == theOldValue;
}

#elif defined(__APPLE__)
// use atomic operations provided by MacOS
//
// 中文说明：
// macOS（Apple）平台的实现
//
// macOS 提供的原子操作库：
// • libkern/OSAtomic.h 提供了 Mac 特有的原子操作函数
// • 这些函数使用最优的 CPU 指令实现
// • 在 Mach 内核级别支持
//
// 相比其他平台的优点：
// • 专门为 Apple 硬件优化
// • 性能好，使用最高效的 CPU 指令
// • API 设计简洁明了

#include <libkern/OSAtomic.h>

// 中文说明：
// macOS 原子操作函数族：
// • OSAtomicIncrement32Barrier(ptr)：原子自增 32 位，带内存屏障
// • OSAtomicDecrement32Barrier(ptr)：原子自减 32 位，带内存屏障
// • OSAtomicCompareAndSwapInt(oldVal, newVal, ptr)：CAS 操作
//
// "Barrier" 后缀说明：
// • Barrier 意味着内存屏障（Memory Barrier）
// • 防止 CPU 乱序执行（Out-of-Order Execution）
// • 确保操作的可见性和顺序性
// • 在多核系统中非常重要

int Standard_Atomic_Increment(volatile int* theValue)
{
    // OSAtomicIncrement32Barrier：原子自增 32 位整数
    // 返回新值
    // Barrier 保证了操作的原子性和内存可见性
    return OSAtomicIncrement32Barrier(theValue);
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
    // OSAtomicDecrement32Barrier：原子自减 32 位整数
    // 返回新值
    // Barrier 保证了操作的原子性和内存可见性
    return OSAtomicDecrement32Barrier(theValue);
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
    // OSAtomicCompareAndSwapInt：CAS 操作
    // 参数 1：旧值（期望值）
    // 参数 2：新值
    // 参数 3：指向要修改变量的指针
    // 返回值：true 表示交换成功，false 表示失败
    return OSAtomicCompareAndSwapInt(theOldValue, theNewValue, theValue);
}

#elif defined(__ANDROID__)

// Atomic operations that were exported by the C library didn't
// provide any memory barriers, which created potential issues on
// multi-core devices. Starting from ndk version r7b they are defined as
// inlined calls to GCC sync builtins, which always provide a full barrier.
// It is strongly recommended to use newer versions of ndk.

// 中文说明：
// Android 平台的实现
//
// Android 原子操作的演变：
// • 早期：C 库提供的原子操作没有内存屏障
// • 问题：在多核设备上可能出现数据竞争
// • NDK r7b+：改为使用 GCC 内置函数
// • 现在：GCC 内置函数提供完整的内存屏障
//
// 建议：
// • 使用较新的 NDK 版本（至少 r7b 之后）
// • 这样才能获得正确的原子操作语义

#include <sys/atomics.h>

// 中文说明：
// Android 原子操作函数：
// • __atomic_inc(ptr)：原子自增，返回修改前的值（fetch_and_add 语义）
// • __atomic_dec(ptr)：原子自减，返回修改前的值（fetch_and_sub 语义）
// • __atomic_cmpxchg(oldVal, newVal, ptr)：CAS，成功返回 0，失败返回非 0
//
// 与其他平台的差异：
// • __atomic_inc 返回的是修改前的值，不是修改后的值
// • 我们需要加 1 来得到修改后的值
// • 同样，__atomic_dec 返回的是修改前的值，需要减 1

int Standard_Atomic_Increment(volatile int* theValue)
{
    // __atomic_inc 返回修改前的值
    // 例如：原值为 5，执行后返回 5，但变量已变为 6
    // 我们需要加 1 来获得修改后的值（6）
    return __atomic_inc(theValue) + 1; // analog of __sync_fetch_and_add
}

int Standard_Atomic_Decrement(volatile int* theValue)
{
    // __atomic_dec 返回修改前的值
    // 例如：原值为 5，执行后返回 5，但变量已变为 4
    // 我们需要减 1 来获得修改后的值（4）
    return __atomic_dec(theValue) - 1; // analog of __sync_fetch_and_sub
}

bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
    // __atomic_cmpxchg：CAS 操作
    // 成功时返回 0，失败时返回非 0
    // 我们比较返回值是否等于 0 来判断是否成功
    return __atomic_cmpxchg(theOldValue, theNewValue, theValue) == 0;
}

#else

// 中文说明：
// 其他平台或不支持原子操作的平台的后备实现
//
// 降级实现（Fallback Implementation）：
// • 对于不支持原子操作的平台
// • 使用普通的 C++ 操作替代
// • 注意：这些实现不是原子的！
// • 仅用于编译通过，在多线程环境下可能不安全
//
// 何时使用：
// • 平台不支持更好的原子操作时
// • 单线程环境下
// • IGNORE_NO_ATOMICS 定义时可以忽略警告

#ifndef IGNORE_NO_ATOMICS
// 如果没有定义 IGNORE_NO_ATOMICS，则产生编译错误
// 强制开发者意识到原子操作不可用
#error "Atomic operation isn't implemented for current platform!"
#endif

// 中文说明：
// 非原子的自增实现（仅作为最后的后备方案）
// 危险：在多线程环境下不安全！
int Standard_Atomic_Increment(volatile int* theValue)
{
    // 简单的自增操作，不具有原子性
    // 在多线程环境下可能出现数据竞争
    return ++(*theValue);
}

// 中文说明：
// 非原子的自减实现（仅作为最后的后备方案）
// 危险：在多线程环境下不安全！
int Standard_Atomic_Decrement(volatile int* theValue)
{
    // 简单的自减操作，不具有原子性
    // 在多线程环境下可能出现数据竞争
    return --(*theValue);
}

// 中文说明：
// 非原子的 CAS 实现（仅作为最后的后备方案）
// 危险：在多线程环境下不安全！
bool Standard_Atomic_CompareAndSwap(volatile int* theValue, int theOldValue, int theNewValue)
{
    // 这两步操作不是原子的：
    // 1. 比较 *theValue == theOldValue
    // 2. 如果相等，赋值 *theValue = theNewValue
    // 两步之间可能被其他线程修改，导致不一致
    if (*theValue == theOldValue)
    {
        *theValue = theNewValue;
        return true;
    }
    return false;
}

#endif
// 平台相关实现的结束

#endif //_Standard_Atomic_HeaderFile
