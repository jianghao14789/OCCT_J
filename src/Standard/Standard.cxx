// Created on: 2005-03-15
// Created by: Peter KURNEV
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// 中文说明：
// 文件用途：Standard 名字空间的实现
// 内容：
// • 内存管理工厂的实现（工厂模式）
// • 平台相关的内存分配函数（MSVC、GCC、POSIX）
// • 全局的 Allocate、Free、Reallocate 等函数
// • 对齐内存分配（AllocateAligned、FreeAligned）

#include <Standard.hxx>
#include <Standard_MMgrOpt.hxx>      // OCCT 优化的内存管理器
#include <Standard_MMgrRaw.hxx>      // 系统原始内存管理器
#include <Standard_MMgrTBBalloc.hxx> // Intel TBB 内存管理器
#include <Standard_Assert.hxx>

#include <stdlib.h>
#if (defined(_WIN32) || defined(__WIN32__))
#include <windows.h>
#include <malloc.h>
#include <locale.h>
#endif

#if defined(_MSC_VER) || defined(__ANDROID__) || defined(__QNX__)
#include <malloc.h>
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) &&                                \
       (defined(__i386) || defined(__x86_64)))
#include <mm_malloc.h>
#else
// POSIX 标准的对齐内存分配函数
extern "C" int posix_memalign(void** thePtr, size_t theAlign, size_t theSize);
#endif

// There is no support for environment variables in UWP
// OSD_Environment could not be used here because of cyclic dependency
// UWP（Universal Windows Platform）不支持环境变量
#ifdef OCCT_UWP
#define getenv(x) NULL
#endif

#ifndef OCCT_MMGT_OPT_DEFAULT
// 默认内存管理器类型：0=系统分配器
#define OCCT_MMGT_OPT_DEFAULT 0
#endif

//=======================================================================
// class    : Standard_MMgrFactory
// purpose  : Container for pointer to memory manager;
//           used to construct appropriate memory manager according
//           to environment settings, and to ensure destruction upon exit
//
// 中文说明：
// 内存管理工厂类
// 功能：
// • 根据环境变量选择合适的内存管理器
// • 保证内存管理器的唯一性（单例模式）
// • 在程序退出时正确清理资源
//
// 设计模式：
// • 工厂模式：根据环境变量创建不同类型的内存管理器
// • 单例模式：确保全局只有一个内存管理器实例
// • 懒初始化：第一次使用时才创建（GetMMgr 函数内）
//=======================================================================
class Standard_MMgrFactory {
public:
    //! 获取全局内存管理器实例
    static Standard_MMgrRoot* GetMMgr();

    //! 析构函数 - 清理资源
    ~Standard_MMgrFactory();

private:
    //! 构造函数 - 初始化内存管理器（受保护）
    Standard_MMgrFactory();

    //! 拷贝构造函数 - 禁止
    Standard_MMgrFactory(const Standard_MMgrFactory&);

    //! 赋值操作符 - 禁止
    Standard_MMgrFactory& operator=(const Standard_MMgrFactory&);

private:
    //! 指向内存管理器对象的指针
    Standard_MMgrRoot* myFMMgr;
};

//=======================================================================
// function : Standard_MMgrFactory
// purpose  : Check environment variables and create appropriate memory manager
//
// 中文说明：
// 构造函数 - 根据环境变量初始化内存管理器
//
// 环境变量说明：
// • MMGT_OPT：选择内存管理器类型
//   - 0（默认）：Standard_MMgrRaw - 系统原始分配器
//   - 1：Standard_MMgrOpt - OCCT 优化分配器
//   - 2：Standard_MMgrTBBalloc - Intel TBB 分配器
//
// • MMGT_CLEAR：是否清空释放的内存（true/false）
//
// • MMGT_MMAP：MMgrOpt 使用内存映射文件（true/false）
// • MMGT_CELLSIZE：MMgrOpt 的单元大小（字节）
// • MMGT_NBPAGES：MMgrOpt 初始页数
// • MMGT_THRESHOLD：MMgrOpt 的阈值
//
// • MMGT_LFH：Windows 低碎片堆（true/false）
//
// 初始化步骤：
// 1. 检查静态断言（确保基本类型大小正确）
// 2. 读取环境变量
// 3. CPU 特性检测（SSE2 支持）
// 4. Windows LFH 配置
// 5. 创建相应的内存管理器
//=======================================================================

Standard_MMgrFactory::Standard_MMgrFactory() : myFMMgr(NULL) {
    /*#if defined(_MSC_VER) && (_MSC_VER > 1400)
      // Turn ON thread-safe C locale globally to avoid side effects by setlocale() calls between threads.
      // After this call all following _configthreadlocale() will be ignored assuming
      // Notice that this is MSVCRT feature - on POSIX systems xlocale API (uselocale instead of setlocale)
      // should be used explicitly to ensure thread-safety!

      // This is not well documented call because _ENABLE_PER_THREAD_LOCALE_GLOBAL flag is defined but not implemented
    for some reason.
      // -1 will set global locale flag to force _ENABLE_PER_THREAD_LOCALE_GLOBAL + _ENABLE_PER_THREAD_LOCALE_NEW
    behaviour
      // although there NO way to turn it off again and following calls will have no effect (locale will be changed only
    for current thread). _configthreadlocale (-1); #endif*/

    // Check basic assumption.
    // If assertion happens, then OCCT should be corrected for compatibility with such CPU architecture.
    // 检查基本假设（确保类型大小正确）
    // 这些检查在编译时执行，确保 OCCT 与当前 CPU 架构兼容
    Standard_STATIC_ASSERT(sizeof(Standard_Utf8Char) == 1);  // UTF-8 字符占 1 字节
    Standard_STATIC_ASSERT(sizeof(short) == 2);              // short 占 2 字节
    Standard_STATIC_ASSERT(sizeof(Standard_Utf16Char) == 2); // UTF-16 字符占 2 字节
    Standard_STATIC_ASSERT(sizeof(Standard_Utf32Char) == 4); // UTF-32 字符占 4 字节
#ifdef _WIN32
    Standard_STATIC_ASSERT(sizeof(Standard_WideChar) == sizeof(Standard_Utf16Char)); // Windows 宽字符
#endif

    // 读取 MMGT_OPT 环境变量，选择内存管理器类型
    char* aVar;
    aVar = getenv("MMGT_OPT");
    // 如果环境变量未设置，使用默认值（通常为 0）
    Standard_Integer anAllocId = (aVar ? atoi(aVar) : OCCT_MMGT_OPT_DEFAULT);

    // TBB 分配器在 32 位 Windows 上的 SSE2 支持检测
    // CR25396: 检查 32 位 x86 处理器是否支持 SSE2 指令
    // 某些 AMD Sempron 变体支持 SSE 但不支持 SSE2，使用 TBB 会导致运行时崩溃
    // 因此需要在运行时检测，如果不支持则降级为 MMgrRaw
#if defined(HAVE_TBB) && defined(_M_IX86)
    if (anAllocId == 2) {
        // CR25396: Check if SSE2 instructions are supported on 32-bit x86 processor on Windows platform,
        // if not then use MMgrRaw instead of MMgrTBBalloc.
        // It is to avoid runtime crash when running on a CPU
        // that supports SSE but does not support SSE2 (some modifications of AMD Sempron).
        static const DWORD _SSE2_FEATURE_BIT(0x04000000); // SSE2 特性位掩码
        DWORD volatile dwFeature;
        _asm
        {
            push eax
            push ebx
            push ecx
            push edx

             // get the CPU feature bits
             // 执行 CPUID 指令，读取 CPU 特性位
            mov eax, 1
            cpuid
            mov dwFeature, edx

            pop edx
            pop ecx
            pop ebx
            pop eax
        }
        // 如果不支持 SSE2，降级为系统分配器
        if ((dwFeature & _SSE2_FEATURE_BIT) == 0) anAllocId = 0;
    }
#endif

    // 读取 MMGT_CLEAR 环境变量
    // 控制是否清空（置零）释放的内存块，用于安全考虑
    aVar = getenv("MMGT_CLEAR");
    Standard_Boolean toClear = (aVar ? (atoi(aVar) != 0) : Standard_True); // 默认 True

    // on Windows (actual for XP and 2000) activate low fragmentation heap
    // for CRT heap in order to get best performance.
    // Environment variable MMGT_LFH can be used to switch off this action (if set to 0)
    //
    // 中文说明：
    // Windows 低碎片堆（Low Fragmentation Heap, LFH）配置
    // 对 Windows XP 和 2000 有效，可以显著降低堆碎片，提高性能
    // 如果 MMGT_LFH 环境变量设为 0，则禁用此功能
#if defined(_MSC_VER)
    aVar = getenv("MMGT_LFH");
    // 如果环境变量未设置或非 0，启用 LFH
    if (aVar == NULL || atoi(aVar) != 0) {
        ULONG aHeapInfo = 2;                          // HeapCompatibilityInformation = 2（启用 LFH）
        HANDLE aCRTHeap = (HANDLE)_get_heap_handle(); // 获取 C 运行时堆句柄
        // 设置堆信息以启用 LFH
        HeapSetInformation(aCRTHeap, HeapCompatibilityInformation, &aHeapInfo, sizeof(aHeapInfo));
    }
#endif

    // 根据 anAllocId 创建相应的内存管理器
    // 这体现了工厂模式：同一接口（Standard_MMgrRoot），不同的实现
    switch (anAllocId) {
        case 1: // OCCT optimized memory allocator
        {
            // OCCT 优化的内存分配器
            // 这是 OCCT 自己开发的高效分配器，支持多个环境变量微调

            // MMGT_MMAP：是否使用内存映射文件
            aVar = getenv("MMGT_MMAP");
            Standard_Boolean bMMap = (aVar ? (atoi(aVar) != 0) : Standard_True);

            // MMGT_CELLSIZE：内存单元大小（字节），默认 200
            // 影响分配效率和内存碎片
            aVar = getenv("MMGT_CELLSIZE");
            Standard_Integer aCellSize = (aVar ? atoi(aVar) : 200);

            // MMGT_NBPAGES：初始分配的页数，默认 1000
            // 更多页意味着初始内存使用更多但分配次数更少
            aVar = getenv("MMGT_NBPAGES");
            Standard_Integer aNbPages = (aVar ? atoi(aVar) : 1000);

            // MMGT_THRESHOLD：阈值（字节），默认 40000
            // 超过此阈值的分配使用不同的策略
            aVar = getenv("MMGT_THRESHOLD");
            Standard_Integer aThreshold = (aVar ? atoi(aVar) : 40000);

            // 创建 OCCT 优化分配器实例
            myFMMgr = new Standard_MMgrOpt(toClear, bMMap, aCellSize, aNbPages, aThreshold);
            break;
        }
        case 2: // TBB memory allocator
        {
            // Intel TBB（Threading Building Blocks）内存分配器
            // 特点：
            // • 线程安全
            // • 并行环境下性能优秀
            // • 减少缓存竞争
            myFMMgr = new Standard_MMgrTBBalloc(toClear);
            break;
        }
        case 0:
        default: // system default memory allocator
        {
            // 系统原始分配器（Standard_MMgrRaw）
            // 特点：
            // • 直接使用 malloc/free（或 new/delete）
            // • 最简单、最可靠
            // • 是所有其他分配器的后备选项
            myFMMgr = new Standard_MMgrRaw(toClear);
        }
    }
}

//=======================================================================
// function : ~Standard_MMgrFactory
// purpose  :
//
// 中文说明：
// 析构函数 - 清理内存管理器
// 调用 Purge(true) 确保所有内存都被释放
// 但并不显式 delete 内存管理器，因为它可能在程序退出时被其他静态对象引用
//=======================================================================

Standard_MMgrFactory::~Standard_MMgrFactory() {
    if (myFMMgr)
        // Purge(true) 释放所有缓存的内存块回系统
        // 这是在程序退出时进行的清理，不是删除管理器本身
        myFMMgr->Purge(Standard_True);
}

//=======================================================================
// function: GetMMgr
//
// This static function has a purpose to wrap static holder for memory
// manager instance.
//
// Wrapping holder inside a function is needed to ensure that it will
// be initialized not later than the first call to memory manager (that
// would be impossible to guarantee if holder was static variable on
// global or file scope, because memory manager may be called from
// constructors of other static objects).
//
// Note that at the same time we could not guarantee that the holder
// object is destroyed after last call to memory manager, since that
// last call may be from static Handle() object which has been initialized
// dynamically during program execution rather than in its constructor.
//
// Therefore holder currently does not call destructor of the memory manager
// but only its method Purge() with Standard_True.
//
// To free the memory completely, we probably could use compiler-specific
// pragmas (such as '#pragma fini' on SUN Solaris and '#pragma init_seg' on
// WNT MSVC++) to put destructing function in code segment that is called
// after destructors of other (even static) objects. However, this is not
// done by the moment since it is compiler-dependent and there is no guarantee
// that some other object calling memory manager is not placed also in that segment...
//
// Note that C runtime function atexit() could not help in this problem
// since its behaviour is the same as for destructors of static objects
// (see ISO 14882:1998 "Programming languages -- C++" 3.6.3)
//
// The correct approach to deal with the problem would be to have memory manager
// to properly control its memory allocation and caching free blocks so
// as to release all memory as soon as it is returned to it, and probably
// even delete itself if all memory it manages has been released and
// last call to method Purge() was with True.
//
// Note that one possible method to control memory allocations could
// be counting calls to Allocate() and Free()...
//
// 中文说明：
// GetMMgr 函数设计说明
//
// 为什么在函数内使用静态变量而不是全局静态变量？
//
// 问题背景：
// • 内存管理器需要在程序执行的任何时刻都可用
// • 它可能在其他静态对象的构造函数中被调用
// • 全局或文件作用域的静态变量初始化时机难以控制
//
// 解决方案（函数内静态变量）：
// • 函数内的静态变量在首次调用函数时初始化
// • 这保证了内存管理器在任何其他代码之前初始化
// • 符合"最迟初始化"的原则（Lazy Initialization）
//
// 相关的生命周期问题：
// • 程序退出时，内存管理器的析构时机难以预测
// • 可能有其他静态对象在内存管理器之后才被销毁
// • 这些对象的析构可能还需要内存管理器
//
// 当前的妥协方案：
// • GetMMgr 函数不显式调用内存管理器的析构函数
// • 只在析构函数中调用 Purge(true) 释放缓存内存
// • 内存管理器对象本身在程序退出时由操作系统回收
//
// 理想方案（未实现）：
// • 使用编译器特定的段（#pragma）确保执行顺序
// • 或让内存管理器自我管理，当所有内存都被释放时自动销毁
// • 但这需要追踪 Allocate() 和 Free() 的调用次数
//
// 这是系统编程中的经典问题：
// 如何管理全局资源的生命周期，使其在任何时刻都可用，
// 又能在程序退出时正确清理？
//=======================================================================
Standard_MMgrRoot* Standard_MMgrFactory::GetMMgr() {
    // 静态工厂实例 - 首次调用时创建
    // 由于是函数内的静态变量，它在首次调用 GetMMgr() 时被初始化
    static Standard_MMgrFactory aFactory;
    // 返回工厂持有的内存管理器指针
    return aFactory.myFMMgr;
}

//=======================================================================
// function : Allocate
// purpose  :
//
// 中文说明：
// 分配指定大小的内存
// 这是 Standard 名字空间的全局分配函数，所有 OCCT 内存分配都通过此函数
//
// @param size 所需的内存大小（字节）
// @return 指向分配内存的指针
//
// 工作流程：
// 1. 获取全局内存管理器实例
// 2. 调用管理器的 Allocate 方法
// 3. 返回分配的指针
//=======================================================================

Standard_Address Standard::Allocate(const Standard_Size size) {
    return Standard_MMgrFactory::GetMMgr()->Allocate(size);
}

//=======================================================================
// function : Free
// purpose  :
//
// 中文说明：
// 释放之前分配的内存
// 这是 Standard 名字空间的全局释放函数
//
// @param theStorage 要释放的内存指针（必须由 Allocate 返回）
//
// 工作流程：
// 1. 获取全局内存管理器实例
// 2. 调用管理器的 Free 方法
// 3. 内存被标记为可重用
//=======================================================================

void Standard::Free(Standard_Address theStorage) {
    Standard_MMgrFactory::GetMMgr()->Free(theStorage);
}

//=======================================================================
// function : Reallocate
// purpose  :
//
// 中文说明：
// 重新分配（调整）内存大小
// 这是 Standard 名字空间的全局重新分配函数
//
// @param theStorage 原始内存指针
// @param theSize 新的大小
// @return 新的内存指针（可能与原指针不同）
//
// 特点：
// • 如果可能在原地扩展或收缩
// • 否则分配新内存，复制数据，释放旧内存
// • 具体行为取决于所使用的内存管理器
//=======================================================================

Standard_Address Standard::Reallocate(Standard_Address theStorage, const Standard_Size theSize) {
    return Standard_MMgrFactory::GetMMgr()->Reallocate(theStorage, theSize);
}

//=======================================================================
// function : Purge
// purpose  :
//
// 中文说明：
// 清理内存管理器的缓存
// 释放所有可回收的内存块给系统
//
// @return 释放的内存块数
//
// 使用场景：
// • 程序执行完一个大任务后
// • 需要释放内存回系统
// • 尽可能回收所有空闲块
//
// 注意：
// • 不会删除已分配的内存
// • 只释放管理器的缓存和空闲池
//=======================================================================

Standard_Integer Standard::Purge() {
    return Standard_MMgrFactory::GetMMgr()->Purge();
}

//=======================================================================
// function : AllocateAligned
// purpose  :
//
// 中文说明：
// 分配对齐的内存
// 某些操作（如 SIMD）需要对齐到特定字节边界的内存
//
// @param theSize 内存大小（字节）
// @param theAlign 对齐粒度（通常为 16、32、64 等 2 的幂）
// @return 指向对齐内存的指针
//
// 平台差异：
// • Windows（MSVC）：使用 _aligned_malloc
// • Android/QNX：使用 memalign
// • GCC（i386/x86_64）：使用 _mm_malloc（SSE 相关）
// • POSIX：使用 posix_memalign
//
// 对齐用途：
// • 向量化运算（SIMD）：SSE2、AVX 等需要对齐
// • 缓存对齐：避免缓存行竞争
// • 硬件要求：某些硬件需要特定对齐
//=======================================================================

Standard_Address Standard::AllocateAligned(const Standard_Size theSize, const Standard_Size theAlign) {
#if defined(_MSC_VER)
    // Windows MSVC：使用 _aligned_malloc
    // 参数顺序：大小在前，对齐在后
    return _aligned_malloc(theSize, theAlign);
#elif defined(__ANDROID__) || defined(__QNX__)
    // Android/QNX：使用 memalign
    // 参数顺序：对齐在前，大小在后（与 _aligned_malloc 相反！）
    return memalign(theAlign, theSize);
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) &&                                \
       (defined(__i386) || defined(__x86_64)))
    // GCC（i386 或 x86_64）：使用 Intel SSE 的 _mm_malloc
    // 用于分配与 SSE 相关的对齐内存
    return _mm_malloc(theSize, theAlign);
#else
    // POSIX 标准方法：posix_memalign
    // 这是最通用的方法，但需要预先声明
    void* aPtr;
    // posix_memalign 返回 0 表示成功，设置 *aPtr 为分配的指针
    if (posix_memalign(&aPtr, theAlign, theSize)) {
        // 分配失败，返回 NULL
        return NULL;
    }
    return aPtr;
#endif
}

//=======================================================================
// function : FreeAligned
// purpose  :
//
// 中文说明：
// 释放对齐的内存
// 必须使用对应的 Free 函数释放由 AllocateAligned 分配的内存
// 不能用普通的 free() 或 Standard::Free()
//
// @param thePtrAligned 由 AllocateAligned 返回的指针
//
// 重要：
// • 必须配对使用：AllocateAligned 分配，FreeAligned 释放
// • 不同平台的释放函数不同
// • 混用会导致崩溃或内存泄漏
//=======================================================================

void Standard::FreeAligned(Standard_Address thePtrAligned) {
#if defined(_MSC_VER)
    // Windows MSVC：使用 _aligned_free
    _aligned_free(thePtrAligned);
#elif defined(__ANDROID__) || defined(__QNX__)
    // Android/QNX：使用普通 free（memalign 分配的内存可用 free 释放）
    free(thePtrAligned);
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) &&                                \
       (defined(__i386) || defined(__x86_64)))
    // GCC（i386 或 x86_64）：使用 Intel SSE 的 _mm_free
    _mm_free(thePtrAligned);
#else
    // POSIX：使用普通 free（posix_memalign 分配的内存可用 free 释放）
    free(thePtrAligned);
#endif
}
