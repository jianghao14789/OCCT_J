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

#ifndef _Message_AlertExtended_HeaderFile
#define _Message_AlertExtended_HeaderFile

#include <Message_Alert.hxx>
#include <Message_Gravity.hxx>
#include <TCollection_AsciiString.hxx>

class Message_Attribute;
class Message_Report;

class Message_CompositeAlerts;

//! Inherited class of Message_Alert with some additional information.
//! It has Message_Attributes to provide the alert name, and other custom information
//! It has a container of composite alerts, if the alert might provide
//! sub-alerts collecting.
//! 
//! Message_Alert 的继承类，具有一些附加信息。
//! 它具有 Message_Attributes 来提供警报名称和其他自定义信息
//! 它具有复合警报的容器，如果警报可能提供子警报的收集。
class Message_AlertExtended : public Message_Alert
{
public:
    //! Creates new instance of the alert and put it into report with Message_Info gravity.
    //! It does nothing if such kind of gravity is not active in the report
    //! @param theReport the message report where new alert is placed
    //! @param theAttribute container of additional values of the alert
    //! @return created alert or NULL if Message_Info is not active in report
    //! 
    //! 创建警报的新实例并将其放入具有 Message_Info 严重性的报告中。
    //! 如果此类严重性在报告中不活跃，则不执行任何操作
    //! @param theReport 放置新警报的消息报告
    //! @param theAttribute 警报的附加值容器
    //! @return 创建的警报，如果 Message_Info 在报告中不活跃，则为 NULL
    Standard_EXPORT static Handle(Message_Alert) AddAlert(const Handle(Message_Report)& theReport,
        const Handle(Message_Attribute)& theAttribute,
        const Message_Gravity theGravity);

public:
    //! Empty constructor
    //! 空构造函数
    Message_AlertExtended() : Message_Alert() {}

    //! Return a C string to be used as a key for generating text user messages describing this alert.
    //! The messages are generated with help of Message_Msg class, in Message_Report::Dump().
    //! Base implementation returns dynamic type name of the instance.
    //! 
    //! 返回用作生成描述此警报的文本用户消息键的 C 字符串。
    //! 消息在 Message_Report::Dump() 中借助 Message_Msg 类生成。
    //! 基础实现返回实例的动态类型名称。
    Standard_EXPORT virtual Standard_CString GetMessageKey() const Standard_OVERRIDE;

    //! Returns container of the alert attributes
    //! 返回警报属性的容器
    const Handle(Message_Attribute)& Attribute() const { return myAttribute; }

    //! Sets container of the alert attributes
    //! @param theAttributes an attribute values
    //! 
    //! 设置警报属性的容器
    //! @param theAttribute 属性值
    void SetAttribute(const Handle(Message_Attribute)& theAttribute) { myAttribute = theAttribute; }

    //! Returns class provided hierarchy of alerts if created or create if the parameter is true
    //! @param theToCreate if composite alert has not been created for this alert, it should be created
    //! @return instance or NULL
    //! 
    //! 返回警报的类提供的层次结构（如果已创建）或在参数为 true 时创建
    //! @param theToCreate 如果尚未为此警报创建复合警报，应创建它
    //! @return 实例或 NULL
    Standard_EXPORT Handle(Message_CompositeAlerts) CompositeAlerts(const Standard_Boolean theToCreate = Standard_False);

    //! Return true if this type of alert can be merged with other
    //! of the same type to avoid duplication.
    //! Hierarchical alerts can not be merged
    //! Basis implementation returns true.
    //! 
    //! 返回 true 如果此类型的警报可以与同类型的其他警报合并以避免重复。
    //! 分层警报不能合并。基础实现返回 true。
    Standard_EXPORT virtual Standard_Boolean SupportsMerge() const Standard_OVERRIDE;

    //! If possible, merge data contained in this alert to theTarget.
    //! Base implementation always returns false.
    //! @return True if merged
    //! 
    //! 如果可能，将此警报中包含的数据合并到 theTarget。
    //! 基础实现始终返回 false。
    //! @return 如果合并则返回 True
    Standard_EXPORT virtual Standard_Boolean Merge(const Handle(Message_Alert)& theTarget) Standard_OVERRIDE;

    //! Dumps the content of me into the stream
    //! 将内容转储到流中
    virtual Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
        Standard_Integer theDepth = -1) const Standard_OVERRIDE;

    DEFINE_STANDARD_RTTIEXT(Message_AlertExtended, Message_Alert)

protected:

    Handle(Message_CompositeAlerts) myCompositAlerts; //!< 警报的类提供的分层结构
                                                      //!< class provided hierarchical structure of alerts
    Handle(Message_Attribute) myAttribute;            //!< 警报属性的容器
                                                      //!< container of the alert attributes
};

DEFINE_STANDARD_HANDLE(Message_AlertExtended, Message_Alert)

#endif // _Message_Alert_HeaderFile
