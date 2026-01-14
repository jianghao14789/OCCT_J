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

#include <Message_Printer.hxx>

#include <Standard_Dump.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

// 为 Message_Printer 类注册 RTTI
IMPLEMENT_STANDARD_RTTIEXT(Message_Printer, Standard_Transient)

//=======================================================================
//function : Constructor
//purpose  : 构造函数，初始化打印机并设置默认的跟踪级别
//
// 说明：
//   - 打印机是 Message 系统中实际输出消息的组件
//   - 每个打印机都有一个跟踪级别阈值
//   - 只有严重级别 >= 跟踪级别的消息才会被打印机处理
//
// 默认跟踪级别：
//   - 设置为 Message_Info
//   - 这意味着默认情况下，Info 及以上级别的消息会被处理
//   - Trace 级别的消息默认被过滤掉（不输出）
//
// 跟踪级别的作用：
//   - 用于控制消息的过滤和输出
//   - 不同的打印机可以有不同的过滤级别
//   - 例如，控制台可能显示 Info 及以上，而日志文件可能显示所有消息
//=======================================================================
Message_Printer::Message_Printer()
    : myTraceLevel(Message_Info)
{
}

//=======================================================================
//function : Send
//purpose  : 发送 C 字符串消息（如果消息级别满足条件）
//
// 参数说明：
//   - theString：消息文本
//   - theGravity：消息的严重级别
//
// 说明：
//   - 这是一个包装函数，负责检查过滤条件
//   - 如果消息通过过滤，调用虚函数 send() 进行实际输出
//   - send() 是一个纯虚函数，由子类实现具体的输出逻辑
//
// 过滤逻辑：
//   - 如果 theGravity < myTraceLevel，消息被丢弃
//   - 否则，将消息转换为 ASCII 字符串并调用 send()
//
// 工作流程：
//   1. Messenger 调用 Printer::Send()
//   2. Printer 检查消息级别是否满足条件
//   3. 如果满足，调用虚函数 send() 进行实际处理
//   4. 子类的 send() 实现决定消息的输出方式
//=======================================================================
void Message_Printer::Send(const Standard_CString theString,
    const Message_Gravity  theGravity) const
{
    // 检查消息严重级别是否达到过滤阈值
    if (theGravity >= myTraceLevel)
    {
        // 将 C 字符串转换为 OCCT 的 ASCII 字符串，调用虚函数 send()
        send(TCollection_AsciiString(theString), theGravity);
    }
}

//=======================================================================
//function : Send
//purpose  : 发送扩展字符串消息（Unicode 支持）
//
// 说明：
//   - ExtendedString 支持 Unicode 和多字节字符
//   - 会被转换为 ASCII 字符串进行处理
//   - 适用于需要多语言支持的场景
//=======================================================================
void Message_Printer::Send(const TCollection_ExtendedString& theString,
    const Message_Gravity theGravity) const
{
    if (theGravity >= myTraceLevel)
    {
        // 将扩展字符串转换为 ASCII 字符串
        send(TCollection_AsciiString(theString), theGravity);
    }
}

//=======================================================================
//function : Send
//purpose  : 发送 ASCII 字符串消息
//
// 说明：
//   - ASCII 字符串是 OCCT 库内部的标准字符串格式
//   - 这是最直接的 Send 方法，无需转换
//=======================================================================
void Message_Printer::Send(const TCollection_AsciiString& theString,
    const Message_Gravity theGravity) const
{
    if (theGravity >= myTraceLevel)
    {
        // 直接调用虚函数处理 ASCII 字符串
        send(theString, theGravity);
    }
}

//=======================================================================
//function : SendStringStream
//purpose  : 发送流消息（如果消息级别满足条件）
//
// 参数说明：
//   - theStream：包含消息的字符串流
//   - theGravity：消息的严重级别
//
// 说明：
//   - 流对象可能包含多行内容或结构化数据
//   - 流会被转换为字符串再传给虚函数 send()
//   - 这种方法适合输出大量格式化数据
//
// 实现细节：
//   - theStream.str().c_str() 从流中提取 C 字符串
//   - str() 方法返回流的内容为 std::string
//   - c_str() 方法返回 C 风格的字符指针
//=======================================================================
void Message_Printer::SendStringStream(const Standard_SStream& theStream,
    const Message_Gravity   theGravity) const
{
    if (theGravity >= myTraceLevel)
    {
        // 从流中提取字符串内容并调用虚函数
        send(theStream.str().c_str(), theGravity);
    }
}

//=======================================================================
//function : SendObject
//purpose  : 发送对象消息
//
// 参数说明：
//   - theObject：要发送的对象
//   - theGravity：消息的严重级别
//
// 说明：
//   - 这个方法用于发送对象而不仅仅是文本
//   - 对象会被格式化为字符串：类型名 + 指针地址
//   - 例如输出：Message_Messenger: 0x12345678
//
// 过滤和处理：
//   - 首先检查对象是否有效（IsNull()）
//   - 然后检查消息级别是否满足条件
//   - 获取对象的动态类型名称和内存地址
//   - 将这些信息拼接成一个字符串
//
// 实现细节：
//   - DynamicType()->Name() 获取对象的运行时类型名称
//   - GetPointerInfo() 获取对象的内存地址和其他指针信息
//=======================================================================
void Message_Printer::SendObject(const Handle(Standard_Transient)& theObject,
    const Message_Gravity          theGravity) const
{
    // 同时检查：对象不为空 && 消息级别满足条件
    if (!theObject.IsNull()
        && theGravity >= myTraceLevel)
    {
        // 构造字符串：类型名称 + 冒号 + 指针信息
        // 例如："Message_Alert: 0x00000123456789AB"
        send(TCollection_AsciiString(theObject->DynamicType()->Name())
            + ": " + Standard_Dump::GetPointerInfo(theObject), theGravity);
    }
}
