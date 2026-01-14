// Created on: 1991-09-05
// Created by: Philippe COICADAN
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

#ifndef _Standard_Failure_HeaderFile
#define _Standard_Failure_HeaderFile

#include <Standard_Type.hxx>

#include <Standard_CString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>

DEFINE_STANDARD_HANDLE(Standard_Failure, Standard_Transient)

//! Forms the root of the entire exception hierarchy.
//! 
//! 形成整个异常层次结构的根
//! 所有 OCCT 异常都继承自此类，包括：
//! - Standard_OutOfRange（索引超出范围）
//! - Standard_NullObject（空对象操作）
//! - Standard_DivideByZero（除数为零）
//! - Standard_OutOfMemory（内存不足）
//! - 等等
class Standard_Failure : public Standard_Transient
{
public:

    //! Creates a status object of type "Failure".
    //! 创建一个"失败"类型的状态对象
    Standard_EXPORT Standard_Failure();

    //! Copy constructor
    //! 拷贝构造函数
    Standard_EXPORT Standard_Failure(const Standard_Failure& f);

    //! Creates a status object of type "Failure".
    //! @param theDesc [in] exception description
    //! 创建一个"失败"类型的状态对象
    //! @param theDesc [输入] 异常描述信息
    Standard_EXPORT Standard_Failure(const Standard_CString theDesc);

    //! Creates a status object of type "Failure" with stack trace.
    //! @param theDesc [in] exception description
    //! @param theStackTrace [in] associated stack trace
    //! 创建一个带有堆栈跟踪的"失败"类型的状态对象
    //! @param theDesc [输入] 异常描述信息
    //! @param theStackTrace [输入] 关联的堆栈跟踪
    Standard_EXPORT Standard_Failure(const Standard_CString theDesc,
        const Standard_CString theStackTrace);

    //! Assignment operator
    //! 赋值运算符
    Standard_EXPORT Standard_Failure& operator= (const Standard_Failure& f);

    //! Destructor
    //! 析构函数
    Standard_EXPORT ~Standard_Failure();

    //! Prints on the stream @p theStream the exception name followed by the error message.
    //!
    //! Note: there is a short-cut @c operator<< (Standard_OStream&, Handle(Standard_Failure)&)
    //! 将异常名称后跟错误消息打印到流
    //! 注意：有一个快捷的 operator<< (Standard_OStream&, Handle(Standard_Failure)&)
    Standard_EXPORT void Print(Standard_OStream& theStream) const;

    //! Returns error message
    //! 返回错误消息
    Standard_EXPORT virtual Standard_CString GetMessageString() const;

    //! Sets error message
    //! 设置错误消息
    Standard_EXPORT virtual void SetMessageString(const Standard_CString theMessage);

    //! Returns the stack trace string
    //! 返回堆栈跟踪字符串
    //! 用于调试，显示异常发生时的调用堆栈
    Standard_EXPORT virtual Standard_CString GetStackString() const;

    //! Sets the stack trace string
    //! 设置堆栈跟踪字符串
    Standard_EXPORT virtual void SetStackString(const Standard_CString theStack);

    //! Re-throws the exception
    //! 重新抛出异常
    Standard_EXPORT void Reraise();

    //! Re-throws the exception and changes its error message
    //! 重新抛出异常并修改其错误消息
    Standard_EXPORT void Reraise(const Standard_CString aMessage);

    //! Reraises a caught exception and changes its error message.
    //! 重新抛出已捕获的异常并修改其错误消息
    //! 可用于在异常处理程序中修改异常信息后继续传播
    Standard_EXPORT void Reraise(const Standard_SStream& aReason);

public:

    //! Raises an exception of type "Failure" and associates
    //! an error message to it. The message can be printed
    //! in an exception handler.
    //! 抛出"失败"类型的异常并关联一个错误消息
    //! 该消息可以在异常处理程序中打印
    Standard_EXPORT static void Raise(const Standard_CString aMessage = "");

    //! Raises an exception of type "Failure" and associates
    //! an error message to it. The message can be constructed
    //! at run-time.
    //! 抛出"失败"类型的异常并关联一个错误消息
    //! 该消息可以在运行时构造
    Standard_EXPORT static void Raise(const Standard_SStream& aReason);

    //! Used to construct an instance of the exception object as a handle.
    //! Shall be used to protect against possible construction of exception object in C stack,
    //! which is dangerous since some of methods require that object was allocated dynamically.
    //! 用于构造异常对象实例作为 Handle
    //! 应该使用此方法以防止在 C 堆栈上构造异常对象，
    //! 因为这很危险，某些方法要求对象是动态分配的
    Standard_EXPORT static Handle(Standard_Failure) NewInstance(Standard_CString theMessage);

    //! Used to construct an instance of the exception object as a handle.
    //! 用于构造异常对象实例作为 Handle（带堆栈跟踪）
    Standard_EXPORT static Handle(Standard_Failure) NewInstance(Standard_CString theMessage,
        Standard_CString theStackTrace);

    //! Returns the default length of stack trace to be captured by Standard_Failure constructor;
    //! 0 by default meaning no stack trace.
    //! 返回 Standard_Failure 构造函数捕获的堆栈跟踪的默认长度
    //! 默认为 0，表示不捕获堆栈跟踪
    //! 可以通过 SetDefaultStackTraceLength() 修改
    Standard_EXPORT static Standard_Integer DefaultStackTraceLength();

    //! Sets default length of stack trace to be captured by Standard_Failure constructor.
    //! 设置 Standard_Failure 构造函数捕获的堆栈跟踪的默认长度
    //! 例如：SetDefaultStackTraceLength(10) 会捕获 10 层调用堆栈
    Standard_EXPORT static void SetDefaultStackTraceLength(Standard_Integer theNbStackTraces);

public:

    //! Used to throw CASCADE exception from C signal handler.
    //! On platforms that do not allow throwing C++ exceptions
    //! from this handler (e.g. Linux), uses longjump to get to
    //! the current active signal handler, and only then is
    //! converted to C++ exception.
    //! 用于从 C 信号处理程序抛出 OCCT 异常
    //! 在不允许从信号处理程序抛出 C++ 异常的平台（例如 Linux）上，
    //! 使用 longjump 获取当前活跃的信号处理程序，然后转换为 C++ 异常
    //! 这是一个高级用法，通常用于处理系统信号
    Standard_EXPORT void Jump();

    DEFINE_STANDARD_RTTIEXT(Standard_Failure, Standard_Transient)

protected:

    //! Used only if standard C++ exceptions are used.
    //! Throws exception of the same type as this by C++ throw,
    //! and stores current object as last thrown exception,
    //! to be accessible by method Caught()
    //! 仅在使用标准 C++ 异常时使用
    //! 通过 C++ throw 抛出相同类型的异常，
    //! 并将当前对象存储为最后抛出的异常，
    //! 以便通过 Caught() 方法访问
    Standard_EXPORT virtual void Throw() const;

private:

    //! Reference-counted string,
    //! Memory block is allocated with an extra 4-byte header (int representing number of references)
    //! using low-level malloc() to avoid exceptions.
    //! 
    //! 引用计数字符串
    //! 内存块通过 4 字节头部（int 表示引用数量）分配
    //! 使用低级 malloc() 以避免异常
    struct StringRef
    {
        Standard_Integer   Counter;  //!< 引用计数
        Standard_Character Message[1];  //!< 消息字符串（实际大小由分配函数确定）

        //! Return message string.
        //! 返回消息字符串
        Standard_CString GetMessage() const { return (Standard_CString)&Message[0]; }

        //! Allocate reference-counted message string.
        //! 分配引用计数消息字符串
        static StringRef* allocate_message(Standard_CString theString);

        //! Copy reference-counted message string.
        //! 拷贝引用计数消息字符串（增加引用计数）
        static StringRef* copy_message(StringRef* theString);

        //! Release reference-counted message string.
        //! 释放引用计数消息字符串（减少引用计数，如果为 0 则删除）
        static void deallocate_message(StringRef* theString);
    };

private:

    StringRef* myMessage;    //!< 错误消息（引用计数）
    StringRef* myStackTrace; //!< 堆栈跟踪（引用计数）

};

// =======================================================================
// function : operator<<
// purpose  : 
// 用于打印 Handle(Standard_Failure) 异常到流
// =======================================================================
inline Standard_OStream& operator<< (Standard_OStream& theStream,
    const Handle(Standard_Failure)& theFailure)
{
    theFailure->Print(theStream);
    return theStream;
}

// =======================================================================
// function : operator<<
// purpose  : 
// 用于打印 Standard_Failure 异常到流
// =======================================================================
inline Standard_OStream& operator<< (Standard_OStream& theStream,
    const Standard_Failure& theFailure)
{
    theFailure.Print(theStream);
    return theStream;
}

#endif // _Standard_Failure_HeaderFile
