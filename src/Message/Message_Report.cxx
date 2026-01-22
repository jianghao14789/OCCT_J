// Created on: 2017-06-26
// Created by: Andrey Betenev
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Message_Report.hxx>

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_Attribute.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterToReport.hxx>

#include <Precision.hxx>
#include <Standard_Dump.hxx>

// 为 Message_Report 类注册 RTTI
IMPLEMENT_STANDARD_RTTIEXT(Message_Report, Standard_Transient)

//=======================================================================
// function : Message_Report
// purpose  : 构造函数，初始化报告对象
//
// 说明：
//   - Report 是警报的集中管理器
//   - 所有产生的警报都被收集到这个对象中
//   - 可以之后统一处理、查询或发送警报
//
// 成员初始化：
//   - myLimit：最大警报数量限制（-1 表示无限制）
//   - myIsActiveInMessenger：是否在 Messenger 中激活（初始为关闭）
//   - myMutex：线程互斥锁（确保线程安全）
//   - myCompositAlerts：复合警报容器（初始为空）
//=======================================================================

Message_Report::Message_Report() : myLimit(-1), myIsActiveInMessenger(Standard_False) {}

//=======================================================================
// function : AddAlert
// purpose  : 向报告中添加警报
//
// 参数说明：
//   - theGravity：警报的严重级别（Trace、Info、Warning、Alarm、Fail）
//   - theAlert：要添加的警报对象
//
// 说明：
//   - 这是报告收集警报的主要方法
//   - 支持两种警报结构：
//     * 顶级警报（myAlertLevels 为空）
//     * 嵌套警报（myAlertLevels 非空）
//   - 如果警报数量超过限制，删除最早的警报
//   - 使用线程锁确保多线程环境中的安全
//
// 工作流程：
//   1. 获取互斥锁（防止并发修改）
//   2. 检查是否有警报级别
//   3. 如果没有，添加到顶级复合警报
//   4. 如果有，添加到最后的警报级别
//   5. 检查警报数量是否超过限制
//=======================================================================

void Message_Report::AddAlert(Message_Gravity theGravity, const Handle(Message_Alert) & theAlert) {
    // 获取互斥锁，确保线程安全
    // Standard_Mutex::Sentry 是一个 RAII 风格的锁守卫
    // 析构时自动释放锁
    Standard_Mutex::Sentry aSentry(myMutex);

    // 顶级警报：如果没有警报级别
    // alerts of the top level
    if (myAlertLevels.IsEmpty()) {
        // 获取或创建复合警报容器
        Handle(Message_CompositeAlerts) aCompositeAlert = compositeAlerts(Standard_True);
        // 尝试添加警报，如果成功则返回
        // AddAlert 会处理合并逻辑
        if (aCompositeAlert->AddAlert(theGravity, theAlert)) {
            return;
        }

        // 如果超过限制，移除最早的警报
        // 仅移除报告下的警报
        // remove alerts under the report only
        const Message_ListOfAlert& anAlerts = aCompositeAlert->Alerts(theGravity);
        if (anAlerts.Extent() > myLimit) {
            // Extent() 返回列表中的元素个数
            // First() 返回列表的第一个元素（最早添加的）
            aCompositeAlert->RemoveAlert(theGravity, anAlerts.First());
        }
        return;
    }

    // 如果有警报级别，新警报将放在最后一个级别下
    // if there are some levels of alerts, the new alert will be placed below the root
    myAlertLevels.Last()->AddAlert(theGravity, theAlert);
}

//=======================================================================
// function : GetAlerts
// purpose  : 获取给定严重级别的所有警报列表
//
// 参数说明：
//   - theGravity：要查询的警报严重级别
//
// 返回值：
//   - 返回对警报列表的常引用
//   - 如果该级别没有警报，返回空列表
//
// 说明：
//   - 这个方法用于遍历和查询警报
//   - 返回的引用指向内部数据结构
//   - 调用者不应修改返回的列表
//
// 使用示例：
//   Message_Report report;
//   // ...添加一些警报...
//   const Message_ListOfAlert& warnings = report.GetAlerts(Message_Warning);
//   for (Message_ListOfAlert::Iterator it(warnings); it.More(); it.Next())
//   {
//       // 处理每个警报
//   }
//=======================================================================

const Message_ListOfAlert& Message_Report::GetAlerts(Message_Gravity theGravity) const {
    // 空列表常量，用作回退
    static const Message_ListOfAlert anEmptyList;
    // 如果复合警报不存在，返回空列表
    if (myCompositAlerts.IsNull()) {
        return anEmptyList;
    }
    // 否则返回对应级别的警报列表
    return myCompositAlerts->Alerts(theGravity);
}

//=======================================================================
// function : HasAlert
// purpose  : 检查是否存在给定类型的警报（任何严重级别）
//
// 参数说明：
//   - theType：要查找的警报类型（例如 Message_AttributeObject）
//
// 返回值：
//   - Standard_True：如果找到该类型的警报
//   - Standard_False：如果没有找到
//
// 说明：
//   - 这个方法遍历所有严重级别
//   - 检查是否存在给定类型的警报
//   - 只要找到一个就返回 true
//
// 实现细节：
//   - 遍历所有警报级别（从 Message_Trace 到 Message_Fail）
//   - 对每个级别调用 HasAlert(type, gravity)
//   - 如果任何一个返回 true，此方法返回 true
//=======================================================================

Standard_Boolean Message_Report::HasAlert(const Handle(Standard_Type) & theType) {
    // 遍历所有严重级别
    for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity) {
        if (HasAlert(theType, (Message_Gravity)iGravity)) return Standard_True;
    }
    return Standard_False;
}

//=======================================================================
// function : HasAlert
// purpose  : 检查是否存在给定类型和严重级别的警报
//
// 参数说明：
//   - theType：要查找的警报类型
//   - theGravity：要查找的警报严重级别
//
// 返回值：
//   - Standard_True：如果找到该类型和级别的警报
//   - Standard_False：如果没有找到
//
// 说明：
//   - 更具体的查询方法
//   - 同时指定了警报的类型和严重级别
//   - 提高了查询效率
//=======================================================================

Standard_Boolean Message_Report::HasAlert(const Handle(Standard_Type) & theType, Message_Gravity theGravity) {
    // 如果没有警报容器，直接返回 false
    if (compositeAlerts().IsNull()) {
        return Standard_False;
    }

    // 委托给 CompositeAlerts 对象处理查询
    return compositeAlerts()->HasAlert(theType, theGravity);
}

//=======================================================================
// function : IsActiveInMessenger
// purpose  : 检查报告是否在 Messenger 中激活
//
// 说明：
//   - 当报告在 Messenger 中激活时，所有发送给 Messenger 的消息都会被拦截并添加到报告中
//   - 这允许将所有警报/消息集中管理到一个 Report 对象中
//   - 禁用时，消息不会被添加到报告
//
// 返回值：
//   - Standard_True：报告已在 Messenger 中激活
//   - Standard_False：报告未激活
//=======================================================================
Standard_Boolean Message_Report::IsActiveInMessenger(const Handle(Message_Messenger) &) const {
    return myIsActiveInMessenger;
}

//=======================================================================
// function : ActivateInMessenger
// purpose  : 在 Messenger 中激活或禁用报告
//
// 参数说明：
//   - toActivate：激活（true）或禁用（false）
//   - theMessenger：要操作的 Messenger 对象
//     * 如果为空，使用默认 Messenger
//
// 说明：
//   - 激活时：添加一个 Message_PrinterToReport 打印机到 Messenger
//     * 这个打印机会拦截所有消息并添加到报告中
//   - 禁用时：移除关联的打印机
//
// 使用示例：
//   Handle(Message_Report) report = new Message_Report();
//   report->ActivateInMessenger(Standard_True);  // 激活
//   // 现在所有通过 Message 发送的消息都会被添加到 report
//   report->ActivateInMessenger(Standard_False);  // 禁用
//   // 现在消息不再被添加到 report
//=======================================================================
void Message_Report::ActivateInMessenger(const Standard_Boolean toActivate,
                                         const Handle(Message_Messenger) & theMessenger) {
    // 如果状态未改变，不做任何操作
    if (toActivate == IsActiveInMessenger()) return;

    myIsActiveInMessenger = toActivate;
    // 使用指定的 Messenger，如果为空则使用默认的
    Handle(Message_Messenger) aMessenger = theMessenger.IsNull() ? Message::DefaultMessenger() : theMessenger;

    if (toActivate) {
        // 激活：创建并添加打印机
        Handle(Message_PrinterToReport) aPrinterToReport = new Message_PrinterToReport();
        aPrinterToReport->SetReport(this); // 将打印机与报告关联
        aMessenger->AddPrinter(aPrinterToReport);
    } else // 禁用
    {
        // 禁用：移除所有关联的打印机
        Message_SequenceOfPrinters aPrintersToRemove;
        // 遍历所有打印机，找到关联到此报告的 PrinterToReport
        for (Message_SequenceOfPrinters::Iterator anIterator(aMessenger->Printers()); anIterator.More();
             anIterator.Next()) {
            const Handle(Message_Printer) aPrinter = anIterator.Value();
            // 检查打印机是否是 PrinterToReport 类型
            if (aPrinter->IsKind(STANDARD_TYPE(Message_PrinterToReport)) &&
                Handle(Message_PrinterToReport)::DownCast(aPrinter)->Report() == this)
                aPrintersToRemove.Append(aPrinter);
        }
        // 移除收集到的所有打印机
        for (Message_SequenceOfPrinters::Iterator anIterator(aPrintersToRemove); anIterator.More(); anIterator.Next()) {
            aMessenger->RemovePrinter(anIterator.Value());
        }
    }
}

//=======================================================================
// function : UpdateActiveInMessenger
// purpose  : 更新报告在 Messenger 中的激活状态（检查实际状态）
//
// 说明：
//   - 这个方法同步 myIsActiveInMessenger 的状态
//   - 用于查询报告是否确实在指定的 Messenger 中有激活的打印机
//   - 应该在 Messenger 可能被外部修改后调用
//=======================================================================
void Message_Report::UpdateActiveInMessenger(const Handle(Message_Messenger) & theMessenger) {
    Handle(Message_Messenger) aMessenger = theMessenger.IsNull() ? Message::DefaultMessenger() : theMessenger;
    // 遍历 Messenger 中的所有打印机
    for (Message_SequenceOfPrinters::Iterator anIterator(aMessenger->Printers()); anIterator.More();
         anIterator.Next()) {
        // 如果找到关联到此报告的 PrinterToReport，标记为激活
        if (anIterator.Value()->IsKind(STANDARD_TYPE(Message_PrinterToReport)) &&
            Handle(Message_PrinterToReport)::DownCast(anIterator.Value())->Report() == this) {
            myIsActiveInMessenger = Standard_True;
            return;
        }
    }
    // 如果没有找到，标记为未激活
    myIsActiveInMessenger = Standard_False;
}

//=======================================================================
// function : AddLevel
// purpose  : 向报告中添加警报级别
//
// 参数说明：
//   - theLevel：要添加的警报级别对象
//   - theName：此级别的名称（例如 "Processing step 1"）
//
// 说明：
//   - 警报级别用于组织分层的警报结构
//   - 例如：在处理多个文件时，每个文件可以是一个级别
//   - 级别之间形成树形结构，便于组织复杂的警报信息
//
// 警报级别的用途：
//   - 帮助跟踪警报发生的上下文
//   - 支持嵌套的错误报告
//   - 提供更清晰的错误信息组织
//
// 实现细节：
//   - 创建根警报来代表此级别
//   - 为根警报添加属性（级别名称）
//   - 如果是第一个级别，添加到报告的复合警报
//   - 如果有前一个级别，添加到前一个级别下面
//=======================================================================
void Message_Report::AddLevel(Message_Level* theLevel, const TCollection_AsciiString& theName) {
    Standard_Mutex::Sentry aSentry(myMutex);

    // 将级别添加到列表
    myAlertLevels.Append(theLevel);

    // 为这个级别创建根警报
    Handle(Message_AlertExtended) aLevelRootAlert = new Message_AlertExtended();

    // 创建属性来标记级别
    Handle(Message_Attribute) anAttribute;
    if (!ActiveMetrics().IsEmpty()) {
        // 如果启用了度量，使用度量属性
        anAttribute = new Message_AttributeMeter(theName);
    } else {
        // 否则使用普通属性
        anAttribute = new Message_Attribute(theName);
    }
    aLevelRootAlert->SetAttribute(anAttribute);
    theLevel->SetRootAlert(aLevelRootAlert, myAlertLevels.Size() == 1);

    // 如果这是第一个级别，根警报应被添加到报告的复合警报
    if (myAlertLevels.Size() == 1) // 这是第一个级别
    {
        compositeAlerts(Standard_True)->AddAlert(Message_Info, theLevel->RootAlert());
    }
    // 如果有前一个级别，根警报应被添加到前一个级别下
    if (myAlertLevels.Size() > 1) // 根警报应被推入前一个级别下
    {
        // root alert of next levels should be pushed under the previous level
        Message_Level* aPrevLevel = myAlertLevels.Value(myAlertLevels.Size() - 1); // 前一个级别
        aPrevLevel->AddAlert(Message_Info, aLevelRootAlert);
    }
}

//=======================================================================
// function : RemoveLevel
// purpose  : 从报告中移除警报级别
//
// 参数说明：
//   - theLevel：要移除的警报级别对象
//
// 说明：
//   - 移除指定的级别及其所有子警报
//   - 通常在处理完一个上下文（如文件处理完成）时调用
//   - 会停止该级别相关的度量统计
//=======================================================================

void Message_Report::RemoveLevel(Message_Level* theLevel) {
    Standard_Mutex::Sentry aSentry(myMutex);

    // 从后向前遍历，移除相关级别
    for (int aLevelIndex = myAlertLevels.Size(); aLevelIndex >= 1; aLevelIndex--) {
        Message_Level* aLevel = myAlertLevels.Value(aLevelIndex);
        // 停止与此级别关联的度量
        Message_AttributeMeter::StopAlert(aLevel->RootAlert());

        myAlertLevels.Remove(aLevelIndex);
        if (aLevel == theLevel) {
            return;
        }
    }
}

//=======================================================================
// function : Clear
// purpose  : 清除所有警报
//
// 说明：
//   - 删除报告中的所有警报
//   - 同时清除所有警报级别
//   - 清空后报告回到初始状态
//=======================================================================
void Message_Report::Clear() {
    // 如果没有警报，直接返回
    if (compositeAlerts().IsNull()) {
        return;
    }

    Standard_Mutex::Sentry aSentry(myMutex);

    // 清除所有警报
    compositeAlerts()->Clear();
    // 清除所有级别
    myAlertLevels.Clear();
}

//=======================================================================
// function : Clear
// purpose  : 清除给定严重级别的所有警报
//
// 参数说明：
//   - theGravity：要清除的警报严重级别
//
// 说明：
//   - 只删除指定级别的警报
//   - 其他级别的警报保留
//   - 例如，可以只删除所有 Warning，保留 Fail 和 Alarm
//=======================================================================
void Message_Report::Clear(Message_Gravity theGravity) {
    if (compositeAlerts().IsNull()) {
        return;
    }

    Standard_Mutex::Sentry aSentry(myMutex);

    compositeAlerts()->Clear(theGravity);
    myAlertLevels.Clear();
}

//=======================================================================
// function : Clear
// purpose  : 清除给定类型的所有警报
//
// 参数说明：
//   - theType：要清除的警报类型
//
// 说明：
//   - 删除所有指定类型的警报（无论什么级别）
//   - 例如，清除所有与文件 I/O 相关的警报
//=======================================================================
void Message_Report::Clear(const Handle(Standard_Type) & theType) {
    if (compositeAlerts().IsNull()) {
        return;
    }

    Standard_Mutex::Sentry aSentry(myMutex);

    compositeAlerts()->Clear(theType);
    myAlertLevels.Clear();
}

//=======================================================================
// function : SetActiveMetric
// purpose  : 激活或禁用指定的度量类型
//
// 参数说明：
//   - theMetricType：度量类型（例如 Message_MetricType_MemPrivate）
//   - theActivate：激活（true）或禁用（false）
//
// 说明：
//   - 度量用于收集性能数据（内存、CPU 时间等）
//   - 激活后，警报会附带相关的度量信息
//   - 例如：记录警报时的内存使用情况
//
// 用途示例：
//   report->SetActiveMetric(Message_MetricType_MemPrivate, Standard_True);
//   report->SetActiveMetric(Message_MetricType_WallClock, Standard_True);
//   // 现在产生的警报会包含私有内存和实时时钟信息
//=======================================================================
void Message_Report::SetActiveMetric(const Message_MetricType theMetricType, const Standard_Boolean theActivate) {
    // 如果状态已经是要求的状态，不做任何操作
    if (theActivate == myActiveMetrics.Contains(theMetricType)) {
        return;
    }

    if (theActivate) {
        // 添加到激活度量集合
        myActiveMetrics.Add(theMetricType);
    } else {
        // 从激活度量集合移除
        myActiveMetrics.RemoveKey(theMetricType);
    }
}

//=======================================================================
// function : Dump
// purpose  : 将所有警报转储到输出流
//
// 参数说明：
//   - theOS：输出流
//
// 说明：
//   - 以人类可读的格式输出所有警报
//   - 遍历所有严重级别并依次输出
//   - 用于调试和日志记录
//=======================================================================

void Message_Report::Dump(Standard_OStream& theOS) {
    // 遍历所有严重级别
    for (int iGravity = Message_Trace; iGravity <= Message_Fail; ++iGravity) {
        Dump(theOS, (Message_Gravity)iGravity);
    }
}

//=======================================================================
// function : Dump
// purpose  : 将给定严重级别的所有警报转储到输出流
//
// 参数说明：
//   - theOS：输出流
//   - theGravity：要输出的警报严重级别
//
// 说明：
//   - 只输出指定级别的警报
//   - 用于生成关注特定类型警报的报告
//=======================================================================

void Message_Report::Dump(Standard_OStream& theOS, Message_Gravity theGravity) {
    if (compositeAlerts().IsNull()) {
        return;
    }

    // 委托给内部方法处理
    dumpMessages(theOS, theGravity, compositeAlerts());
}

//=======================================================================
// function : SendMessages
// purpose  : 向 Messenger 发送所有警报消息
//
// 参数说明：
//   - theMessenger：接收消息的 Messenger 对象
//
// 说明：
//   - 将报告中的所有警报转换为消息并发送给 Messenger
//   - Messenger 会将消息分发给所有已注册的打印机
//   - 用于重新发送或回放警报
//=======================================================================

void Message_Report::SendMessages(const Handle(Message_Messenger) & theMessenger) {
    // 遍历所有严重级别并发送
    for (int aGravIter = Message_Trace; aGravIter <= Message_Fail; ++aGravIter) {
        SendMessages(theMessenger, (Message_Gravity)aGravIter);
    }
}

//=======================================================================
// function : SendMessages
// purpose  : 向 Messenger 发送给定严重级别的警报消息
//
// 参数说明：
//   - theMessenger：接收消息的 Messenger 对象
//   - theGravity：要发送的警报严重级别
//
// 说明：
//   - 只发送指定级别的警报消息
//   - 用于有选择性地重新发送警报
//=======================================================================

void Message_Report::SendMessages(const Handle(Message_Messenger) & theMessenger, Message_Gravity theGravity) {
    if (compositeAlerts().IsNull()) {
        return;
    }

    // 委托给内部方法处理
    sendMessages(theMessenger, theGravity, compositeAlerts());
}

//=======================================================================
// function : Merge
// purpose  : 合并另一个报告的所有警报到此报告
//
// 参数说明：
//   - theOther：要合并的源报告
//
// 说明：
//   - 将另一个报告中的所有警报复制到此报告
//   - 两个报告的警报都会保留
//   - 用于合并来自不同来源的警报
//
// 使用示例：
//   Handle(Message_Report) report1 = new Message_Report();
//   Handle(Message_Report) report2 = new Message_Report();
//   // ...添加一些警报到 report1 和 report2...
//   report1->Merge(report2);
//   // 现在 report1 包含来自 report1 和 report2 的所有警报
//=======================================================================

void Message_Report::Merge(const Handle(Message_Report) & theOther) {
    // 遍历所有严重级别并合并
    for (int aGravIter = Message_Trace; aGravIter <= Message_Fail; ++aGravIter) {
        Merge(theOther, (Message_Gravity)aGravIter);
    }
}

//=======================================================================
// function : Merge
// purpose  : 合并另一个报告的给定严重级别的警报到此报告
//
// 参数说明：
//   - theOther：要合并的源报告
//   - theGravity：要合并的警报严重级别
//
// 说明：
//   - 只合并指定级别的警报
//=======================================================================

void Message_Report::Merge(const Handle(Message_Report) & theOther, Message_Gravity theGravity) {
    // 遍历源报告中此级别的所有警报
    for (Message_ListOfAlert::Iterator anIt(theOther->GetAlerts(theGravity)); anIt.More(); anIt.Next()) {
        // 将每个警报添加到此报告
        AddAlert(theGravity, anIt.Value());
    }
}

//=======================================================================
// function : compositeAlerts
// purpose  : 获取或创建复合警报容器（内部方法）
//
// 参数说明：
//   - isCreate：如果为 true 且容器不存在，则创建新的
//
// 返回值：
//   - 返回对复合警报容器的常引用
//   - 如果 isCreate 为 false 且容器不存在，返回空对象
//
// 说明：
//   - 这是一个内部方法，实现懒加载模式
//   - 只在需要时创建复合警报容器
//   - 这样可以节省内存，因为空报告不需要创建容器
//=======================================================================
const Handle(Message_CompositeAlerts) & Message_Report::compositeAlerts(const Standard_Boolean isCreate) {
    // 如果不存在且要求创建，则创建新的
    if (myCompositAlerts.IsNull() && isCreate) {
        myCompositAlerts = new Message_CompositeAlerts();
    }

    return myCompositAlerts;
}

//=======================================================================
// function : sendMessages
// purpose  : 向 Messenger 发送警报消息（内部递归方法）
//
// 参数说明：
//   - theMessenger：接收消息的 Messenger 对象
//   - theGravity：要发送的警报严重级别
//   - theCompositeAlert：包含要发送的警报的容器
//
// 说明：
//   - 这是一个递归方法，用于处理嵌套的警报结构
//   - 遍历给定容器中的所有警报
//   - 对于每个警报，尝试将其转换为 AlertExtended
//   - 如果是 AlertExtended，递归处理其子警报
//   - 用于支持警报的树形结构
//=======================================================================
void Message_Report::sendMessages(const Handle(Message_Messenger) & theMessenger, Message_Gravity theGravity,
                                  const Handle(Message_CompositeAlerts) & theCompositeAlert) {
    if (theCompositeAlert.IsNull()) {
        return;
    }

    // 获取此容器中给定级别的所有警报
    const Message_ListOfAlert& anAlerts = theCompositeAlert->Alerts(theGravity);
    for (Message_ListOfAlert::Iterator anIt(anAlerts); anIt.More(); anIt.Next()) {
        // 向 Messenger 发送警报的消息键
        theMessenger->Send(anIt.Value()->GetMessageKey(), theGravity);

        // 尝试将警报转换为 AlertExtended（扩展警报）
        // 扩展警报可能包含子警报
        Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anIt.Value());
        if (anExtendedAlert.IsNull()) {
            // 如果不是扩展警报，无子警报，继续下一个
            continue;
        }

        // 获取扩展警报内的复合警报
        Handle(Message_CompositeAlerts) aCompositeAlerts = anExtendedAlert->CompositeAlerts();
        if (aCompositeAlerts.IsNull()) {
            // 没有子警报，继续下一个
            continue;
        }

        // 递归处理子警报
        sendMessages(theMessenger, theGravity, aCompositeAlerts);
    }
}

//=======================================================================
// function : dumpMessages
// purpose  : 转储警报消息到输出流（内部递归方法）
//
// 参数说明：
//   - theOS：输出流
//   - theGravity：要转储的警报严重级别
//   - theCompositeAlert：包含要转储的警报的容器
//
// 说明：
//   - 与 sendMessages 类似，但输出到流而不是 Messenger
//   - 递归处理嵌套的警报结构
//   - 用于生成警报的文本报告
//=======================================================================
void Message_Report::dumpMessages(Standard_OStream& theOS, Message_Gravity theGravity,
                                  const Handle(Message_CompositeAlerts) & theCompositeAlert) {
    if (theCompositeAlert.IsNull()) {
        return;
    }

    // 获取此容器中给定级别的所有警报
    const Message_ListOfAlert& anAlerts = theCompositeAlert->Alerts(theGravity);
    for (Message_ListOfAlert::Iterator anIt(anAlerts); anIt.More(); anIt.Next()) {
        // 输出警报的消息键
        theOS << anIt.Value()->GetMessageKey() << std::endl;

        // 尝试转换为扩展警报
        Handle(Message_AlertExtended) anExtendedAlert = Handle(Message_AlertExtended)::DownCast(anIt.Value());
        if (anExtendedAlert.IsNull()) {
            continue;
        }

        // 递归输出子警报
        dumpMessages(theOS, theGravity, anExtendedAlert->CompositeAlerts());
    }
}

//=======================================================================
// function : DumpJson
// purpose  : 将报告对象导出为 JSON 格式（用于调试）
//=======================================================================
void Message_Report::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

    if (!myCompositAlerts.IsNull()) {
        OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCompositAlerts.get())
    }

    // 输出警报级别的数量
    Standard_Integer anAlertLevels = myAlertLevels.Size();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, anAlertLevels)

    // 输出激活的度量类型
    Standard_Integer anInc = 1;
    for (NCollection_IndexedMap<Message_MetricType>::Iterator anIterator(myActiveMetrics); anIterator.More();
         anIterator.Next()) {
        Message_MetricType anActiveMetric = anIterator.Value();
        OCCT_DUMP_FIELD_VALUE_NUMERICAL_INC(theOStream, anActiveMetric, anInc++)
    }

    // 输出其他设置
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLimit)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsActiveInMessenger)
}
