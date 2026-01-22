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

#ifndef _Message_CompositeAlerts_HeaderFile
#define _Message_CompositeAlerts_HeaderFile

#include <Message_Alert.hxx>
#include <Message_Gravity.hxx>
#include <Message_ListOfAlert.hxx>
#include <Standard_Transient.hxx>

//! Class providing container of alerts
//! 提供警报容器的类
class Message_CompositeAlerts : public Standard_Transient {
    DEFINE_STANDARD_RTTIEXT(Message_CompositeAlerts, Standard_Transient)
public:
    //! Empty constructor
    //! 空构造函数
    Message_CompositeAlerts() {}

    //! Returns list of collected alerts with specified gravity
    //! 返回具有指定严重级别的收集的警报列表
    Standard_EXPORT const Message_ListOfAlert& Alerts(const Message_Gravity theGravity) const;

    //! Add alert with specified gravity. If the alert supports merge it will be merged.
    //! @param theGravity an alert gravity
    //! @param theAlert an alert to be added as a child alert
    //! @return true if the alert is added or merged
    //!
    //! 添加具有指定严重级别的警报。如果警报支持合并，它将被合并。
    //! @param theGravity 警报严重级别
    //! @param theAlert 要添加为子警报的警报
    //! @return 如果警报被添加或合并，返回 true
    Standard_EXPORT Standard_Boolean AddAlert(Message_Gravity theGravity, const Handle(Message_Alert) & theAlert);

    //! Removes alert with specified gravity.
    //! @param theGravity an alert gravity
    //! @param theAlert an alert to be removed from the children
    //! @return true if the alert is removed
    //!
    //! 删除具有指定严重级别的警报。
    //! @param theGravity 警报严重级别
    //! @param theAlert 要从子警报中删除的警报
    //! @return 如果警报被删除，返回 true
    Standard_EXPORT Standard_Boolean RemoveAlert(Message_Gravity theGravity, const Handle(Message_Alert) & theAlert);

    //! Returns true if the alert belong the list of the child alerts.
    //! @param theAlert an alert to be checked as a child alert
    //! @return true if the alert is found in a container of children
    //!
    //! 返回 true 如果警报属于子警报列表。
    //! @param theAlert 要作为子警报检查的警报
    //! @return 如果在子警报容器中找到警报，返回 true
    Standard_EXPORT Standard_Boolean HasAlert(const Handle(Message_Alert) & theAlert);

    //! Returns true if specific type of alert is recorded with specified gravity
    //! @param theType an alert type
    //! @param theGravity an alert gravity
    //! @return true if the alert is found in a container of children
    //!
    //! 返回 true 如果以指定的严重级别记录了特定类型的警报
    //! @param theType 警报类型
    //! @param theGravity 警报严重级别
    //! @return 如果在子警报容器中找到警报，返回 true
    Standard_EXPORT Standard_Boolean HasAlert(const Handle(Standard_Type) & theType, Message_Gravity theGravity);

    //! Clears all collected alerts
    //! 清除所有收集的警报
    Standard_EXPORT void Clear();

    //! Clears collected alerts with specified gravity
    //! @param theGravity an alert gravity
    //! 清除具有指定严重级别的收集的警报
    //! @param theGravity 警报严重级别
    Standard_EXPORT void Clear(Message_Gravity theGravity);

    //! Clears collected alerts with specified type
    //! @param theType an alert type
    //! 清除具有指定类型的收集的警报
    //! @param theType 警报类型
    Standard_EXPORT void Clear(const Handle(Standard_Type) & theType);

    //! Dumps the content of me into the stream
    //! 将内容转储到流中
    Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:
    // store messages in a lists sorted by gravity;
    // here we rely on knowledge that Message_Fail is the last element of the enum
    // 在按严重级别排序的列表中存储消息；这里我们依赖于 Message_Fail 是枚举的最后一个元素的知识
    Message_ListOfAlert myAlerts[Message_Fail + 1]; //!< 每种严重级别的子警报的容器
                                                    //!< container of child alert for each type of gravity
};

DEFINE_STANDARD_HANDLE(Message_CompositeAlerts, Standard_Transient)

#endif // _Message_CompositeAlerts_HeaderFile
