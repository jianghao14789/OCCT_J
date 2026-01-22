// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Message_Level_HeaderFile
#define _Message_Level_HeaderFile

#include <Message.hxx>
#include <Message_AlertExtended.hxx>
#include <Message_Gravity.hxx>
#include <Message_Messenger.hxx>
#include <Message_MetricType.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard.hxx>

//! This class is an instance of Sentry to create a level in a message report
//! Constructor of the class add new (active) level in the report, destructor removes it
//! While the level is active in the report, new alerts are added below the level root alert.
//!
//! The first added alert is a root alert, other are added below the root alert
//!
//! If alert has Message_AttributeMeter attribute, active metrics of the default report are stored in
//! the attribute: start value of metric on adding alert, stop on adding another alert or closing (delete) the level
//! in the report.
//!
//! Processing of this class is implemented in Message_Report, it is used only inside it.
//! Levels using should be only through using OCCT_ADD_MESSAGE_LEVEL_SENTRY only. No other code is required outside.
//!
//! 该类是在消息报告中创建级别的 Sentry 的实例
//! 该类的构造函数在报告中添加新的（活跃的）级别，析构函数删除它
//! 当该级别在报告中处于活跃状态时，新警报被添加到级别根警报下方
//!
//! 第一个添加的警报是根警报，其他警报添加在根警报下方
//!
//! 如果警报具有 Message_AttributeMeter 属性，默认报告的活跃指标存储在
//! 该属性中：添加警报时的指标起始值，添加另一个警报或关闭（删除）该级别时的停止值
//! 在报告中。
//!
//! 此类的处理在 Message_Report 中实现，仅在其内部使用。
//! 级别的使用应仅通过 OCCT_ADD_MESSAGE_LEVEL_SENTRY 进行。外部不需要其他代码。
class Message_Level {
public:
    //! Constructor.
    //! One string key is used for all alert meters.
    //! The perf meter is not started automatically, it will be done in AddAlert() method
    //!
    //! 构造函数。为所有警报计量表使用一个字符串键。
    //! 性能计量表不会自动启动，将在 AddAlert() 方法中执行
    Standard_EXPORT Message_Level(const TCollection_AsciiString& theName = TCollection_AsciiString());

    //! Assures stopping upon destruction
    //! 确保析构时停止
    Standard_EXPORT ~Message_Level();

    //! Returns root alert of the level
    //! @return alert instance or NULL
    //! 返回级别的根警报
    //! @return 警报实例或 NULL
    const Handle(Message_AlertExtended) & RootAlert() const {
        return myRootAlert;
    }

    //! Sets the root alert. Starts collects alert metrics if active.
    //! @param theAlert an alert
    //!
    //! 设置根警报。如果活跃，启动收集警报指标。
    //! @param theAlert 一个警报
    //! @param isRequiredToStart 是否需要启动度量
    Standard_EXPORT void SetRootAlert(const Handle(Message_AlertExtended) & theAlert,
                                      const Standard_Boolean isRequiredToStart);

    //! Adds new alert on the level. Stops the last alert metric, appends the alert and starts the alert metrics
    //! collecting. Sets root alert beforehand this method using, if the root is NULL, it does nothing.
    //! @param theGravity an alert gravity
    //! @param theAlert an alert
    //! @return true if alert is added
    //!
    //! 在级别上添加新警报。停止上一个警报指标，附加警报并启动警报指标收集。
    //! 在使用该方法之前先使用 SetRootAlert 设置根警报，如果根为 NULL，它什么也不做。
    //! @param theGravity 警报严重级别
    //! @param theAlert 一个警报
    //! @return 如果警报被添加则返回 true
    Standard_EXPORT Standard_Boolean AddAlert(const Message_Gravity theGravity, const Handle(Message_Alert) & theAlert);

private:
    //! Remove the current level from the report. It stops metric collecting for the last and the root alerts.
    //! 从报告中删除当前级别。它停止为最后一个和根警报收集指标。
    Standard_EXPORT void remove();

private:
    Handle(Message_AlertExtended) myRootAlert; //!< 根警报
                                               //!< root alert
    Handle(Message_AlertExtended) myLastAlert; //!< 在根警报上添加的最后一个警报
                                               //!< last added alert on the root alert
};

//! @def MESSAGE_NEW_LEVEL
//! Creates a new level instance of Sentry. This row should be inserted before messages using in the method.
//! @def OCCT_ADD_MESSAGE_LEVEL_SENTRY
//! 创建一个 Sentry 级别的新实例。此行应在方法中使用消息之前插入。
#define OCCT_ADD_MESSAGE_LEVEL_SENTRY(theMessage) Message_Level aLevel(theMessage);

#endif // _Message_Level_HeaderFile
