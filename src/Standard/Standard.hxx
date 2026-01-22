// Created on: 1991-09-05
// Created by: J.P. TIRAUlt
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Standard_HeaderFile
#define _Standard_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Address.hxx>
#include <Standard_Size.hxx>
#include <Standard_Integer.hxx>

class Standard_ErrorHandler;
class Standard_Persistent;
class Standard_Transient;
class Standard_Failure;

//! The package Standard provides global memory allocator and other basic
//! services used by other OCCT components.
//!
//! Standard 包提供全局内存分配器和其他基本服务，供 OCCT 其他组件使用。
//! 这是 OCCT 的核心模块，处理内存管理、错误处理等基础功能。

class Standard {
public:
    DEFINE_STANDARD_ALLOC;

    //! Allocates memory blocks
    //! aSize - bytes to  allocate
    //! 分配内存块
    //! aSize - 要分配的字节数
    Standard_EXPORT static Standard_Address Allocate(const Standard_Size aSize);

    //! Deallocates memory blocks
    //! @param thePtr - previously allocated memory block to be freed
    //! 释放内存块
    //! @param thePtr - 之前分配的需要释放的内存块
    Standard_EXPORT static void Free(const Standard_Address thePtr);

    //! Template version of function Free(), nullifies the argument pointer
    //! @param thePtr - previously allocated memory block to be freed
    //! Free() 的模板版本，释放内存后将指针置为空
    //! @param thePtr - 之前分配的需要释放的内存块
    template <typename T> static inline void Free(T*& thePtr) {
        Free((void*)thePtr);
        thePtr = 0;
    }

    //! Reallocates memory blocks
    //! aStorage - previously allocated memory block
    //! aNewSize - new size in bytes
    //! 重新分配内存块（改变已分配内存的大小）
    //! aStorage - 之前分配的内存块
    //! aNewSize - 新的大小（字节数）
    Standard_EXPORT static Standard_Address Reallocate(const Standard_Address aStorage, const Standard_Size aNewSize);

    //! Allocates aligned memory blocks.
    //! Should be used with CPU instructions which require specific alignment.
    //! For example: SSE requires 16 bytes, AVX requires 32 bytes.
    //! @param theSize  bytes to allocate
    //! @param theAlign alignment in bytes
    //! 分配对齐的内存块
    //! 对于需要特定内存对齐的 CPU 指令（如 SSE、AVX）应该使用此函数
    //! @param theSize  要分配的字节数
    //! @param theAlign 对齐的字节数（如 SSE 需要 16 字节，AVX 需要 32 字节）
    Standard_EXPORT static Standard_Address AllocateAligned(const Standard_Size theSize, const Standard_Size theAlign);

    //! Deallocates memory blocks
    //! @param thePtrAligned the memory block previously allocated with AllocateAligned()
    //! 释放对齐的内存块
    //! @param thePtrAligned 之前用 AllocateAligned() 分配的内存块
    Standard_EXPORT static void FreeAligned(const Standard_Address thePtrAligned);

    //! Template version of function FreeAligned(), nullifies the argument pointer
    //! @param thePtrAligned the memory block previously allocated with AllocateAligned()
    //! FreeAligned() 的模板版本，释放对齐内存后将指针置为空
    //! @param thePtrAligned 之前用 AllocateAligned() 分配的内存块
    template <typename T> static inline void FreeAligned(T*& thePtrAligned) {
        FreeAligned((void*)thePtrAligned);
        thePtrAligned = 0;
    }

    //! Deallocates the storage retained on the free list
    //! and clears the list.
    //! Returns non-zero if some memory has been actually freed.
    //! 释放保存在空闲列表中的存储空间并清空列表
    //! 返回值：如果实际释放了内存则返回非零值，否则返回零
    Standard_EXPORT static Standard_Integer Purge();

    //! Appends backtrace to a message buffer.
    //! Stack information might be incomplete in case of stripped binaries.
    //! Implementation details:
    //! - Not implemented for Android, iOS, QNX and UWP platforms.
    //! - On non-Windows platform, this function is a wrapper to backtrace() system call.
    //! - On Windows (Win32) platform, the function loads DbgHelp.dll dynamically,
    //!   and no stack will be provided if this or companion libraries (SymSrv.dll, SrcSrv.dll, etc.) will not be found;
    //!   .pdb symbols should be provided on Windows platform to retrieve a meaningful stack;
    //!   only x86_64 CPU architecture is currently implemented.
    //! @param theBuffer [in] [out] message buffer to extend
    //! @param theBufferSize [in] message buffer size
    //! @param theNbTraces [in] maximum number of stack traces
    //! @param theContext [in] optional platform-dependent frame context;
    //!                        in case of DbgHelp (Windows) should be a pointer to CONTEXT
    //! @param theNbTopSkip [in] number of traces on top of the stack to skip
    //! @return TRUE on success
    //!
    //! 向消息缓冲区追加堆栈跟踪信息（用于调试）
    //! 在去除符号的二进制文件中，堆栈信息可能不完整
    //! 实现细节：
    //! - Android、iOS、QNX 和 UWP 平台不支持此功能
    //! - 在非 Windows 平台上，此函数是 backtrace() 系统调用的包装器
    //! - 在 Windows 平台上，动态加载 DbgHelp.dll
    //!   如果找不到 DbgHelp.dll 或相关库（SymSrv.dll、SrcSrv.dll 等），将无法获取堆栈
    //!   需要提供 .pdb 符号文件以获得有意义的堆栈信息
    //!   目前仅实现了 x86_64 CPU 架构
    //! @param theBuffer [输入/输出] 要扩展的消息缓冲区
    //! @param theBufferSize [输入] 消息缓冲区大小
    //! @param theNbTraces [输入] 最大堆栈跟踪数量
    //! @param theContext [输入] 可选的平台相关帧上下文（Windows DbgHelp 应为 CONTEXT 指针）
    //! @param theNbTopSkip [输入] 从堆栈顶部跳过的跟踪数量
    //! @return 成功返回 TRUE，失败返回 FALSE
    Standard_EXPORT static Standard_Boolean StackTrace(char* theBuffer, const int theBufferSize, const int theNbTraces,
                                                       void* theContext = NULL, const int theNbTopSkip = 0);
};

// include definition of handle to make it always visible
// (put at the and of the file due to cyclic dependency between headers)
// 包含 Handle 定义以使其始终可见
// （放在文件末尾是因为头文件之间存在循环依赖）
#include <Standard_Transient.hxx>

#endif // _Standard_HeaderFile
