// Created on: 2007-06-28
// Created by: OCC Team
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

#ifndef _Message_Messenger_HeaderFile
#define _Message_Messenger_HeaderFile

#include <Message_Gravity.hxx>
#include <Message_SequenceOfPrinters.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

class Message_Printer;

// resolve name collisions with WinAPI headers
#ifdef AddPrinter
#undef AddPrinter
#endif

class Message_Messenger;
DEFINE_STANDARD_HANDLE(Message_Messenger, Standard_Transient)

//! Messenger is API class providing general-purpose interface for
//! libraries that may issue text messages without knowledge
//! of how these messages will be further processed.
//!
//! The messenger contains a sequence of "printers" which can be
//! customized by the application, and dispatches every received
//! message to all the printers.
//!
//! For convenience, a set of methods Send...() returning a string
//! stream buffer is defined for use of stream-like syntax with operator << 
//!
//! Example:
//! ~~~~~
//! Messenger->SendFail() << " Unknown fail at line " << aLineNo << " in file " << aFile;
//! ~~~~~
//!
//! The message is sent to messenger on destruction of the stream buffer,
//! call to Flush(), or passing manipulator std::ends, std::endl, or std::flush.
//! Empty messages are not sent except if manipulator is used.
//! 
//! Messenger 是一个 API 类，为库提供通用接口以发送文本消息，
//! 而不需要了解这些消息如何被进一步处理
//! 
//! messenger 包含一个"打印器"序列，可以由应用程序自定义，
//! 并将接收到的每条消息分发到所有打印器
//! 
//! 为了方便起见，定义了一组 Send...() 方法，返回字符串
//! 流缓冲区，可使用 operator << 进行类似流的语法
//! 
//! 示例：
//! ~~~~~
//! Messenger->SendFail() << " 在第 " << aLineNo << " 行出错，文件 " << aFile;
//! ~~~~~
//! 
//! 在销毁流缓冲区、调用 Flush()、或传递操纵符 std::ends、std::endl 或 std::flush 时，
//! 消息被发送到 messenger。除非使用操纵符，否则不发送空消息。
class Message_Messenger : public Standard_Transient
{
    DEFINE_STANDARD_RTTIEXT(Message_Messenger, Standard_Transient)
public:
    //! Auxiliary class wrapping std::stringstream thus allowing constructing
    //! message via stream interface, and putting result into its creator
    //! Message_Messenger within destructor.
    //!
    //! It is intended to be used either as temporary object or as local
    //! variable, note that content will be lost if it is copied.
    //! 
    //! 辅助类，包装 std::stringstream，允许通过流接口构造消息，
    //! 并在析构函数中将结果放入其创建者 Message_Messenger
    //! 
    //! 应将其作为临时对象或局部变量使用，
    //! 注意如果复制则内容会丢失
    class StreamBuffer
    {
    public:

        //! Destructor flushing constructed message.
        //! 析构函数，刷新已构造的消息
        ~StreamBuffer() { Flush(); }

        //! Flush collected string to messenger
        //! 将收集到的字符串刷新到 messenger
        void Flush(Standard_Boolean doForce = Standard_False)
        {
            myStream.flush();
            if (doForce || myStream.rdbuf()->in_avail() > 0)
            {
                if (myMessenger)
                {
                    myMessenger->Send(myStream, myGravity);
                }
                myStream.str(std::string()); // empty the buffer for possible reuse（清空缓冲区以备重复使用）
            }
        }

        //! Formal copy constructor.
        //!
        //! Since buffer is intended for use as temporary object or local
        //! variable, copy (or move) is needed only formally to be able to
        //! return the new instance from relevant creation method.
        //! In practice it should never be called because modern compilers
        //! create such instances in place.
        //! However note that if this constructor is called, the buffer
        //! content (string) will not be copied  (move is not supported for
        //! std::stringstream class on old compilers such as gcc 4.4, msvc 9).
        //! 
        //! 形式上的拷贝构造函数
        //! 
        //! 由于缓冲区是为作为临时对象或局部变量使用而设计的，
        //! 拷贝（或移动）只是形式上需要以从相关创建方法返回新实例。
        //! 在实践中不应该被调用，因为现代编译器在原地创建这样的实例。
        //! 但请注意，如果调用了此构造函数，缓冲区内容（字符串）将不会被复制
        //! （移动不受旧编译器如 gcc 4.4、msvc 9 的 std::stringstream 类支持）
        StreamBuffer(const StreamBuffer& theOther)
            : myMessenger(theOther.myMessenger), myGravity(theOther.myGravity)
        {
        }

        //! Wrapper for operator << of the stream
        //! 流的 operator << 的包装器
        template <typename T>
        StreamBuffer& operator << (const T& theArg)
        {
            myStream << theArg;
            return *this;
        }

        //! Operator << for manipulators of ostream (ends, endl, flush),
        //! flushes the buffer (sends the message)
        //! 用于 ostream 操纵符（ends、endl、flush）的 operator <<，
        //! 刷新缓冲区（发送消息）
        StreamBuffer& operator << (std::ostream& (*)(std::ostream&))
        {
            Flush(Standard_True);
            return *this;
        }

        //! Access to the stream object
        //! 访问流对象
        Standard_SStream& Stream() { return myStream; }

        //! Cast to OStream&
        //! 强制转换为 OStream&
        operator Standard_OStream& () { return myStream; }

        //! Access to the messenger
        //! 访问 messenger
        Message_Messenger* Messenger() { return myMessenger; }

    private:
        friend class Message_Messenger;

        //! Main constructor creating temporary buffer.
        //! Accessible only to Messenger class.
        //! 创建临时缓冲区的主构造函数，仅 Messenger 类可访问
        StreamBuffer(Message_Messenger* theMessenger, Message_Gravity theGravity)
            : myMessenger(theMessenger),
            myGravity(theGravity)
        {
        }

    private:
        Message_Messenger* myMessenger; // don't make a Handle since this object should be created on stack
                                        // 不要创建 Handle，因为该对象应在栈上创建
        Message_Gravity    myGravity;   // 消息的严重级别
        Standard_SStream   myStream;    // 消息流缓冲区
    };

public:

    //! Empty constructor; initializes by single printer directed to std::cout.
    //! Note: the default messenger is not empty but directed to cout
    //! in order to protect against possibility to forget defining printers.
    //! If printing to cout is not needed, clear messenger by GetPrinters().Clear()
    //! 
    //! 空构造函数，通过单个指向 std::cout 的打印器进行初始化
    //! 注意：默认的 messenger 不为空，而是指向 cout，
    //! 为了防止忘记定义打印器的可能性
    //! 如果不需要打印到 cout，通过 GetPrinters().Clear() 清除 messenger
    Standard_EXPORT Message_Messenger();

    //! Create messenger with single printer
    //! 使用单个打印器创建 messenger
    Standard_EXPORT Message_Messenger(const Handle(Message_Printer)& thePrinter);

    //! Add a printer to the messenger.
    //! The printer will be added only if it is not yet in the list.
    //! Returns True if printer has been added.
    //! 
    //! 向 messenger 添加打印器
    //! 仅当打印器尚未在列表中时才会添加它
    //! 如果打印器已添加则返回 True
    Standard_EXPORT Standard_Boolean AddPrinter(const Handle(Message_Printer)& thePrinter);

    //! Removes specified printer from the messenger.
    //! Returns True if this printer has been found in the list
    //! and removed.
    //! 
    //! 从 messenger 中删除指定的打印器
    //! 如果在列表中找到此打印器并删除它，返回 True
    Standard_EXPORT Standard_Boolean RemovePrinter(const Handle(Message_Printer)& thePrinter);

    //! Removes printers of specified type (including derived classes)
    //! from the messenger.
    //! Returns number of removed printers.
    //! 
    //! 从 messenger 中删除指定类型（包括派生类）的打印器
    //! 返回删除的打印器数量
    Standard_EXPORT Standard_Integer RemovePrinters(const Handle(Standard_Type)& theType);

    //! Returns current sequence of printers
    //! 返回当前的打印器序列
    const Message_SequenceOfPrinters& Printers() const { return myPrinters; }

    //! Returns sequence of printers
    //! The sequence can be modified.
    //! 返回打印器序列。该序列可以被修改
    Message_SequenceOfPrinters& ChangePrinters() { return myPrinters; }

    //! Dispatch a message to all the printers in the list.
    //! Three versions of string representations are accepted for
    //! convenience, by default all are converted to ExtendedString.
    //! 
    //! 将消息分发到列表中的所有打印器
    //! 为了方便起见，接受三种字符串表示形式，默认都转换为 ExtendedString
    Standard_EXPORT void Send(const Standard_CString theString,
        const Message_Gravity theGravity = Message_Warning) const;

    //! See above
    //! 同上（从标准流发送）
    Standard_EXPORT void Send(const Standard_SStream& theStream,
        const Message_Gravity theGravity = Message_Warning) const;

    //! See above
    //! 同上（从 ASCII 字符串发送）
    Standard_EXPORT void Send(const TCollection_AsciiString& theString,
        const Message_Gravity theGravity = Message_Warning) const;

    //! See above
    //! 同上（从扩展字符串发送）
    Standard_EXPORT void Send(const TCollection_ExtendedString& theString,
        const Message_Gravity theGravity = Message_Warning) const;

    //! Create string buffer for message of specified type
    //! 为指定类型的消息创建字符串缓冲区
    StreamBuffer Send(Message_Gravity theGravity) { return StreamBuffer(this, theGravity); }

    //! See above
    //! 同上（从 Transient 对象发送）
    Standard_EXPORT void Send(const Handle(Standard_Transient)& theObject, const Message_Gravity theGravity = Message_Warning) const;

    //! Create string buffer for sending Fail message
    //! 创建字符串缓冲区以发送失败消息
    StreamBuffer SendFail() { return Send(Message_Fail); }

    //! Create string buffer for sending Alarm message
    //! 创建字符串缓冲区以发送警报消息
    StreamBuffer SendAlarm() { return Send(Message_Alarm); }

    //! Create string buffer for sending Warning message
    //! 创建字符串缓冲区以发送警告消息
    StreamBuffer SendWarning() { return Send(Message_Warning); }

    //! Create string buffer for sending Info message
    //! 创建字符串缓冲区以发送信息消息
    StreamBuffer SendInfo() { return Send(Message_Info); }

    //! Create string buffer for sending Trace message
    //! 创建字符串缓冲区以发送跟踪消息
    StreamBuffer SendTrace() { return Send(Message_Trace); }

    //! Short-cut to Send (theMessage, Message_Fail)
    //! 快捷调用 Send (theMessage, Message_Fail)
    void SendFail(const TCollection_AsciiString& theMessage) { Send(theMessage, Message_Fail); }

    //! Short-cut to Send (theMessage, Message_Alarm)
    //! 快捷调用 Send (theMessage, Message_Alarm)
    void SendAlarm(const TCollection_AsciiString& theMessage) { Send(theMessage, Message_Alarm); }

    //! Short-cut to Send (theMessage, Message_Warning)
    //! 快捷调用 Send (theMessage, Message_Warning)
    void SendWarning(const TCollection_AsciiString& theMessage) { Send(theMessage, Message_Warning); }

    //! Short-cut to Send (theMessage, Message_Info)
    //! 快捷调用 Send (theMessage, Message_Info)
    void SendInfo(const TCollection_AsciiString& theMessage) { Send(theMessage, Message_Info); }

    //! Short-cut to Send (theMessage, Message_Trace)
    //! 快捷调用 Send (theMessage, Message_Trace)
    void SendTrace(const TCollection_AsciiString& theMessage) { Send(theMessage, Message_Trace); }

    //! Dumps the content of me into the stream
    //! 将内容转储到流中
    Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:

    Message_SequenceOfPrinters myPrinters; // 打印器序列

};

#endif // _Message_Messenger_HeaderFile
