// Created on: 2001-01-06
// Created by: OCC Team
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _Message_Printer_HeaderFile
#define _Message_Printer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Message_Gravity.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_SStream.hxx>

class TCollection_ExtendedString;
class TCollection_AsciiString;

class Message_Printer;
DEFINE_STANDARD_HANDLE(Message_Printer, Standard_Transient)

//! Abstract interface class defining printer as output context for text messages
//!
//! The message, besides being text string, has associated gravity
//! level, which can be used by printer to decide either to process a message or ignore it.
//!
//! 定义打印器作为文本消息输出上下文的抽象接口类
//!
//! 除了是文本字符串外，消息还有关联的严重级别，
//! 打印器可以使用该级别来决定是处理消息还是忽略消息
class Message_Printer : public Standard_Transient {
    DEFINE_STANDARD_RTTIEXT(Message_Printer, Standard_Transient)
public:
    //! Return trace level used for filtering messages;
    //! messages with lover gravity will be ignored.
    //!
    //! 返回用于过滤消息的跟踪级别
    //! 严重级别较低的消息将被忽略
    Message_Gravity GetTraceLevel() const {
        return myTraceLevel;
    }

    //! Set trace level used for filtering messages.
    //! By default, trace level is Message_Info, so that all messages are output
    //!
    //! 设置用于过滤消息的跟踪级别
    //! 默认情况下，跟踪级别为 Message_Info，因此所有消息都将输出
    void SetTraceLevel(const Message_Gravity theTraceLevel) {
        myTraceLevel = theTraceLevel;
    }

    //! Send a string message with specified trace level.
    //! The last Boolean argument is deprecated and unused.
    //! Default implementation redirects to send().
    //!
    //! 发送带有指定跟踪级别的字符串消息
    //! 最后的布尔参数已弃用且未使用
    //! 默认实现重定向到 send()
    Standard_EXPORT virtual void Send(const TCollection_ExtendedString& theString,
                                      const Message_Gravity theGravity) const;

    //! Send a string message with specified trace level.
    //! The last Boolean argument is deprecated and unused.
    //! Default implementation redirects to send().
    //!
    //! 发送带有指定跟踪级别的字符串消息（C 字符串）
    //! 最后的布尔参数已弃用且未使用
    //! 默认实现重定向到 send()
    Standard_EXPORT virtual void Send(const Standard_CString theString, const Message_Gravity theGravity) const;

    //! Send a string message with specified trace level.
    //! The last Boolean argument is deprecated and unused.
    //! Default implementation redirects to send().
    //!
    //! 发送带有指定跟踪级别的字符串消息（ASCII 字符串）
    //! 最后的布尔参数已弃用且未使用
    //! 默认实现重定向到 send()
    Standard_EXPORT virtual void Send(const TCollection_AsciiString& theString, const Message_Gravity theGravity) const;

    //! Send a string message with specified trace level.
    //! Stream is converted to string value.
    //! Default implementation calls first method Send().
    //!
    //! 发送带有指定跟踪级别的字符串消息（从流）
    //! 流被转换为字符串值
    //! 默认实现调用第一个 Send() 方法
    Standard_EXPORT virtual void SendStringStream(const Standard_SStream& theStream,
                                                  const Message_Gravity theGravity) const;

    //! Send a string message with specified trace level.
    //! The object is converted to string in format: <object kind> : <object pointer>.
    //! Default implementation calls first method Send().
    //!
    //! 发送带有指定跟踪级别的字符串消息（从 Transient 对象）
    //! 对象被转换为字符串格式：<object kind> : <object pointer>
    //! 默认实现调用第一个 Send() 方法
    Standard_EXPORT virtual void SendObject(const Handle(Standard_Transient) & theObject,
                                            const Message_Gravity theGravity) const;

protected:
    //! Empty constructor with Message_Info trace level
    //! 空构造函数，跟踪级别为 Message_Info
    Standard_EXPORT Message_Printer();

    //! Send a string message with specified trace level.
    //! This method must be redefined in descendant.
    //!
    //! 发送带有指定跟踪级别的字符串消息
    //! 此方法必须在派生类中重新定义
    Standard_EXPORT virtual void send(const TCollection_AsciiString& theString,
                                      const Message_Gravity theGravity) const = 0;

protected:
    Message_Gravity myTraceLevel; // 用于过滤消息的跟踪级别
};

#endif // _Message_Printer_HeaderFile
