// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Message_PrinterToReport.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_Attribute.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_Report.hxx>

#include <Standard_Dump.hxx>
#include <TCollection_ExtendedString.hxx>

// 为 Message_PrinterToReport 类注册 RTTI
IMPLEMENT_STANDARD_RTTIEXT(Message_PrinterToReport, Message_Printer)

//=======================================================================
// function : Report
// purpose  : 获取报告实例，如果未设置则返回默认报告
//
// 说明：
//   - 这是一个内部辅助方法，用于获取关联的 Report
//   - 当打印机未显式设置报告时，使用全局默认报告
//   - 确保总是有一个有效的报告对象可以使用
//
// 返回值：
//   - 返回对 Message_Report 对象的常引用
//   - 保证返回值永远不为空
//
// 工作流程：
//   1. 检查 myReport 是否为空
//   2. 如果不为空，返回 myReport
//   3. 如果为空，调用 Message::DefaultReport() 获取全局默认报告
//   4. 参数 Standard_True 表示如果默认报告不存在，则创建它
//
// 使用场景：
//   - 当用户未设置专用报告时，消息被转发到全局默认报告
//   - 这确保所有消息都不会丢失
//
// 实现细节：
//   - IsNull() 检查 Handle（智能指针）是否为空
//   - Message::DefaultReport(Standard_True) 获取或创建默认报告
//   - 这样做的好处是用户可以选择使用专用报告或全局报告
//=======================================================================
const Handle(Message_Report) & Message_PrinterToReport::Report() const {
    // 检查是否设置了专用报告
    if (!myReport.IsNull()) {
        // 如果设置了，返回它
        return myReport;
    }

    // 如果未设置，返回全局默认报告
    // 参数 Standard_True 表示需要时创建默认报告
    return Message::DefaultReport(Standard_True);
}

//=======================================================================
// function : SendStringStream
// purpose  : 发送流消息到报告
//
// 参数说明：
//   - theStream：包含消息内容的字符串流
//   - theGravity：消息的严重级别
//
// 说明：
//   - 这个方法处理来自流的消息
//   - 流可能包含多行内容、格式化数据等
//   - 方法决定如何将流消息存储到警报中
//
// 工作流程：
//   1. 获取目标报告
//   2. 检查是否激活了度量
//   3. 如果激活了度量，调用 sendMetricAlert
//   4. 如果流包含子键（嵌套结构），创建 AttributeStream 警报
//   5. 否则，将流内容视为普通字符串
//
// 说明详解：
//   - Standard_Dump::HasChildKey() 检查流是否包含嵌套的结构化数据
//   - 如果包含，使用 Message_AttributeStream 保存整个流
//   - 如果不包含，使用普通 Message_Attribute 保存文本
//   - myName 是累积的名称前缀（从之前的调用保留）
//
// 使用场景示例：
//   1. 调试信息：包含复杂的对象状态转储（使用 AttributeStream）
//   2. 错误消息：简单的文本消息（使用 Attribute）
//   3. 日志信息：带有时间戳和上下文的多行输出（使用 AttributeStream）
//=======================================================================
void Message_PrinterToReport::SendStringStream(const Standard_SStream& theStream,
                                               const Message_Gravity theGravity) const {
    // 获取报告对象
    const Handle(Message_Report) & aReport = Report();

    // 检查是否激活了度量（性能监测）
    if (!aReport->ActiveMetrics().IsEmpty()) {
        // 如果激活了度量，发送度量警报
        // 将流转换为字符串传给 sendMetricAlert
        sendMetricAlert(theStream.str().c_str(), theGravity);
        return;
    }

    // 检查流中是否包含子键（嵌套的结构化数据）
    // HasChildKey() 检查是否有形如 "key: value" 的子结构
    if (Standard_Dump::HasChildKey(Standard_Dump::Text(theStream))) {
        // 如果有子键，使用 AttributeStream 保存整个流结构
        // 这样可以保留流的完整结构信息
        Message_AlertExtended::AddAlert(aReport, new Message_AttributeStream(theStream, myName), theGravity);
        // 清空累积的名称，供下一条消息使用
        myName.Clear();
    } else {
        // 流中没有子键，是简单的文本
        // 如果之前累积了名称前缀，先发送它
        if (!myName.IsEmpty()) {
            TCollection_AsciiString aName = myName;
            myName.Clear();
            // 递归调用 send() 方法发送名称
            send(aName, theGravity);
        }
        // 将流内容提取为文本
        myName = Standard_Dump::Text(theStream);
    }
}

//=======================================================================
// function : SendObject
// purpose  : 发送对象消息到报告
//
// 参数说明：
//   - theObject：要发送的对象
//   - theGravity：消息的严重级别
//
// 说明：
//   - 这个方法处理来自对象的消息
//   - 将对象附加到警报，以便保留对原始对象的引用
//   - 这对于定位和调试错误非常有用
//
// 工作流程：
//   1. 获取报告对象
//   2. 检查是否激活了度量
//   3. 如果激活了度量，调用 sendMetricAlert
//   4. 否则，创建 AttributeObject 警报并添加对象
//
// 使用场景：
//   - 当处理某个几何体时发生错误 -> 附加该几何体的引用
//   - 当处理某个文档时发生错误 -> 附加该文档的引用
//   - 当处理某个数据结构时发生错误 -> 附加该结构的引用
//
// 实现细节：
//   - myName 存储对象的描述性名称
//   - Message_AttributeObject 将对象和名称打包在一起
//   - Message_AlertExtended::AddAlert 负责创建警报并添加属性
//=======================================================================
void Message_PrinterToReport::SendObject(const Handle(Standard_Transient) & theObject,
                                         const Message_Gravity theGravity) const {
    // 获取报告对象
    const Handle(Message_Report) & aReport = Report();

    // 检查是否激活了度量
    if (!aReport->ActiveMetrics().IsEmpty()) {
        // 如果激活了度量，发送度量警报
        sendMetricAlert(myName, theGravity);
        return;
    }

    // 创建对象属性警报并添加到报告
    // Message_AttributeObject 用于存储对象引用和其名称
    Message_AlertExtended::AddAlert(aReport, new Message_AttributeObject(theObject, myName), theGravity);
}

//=======================================================================
// function : send
// purpose  : 发送字符串消息到报告（虚函数实现）
//
// 参数说明：
//   - theString：要发送的文本内容
//   - theGravity：消息的严重级别
//
// 说明：
//   - 这个方法是基类 Message_Printer::send() 的实现
//   - 负责处理基本的字符串消息
//   - 将消息作为警报添加到报告
//
// 工作流程：
//   1. 如果有累积的名称前缀，先递归发送它
//   2. 获取报告对象
//   3. 检查是否激活了度量
//   4. 根据情况创建相应的警报
//
// 递归发送名称的原因：
//   - 当多个 Send* 方法被连续调用时
//   - 名称会累积在 myName 中
//   - 需要确保它被正确处理和清空
//
// 使用示例：
//   Messenger 调用 send("Error in processing")
//   -> PrinterToReport::send() 创建警报
//   -> 警报被添加到 Report 中
//=======================================================================
void Message_PrinterToReport::send(const TCollection_AsciiString& theString, const Message_Gravity theGravity) const {
    // 如果有累积的名称，先发送它
    // 这确保名称和内容不会混淆
    if (!myName.IsEmpty()) {
        TCollection_AsciiString aName = myName;
        myName.Clear();
        // 递归调用 send() 发送名称
        send(aName, theGravity);
    }

    // 获取报告对象
    const Handle(Message_Report) & aReport = Report();

    // 检查是否激活了度量
    if (!aReport->ActiveMetrics().IsEmpty()) {
        // 如果激活了度量，发送度量警报
        sendMetricAlert(theString, theGravity);
        return;
    }

    // 创建普通属性警报并添加到报告
    // Message_Attribute 是最简单的属性，仅包含字符串内容
    Message_AlertExtended::AddAlert(aReport, new Message_Attribute(theString), theGravity);
}

//=======================================================================
// function : sendMetricAlert
// purpose  : 发送包含度量的警报到报告
//
// 参数说明：
//   - theValue：警报的文本描述
//   - theGravity：警报的严重级别
//
// 说明：
//   - 这个方法创建一个度量属性警报
//   - 当报告激活了度量监测时，所有警报都应该包含度量信息
//   - 度量包括：时间、内存使用、CPU 时间等
//
// 使用场景：
//   - 当需要监测处理时间和资源消耗时
//   - 例如："Processing took 2.5 seconds and used 128MB"
//   - 警报会自动捕获此时的所有激活度量
//
// 实现细节：
//   - Message_AttributeMeter 是用于度量的属性类
//   - Message_AttributeMeter::SetAlertMetrics() 自动捕获度量
//   - 因为度量设置发生在警报创建之后
//   - 所以警报包含的度量是创建时的快照
//=======================================================================
void Message_PrinterToReport::sendMetricAlert(const TCollection_AsciiString theValue,
                                              const Message_Gravity theGravity) const {
    // 创建一个度量属性并添加到报告
    // Message_AttributeMeter 会自动记录当前的各种度量
    Message_AlertExtended::AddAlert(Report(), new Message_AttributeMeter(theValue), theGravity);
}
