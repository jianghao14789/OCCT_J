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

#include <Message_Messenger.hxx>

#include <Message_Printer.hxx>
#include <Message_PrinterOStream.hxx>
#include <Standard_Dump.hxx>

// 为 Message_Messenger 类注册 RTTI（运行时类型信息）
IMPLEMENT_STANDARD_RTTIEXT(Message_Messenger, Standard_Transient)

//=======================================================================
// function : Message_Messenger
// purpose  : 默认构造函数，创建 Messenger 并自动添加一个标准输出流打印机
//
// 说明：
//   - Messenger 是消息路由的核心组件
//   - 它维护了一个打印机列表
//   - 当 Messenger 发送消息时，会将消息传递给所有已注册的打印机
//   - 每个打印机独立处理消息（例如输出到不同的目的地）
//
// 默认行为：
//   - 创建一个 Message_PrinterOStream 打印机
//   - 这个打印机输出到标准输出流（通常是控制台）
//
// 打印机的作用：
//   - 决定消息输出到哪里（控制台、文件、网络等）
//   - 决定消息的格式（纯文本、彩色文本、JSON 等）
//   - 决定消息的过滤（不同的严重级别可能有不同的处理）
//=======================================================================
Message_Messenger::Message_Messenger() {
    // 调用 AddPrinter 方法添加一个标准的输出流打印机
    // Message_PrinterOStream 是一个将消息输出到 C++ 输出流的打印机
    AddPrinter(new Message_PrinterOStream);
}

//=======================================================================
// function : Message_Messenger
// purpose  : 带参数的构造函数，创建 Messenger 并添加指定的打印机
//
// 参数说明：
//   - thePrinter：要添加的打印机对象
//
// 用途：
//   - 当需要自定义打印机时使用此构造函数
//   - 例如，创建输出到文件的 Messenger
//
// 示例：
//   Handle(Message_PrinterOStream) aFilePrinter =
//       new Message_PrinterOStream("output.log");
//   Handle(Message_Messenger) aMsg =
//       new Message_Messenger(aFilePrinter);
//=======================================================================

Message_Messenger::Message_Messenger(const Handle(Message_Printer) & thePrinter) {
    // 添加传入的打印机
    AddPrinter(thePrinter);
}

//=======================================================================
// function : AddPrinter
// purpose  : 向 Messenger 添加一个新的打印机
//
// 参数说明：
//   - thePrinter：要添加的打印机对象
//
// 返回值：
//   - Standard_True：成功添加打印机
//   - Standard_False：打印机已存在（不重复添加）
//
// 说明：
//   - Messenger 维护一个打印机列表（myPrinters）
//   - 同一个打印机不会被添加两次
//   - 当 Messenger 发送消息时，会遍历所有打印机并调用它们的 Send 方法
//
// 实现细节：
//   - 函数首先检查打印机是否已在列表中
//   - 使用迭代器遍历现有打印机
//   - 如果找到相同的打印机，返回 false
//   - 否则将新打印机添加到列表末尾
//=======================================================================

Standard_Boolean Message_Messenger::AddPrinter(const Handle(Message_Printer) & thePrinter) {
    // 检查打印机是否已在列表中
    // check whether printer is already in the list
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More(); aPrinterIter.Next()) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        // 如果找到相同的打印机，表示已存在
        if (aPrinter == thePrinter) {
            // 返回 false 表示未添加
            return Standard_False;
        }
    }

    // 将新打印机添加到列表末尾
    myPrinters.Append(thePrinter);
    return Standard_True;
}

//=======================================================================
// function : RemovePrinter
// purpose  : 从 Messenger 中移除指定的打印机
//
// 参数说明：
//   - thePrinter：要移除的打印机对象
//
// 返回值：
//   - Standard_True：成功移除
//   - Standard_False：打印机不存在
//
// 说明：
//   - 在移除打印机后，Messenger 将不再向其发送消息
//   - 通常在不再需要某个输出目的地时使用
//
// 实现细节：
//   - 遍历打印机列表查找目标打印机
//   - 找到后立即移除并返回 true
//   - 未找到则返回 false
//=======================================================================

Standard_Boolean Message_Messenger::RemovePrinter(const Handle(Message_Printer) & thePrinter) {
    // 在列表中查找打印机
    // find printer in the list
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More(); aPrinterIter.Next()) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        if (aPrinter == thePrinter) {
            // 找到了，从列表中移除
            myPrinters.Remove(aPrinterIter);
            return Standard_True;
        }
    }
    // 未找到
    return Standard_False;
}

//=======================================================================
// function : RemovePrinters
// purpose  : 从 Messenger 中移除所有指定类型的打印机
//
// 参数说明：
//   - theType：要移除的打印机类型（例如 Message_PrinterOStream）
//
// 返回值：
//   - 返回被移除的打印机数量
//
// 说明：
//   - 这个方法用于一次性移除所有特定类型的打印机
//   - 例如，可以一次性移除所有输出到文件的打印机
//
// 实现细节：
//   - 使用 IsKind() 方法检查打印机是否属于指定类型
//   - IsKind() 检查类型继承关系，包括基类
//   - 遍历列表时需要小心，因为列表大小可能改变
//=======================================================================

Standard_Integer Message_Messenger::RemovePrinters(const Handle(Standard_Type) & theType) {
    // 用于计数被移除的打印机数量
    // remove printers from the list
    Standard_Integer nb = 0;
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        // IsKind() 检查打印机是否是指定类型或其派生类
        if (!aPrinter.IsNull() && aPrinter->IsKind(theType)) {
            // 移除当前迭代器指向的打印机
            myPrinters.Remove(aPrinterIter);
            nb++;
        } else {
            // 如果不移除，则向前移动迭代器
            aPrinterIter.Next();
        }
    }
    return nb;
}

//=======================================================================
// function : Send
// purpose  : 向所有打印机发送 C 字符串消息
//
// 参数说明：
//   - theString：消息文本（C 风格的字符串）
//   - theGravity：消息的严重级别（Trace、Info、Warning、Alarm、Fail）
//
// 说明：
//   - Messenger 遍历所有已注册的打印机
//   - 每个打印机根据其设置决定是否处理该消息
//   - 严重级别决定消息的重要性
//
// 严重级别说明：
//   - Message_Trace：跟踪信息（最详细，通常用于调试）
//   - Message_Info：普通信息
//   - Message_Warning：警告信息
//   - Message_Alarm：报警信息（严重）
//   - Message_Fail：失败信息（最严重）
//
// 实现细节：
//   - 遍历 myPrinters 列表中的所有打印机
//   - 调用每个打印机的 Send 方法
//=======================================================================

void Message_Messenger::Send(const Standard_CString theString, const Message_Gravity theGravity) const {
    // 遍历所有打印机
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More(); aPrinterIter.Next()) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        // 确保打印机不为空
        if (!aPrinter.IsNull()) {
            // 调用打印机的 Send 方法
            aPrinter->Send(theString, theGravity);
        }
    }
}

//=======================================================================
// function : Send
// purpose  : 向所有打印机发送流消息
//
// 参数说明：
//   - theStream：包含消息的字符串流对象
//   - theGravity：消息的严重级别
//
// 说明：
//   - 这个版本接收一个 C++ 流对象（Standard_SStream）
//   - 流对象包含多行或结构化的消息
//   - 比 C 字符串更灵活，支持格式化输出
//=======================================================================
void Message_Messenger::Send(const Standard_SStream& theStream, const Message_Gravity theGravity) const {
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More(); aPrinterIter.Next()) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        if (!aPrinter.IsNull()) {
            // 调用打印机的 SendStringStream 方法处理流消息
            aPrinter->SendStringStream(theStream, theGravity);
        }
    }
}

//=======================================================================
// function : Send
// purpose  : 向所有打印机发送 ASCII 字符串消息
//
// 说明：
//   - ASCII 字符串是 OCCT 库内部常用的字符串格式
//   - 与 C 字符串相比，提供了更多的字符串操作方法
//=======================================================================
void Message_Messenger::Send(const TCollection_AsciiString& theString, const Message_Gravity theGravity) const {
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More(); aPrinterIter.Next()) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        if (!aPrinter.IsNull()) {
            aPrinter->Send(theString, theGravity);
        }
    }
}

//=======================================================================
// function : Send
// purpose  : 向所有打印机发送扩展字符串消息（支持 Unicode）
//
// 说明：
//   - ExtendedString 支持 Unicode 字符
//   - 用于需要多语言支持的应用
//=======================================================================

void Message_Messenger::Send(const TCollection_ExtendedString& theString, const Message_Gravity theGravity) const {
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More(); aPrinterIter.Next()) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        if (!aPrinter.IsNull()) {
            aPrinter->Send(theString, theGravity);
        }
    }
}

//=======================================================================
// function : Send
// purpose  : 向所有打印机发送对象消息
//
// 参数说明：
//   - theObject：要发送的对象（任何 OCCT 对象）
//
// 说明：
//   - 这个版本允许发送对象本身而不仅仅是文本
//   - 打印机可以决定如何序列化对象（例如转换为 JSON）
//=======================================================================
void Message_Messenger::Send(const Handle(Standard_Transient) & theObject, const Message_Gravity theGravity) const {
    for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More(); aPrinterIter.Next()) {
        const Handle(Message_Printer) & aPrinter = aPrinterIter.Value();
        if (!aPrinter.IsNull()) {
            // 调用打印机的 SendObject 方法
            aPrinter->SendObject(theObject, theGravity);
        }
    }
}

//=======================================================================
// function : DumpJson
// purpose  : 将 Messenger 的状态以 JSON 格式输出
//
// 说明：
//   - 输出 Messenger 中所有已注册的打印机信息
//   - 用于调试和监控 Messenger 的配置
//=======================================================================
void Message_Messenger::DumpJson(Standard_OStream& theOStream, Standard_Integer) const {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

    // 输出打印机列表的大小
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPrinters.Size())
}
