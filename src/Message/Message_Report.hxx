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

#ifndef _Message_Report_HeaderFile
#define _Message_Report_HeaderFile

#include <Message_Gravity.hxx>
#include <Message_Level.hxx>
#include <Message_ListOfAlert.hxx>
#include <Message_MetricType.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Mutex.hxx>

class Message_CompositeAlerts;
class Message_Messenger;

class Message_Report;

DEFINE_STANDARD_HANDLE(Message_Report, Standard_Transient)

//! Container for alert messages, sorted according to their gravity.
//!
//! For each gravity level, alerts are stored in simple list.
//! If alert being added can be merged with another alert of the same
//! type already in the list, it is merged and not added to the list.
//!
//! This class is intended to be used as follows:
//!
//! - In the process of execution, algorithm fills report by alert objects
//!   using methods AddAlert()
//!
//! - The result can be queried for presence of particular alert using
//!   methods HasAlert()
//!
//! - The reports produced by nested or sequentially executed algorithms
//!   can be collected in one using method Merge()
//!
//! - The report can be shown to the user either as plain text with method
//!   Dump() or in more advanced way, by iterating over lists returned by GetAlerts()
//!
//! - Report can be cleared by methods Clear() (usually after reporting)
//!
//! Message_PrinterToReport is a printer in Messenger to convert data sent to messenger into report
//!
//! 警报消息的容器，根据其严重级别排序
//!
//! 对于每个严重级别，警报存储在简单列表中。
//! 如果要添加的警报可以与列表中已有的同类型警报合并，
//! 则将其合并并不添加到列表中。
//!
//! 该类的使用方式如下：
//!
//! - 在执行过程中，算法使用 AddAlert() 方法将警报对象添加到报告中
//!
//! - 可以使用 HasAlert() 方法查询结果中是否存在特定警报
//!
//! - 由嵌套或顺序执行的算法产生的报告可以使用 Merge() 方法收集为一个
//!
//! - 报告可以通过 Dump() 方法以纯文本形式显示给用户，或通过迭代 GetAlerts()
//!   返回的列表以更高级的方式显示
//!
//! - 报告可以通过 Clear() 方法清除（通常在报告后）
//!
//! Message_PrinterToReport 是 Messenger 中的打印器，用于将发送到 messenger 的数据转换为报告
class Message_Report : public Standard_Transient {
public:
    //! Empty constructor
    //! 空构造函数
    Standard_EXPORT Message_Report();

    //! Add alert with specified gravity.
    //! This method is thread-safe, i.e. alerts can be added from parallel threads safely.
    //!
    //! 添加具有指定严重级别的警报
    //! 此方法是线程安全的，即警报可以从并行线程安全地添加
    Standard_EXPORT void AddAlert(Message_Gravity theGravity, const Handle(Message_Alert) & theAlert);

    //! Returns list of collected alerts with specified gravity
    //! 返回具有指定严重级别的收集的警报列表
    Standard_EXPORT const Message_ListOfAlert& GetAlerts(Message_Gravity theGravity) const;

    //! Returns true if specific type of alert is recorded
    //! 如果记录了特定类型的警报，返回 true
    Standard_EXPORT Standard_Boolean HasAlert(const Handle(Standard_Type) & theType);

    //! Returns true if specific type of alert is recorded with specified gravity
    //! 如果用指定的严重级别记录了特定类型的警报，返回 true
    Standard_EXPORT Standard_Boolean HasAlert(const Handle(Standard_Type) & theType, Message_Gravity theGravity);

    //! Returns true if a report printer for the current report is registered in the messenger
    //! @param theMessenger the messenger. If it's NULL, the default messenger is used
    //!
    //! 返回 true 如果当前报告的报告打印器在 messenger 中注册
    //! @param theMessenger messenger。如果为 NULL，则使用默认的 messenger
    Standard_EXPORT Standard_Boolean IsActiveInMessenger(const Handle(Message_Messenger) & theMessenger = NULL) const;

    //! Creates an instance of Message_PrinterToReport with the current report and register it in messenger
    //! @param toActivate if true, activated else deactivated
    //! @param theMessenger the messenger. If it's NULL, the default messenger is used
    //!
    //! 创建 Message_PrinterToReport 实例并在 messenger 中注册它
    //! @param toActivate 如果为 true，则激活；否则停用
    //! @param theMessenger messenger。如果为 NULL，则使用默认的 messenger
    Standard_EXPORT void ActivateInMessenger(const Standard_Boolean toActivate,
                                             const Handle(Message_Messenger) & theMessenger = NULL);

    //! Updates internal flag IsActiveInMessenger.
    //! It becomes true if messenger contains at least one instance of Message_PrinterToReport.
    //! @param theMessenger the messenger. If it's NULL, the default messenger is used
    //!
    //! 更新内部标记 IsActiveInMessenger。
    //! 如果 messenger 包含至少一个 Message_PrinterToReport 实例，则为 true
    //! @param theMessenger messenger。如果为 NULL，则使用默认的 messenger
    Standard_EXPORT void UpdateActiveInMessenger(const Handle(Message_Messenger) & theMessenger = NULL);

    //! Add new level of alerts
    //! @param theLevel a level
    //!
    //! 添加新的警报级别
    //! @param theLevel 一个级别
    Standard_EXPORT void AddLevel(Message_Level* theLevel, const TCollection_AsciiString& theName);

    //! Remove level of alerts
    //! 移除警报级别
    Standard_EXPORT void RemoveLevel(Message_Level* theLevel);

    //! Clears all collected alerts
    //! 清除所有收集的警报
    Standard_EXPORT void Clear();

    //! Clears collected alerts with specified gravity
    //! 清除具有指定严重级别的收集的警报
    Standard_EXPORT void Clear(Message_Gravity theGravity);

    //! Clears collected alerts with specified type
    //! 清除具有指定类型的收集的警报
    Standard_EXPORT void Clear(const Handle(Standard_Type) & theType);

    //! Returns computed metrics when alerts are performed
    //! 返回执行警报时计算的指标
    const NCollection_IndexedMap<Message_MetricType>& ActiveMetrics() const {
        return myActiveMetrics;
    }

    //! Sets metrics to compute when alerts are performed
    //! @param theMetrics container of metrics
    //!
    //! 设置执行警报时要计算的指标
    //! @param theMetricType 度量类型
    //! @param theActivate 是否激活此度量
    Standard_EXPORT void SetActiveMetric(const Message_MetricType theMetricType, const Standard_Boolean theActivate);

    //! Removes all activated metrics
    //! 移除所有激活的指标
    void ClearMetrics() {
        myActiveMetrics.Clear();
    }

    //! Returns maximum number of collecting alerts. If the limit is achieved,
    //! first alert is removed, the new alert is added in the container.
    //! @return the limit value
    //!
    //! 返回收集警报的最大数量。如果达到限制，
    //! 将移除第一个警报，新警报被添加到容器中。
    //! @return 限制值
    Standard_Integer Limit() const {
        return myLimit;
    }

    //! Sets maximum number of collecting alerts.
    //! @param theLimit limit value
    //!
    //! 设置收集警报的最大数量
    //! @param theLimit 限制值
    void SetLimit(const Standard_Integer theLimit) {
        myLimit = theLimit;
    }

    //! Dumps all collected alerts to stream
    //! 将所有收集的警报转储到流
    Standard_EXPORT void Dump(Standard_OStream& theOS);

    //! Dumps collected alerts with specified gravity to stream
    //! 将具有指定严重级别的收集的警报转储到流
    Standard_EXPORT void Dump(Standard_OStream& theOS, Message_Gravity theGravity);

    //! Sends all collected alerts to messenger.
    //! 将所有收集的警报发送到 messenger
    Standard_EXPORT virtual void SendMessages(const Handle(Message_Messenger) & theMessenger);

    //! Dumps collected alerts with specified gravity to messenger.
    //! Default implementation creates Message_Msg object with a message
    //! key returned by alert, and sends it in the messenger.
    //!
    //! 将具有指定严重级别的收集的警报转储到 messenger。
    //! 默认实现创建 Message_Msg 对象，使用警报返回的消息键，
    //! 并在 messenger 中发送它
    Standard_EXPORT virtual void SendMessages(const Handle(Message_Messenger) & theMessenger,
                                              Message_Gravity theGravity);

    //! Merges data from theOther report into this
    //! 将 theOther 报告中的数据合并到此报告中
    Standard_EXPORT void Merge(const Handle(Message_Report) & theOther);

    //! Merges alerts with specified gravity from theOther report into this
    //! 将 theOther 报告中具有指定严重级别的警报合并到此报告中
    Standard_EXPORT void Merge(const Handle(Message_Report) & theOther, Message_Gravity theGravity);

    //! Dumps the content of me into the stream
    //! 将内容转储到流中
    Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

    // OCCT RTTI
    DEFINE_STANDARD_RTTIEXT(Message_Report, Standard_Transient)

protected:
    //! Returns class provided hierarchy of alerts if created or create if the parameter is true
    //! @param isCreate if composite alert has not been created for this alert, it should be created
    //! @return instance or NULL
    //!
    //! 返回警报的类层次结构（如果已创建）或在参数为 true 时创建
    //! @param isCreate 如果尚未为此警报创建复合警报，应创建它
    //! @return 实例或 NULL
    Standard_EXPORT const Handle(Message_CompositeAlerts) &
        compositeAlerts(const Standard_Boolean isCreate = Standard_False);

    //! Sends alerts to messenger
    //! 将警报发送到 messenger
    Standard_EXPORT void sendMessages(const Handle(Message_Messenger) & theMessenger, Message_Gravity theGravity,
                                      const Handle(Message_CompositeAlerts) & theCompositeAlert);

    //! Dumps collected alerts with specified gravity to stream
    //! 将具有指定严重级别的收集的警报转储到流
    Standard_EXPORT void dumpMessages(Standard_OStream& theOS, Message_Gravity theGravity,
                                      const Handle(Message_CompositeAlerts) & theCompositeAlert);

protected:
    Standard_Mutex myMutex; // 用于线程安全的互斥锁

    Handle(Message_CompositeAlerts) myCompositAlerts; //!< 警报容器
                                                      //!< container of alerts

    NCollection_Sequence<Message_Level*>
        myAlertLevels; //!< 活动级别容器，新警报添加在最新级别下方
                       //!< container of active levels, new alerts are added below the latest level
    NCollection_IndexedMap<Message_MetricType> myActiveMetrics; //!< 警报上计算的指标
                                                                //!< metrics to compute on alerts

    Standard_Integer myLimit;               //!< 顶级收集的最大警报数
                                            //!< Maximum number of collected alerts on the top level
    Standard_Boolean myIsActiveInMessenger; //! 报告是否在 messenger 中激活的状态
                                            //! state whether the report is activated in messenger
};

#endif // _Message_Report_HeaderFile
