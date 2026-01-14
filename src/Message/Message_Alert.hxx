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

#ifndef _Message_Alert_HeaderFile
#define _Message_Alert_HeaderFile

#include <Standard_Type.hxx>

DEFINE_STANDARD_HANDLE(Message_Alert, Standard_Transient)

//! Base class of the hierarchy of classes describing various situations
//! occurring during execution of some algorithm or procedure.
//!
//! Alert should provide unique text identifier that can be used to distinguish 
//! particular type of alerts, e.g. to get text message string describing it. 
//! See method GetMessageKey(); by default, dynamic type name is used.
//!
//! Alert can contain some data. To avoid duplication of data, new alert
//! can be merged with another one of the same type. Method SupportsMerge() 
//! should return true if merge is supported; method Merge() should do the
//! merge if possible and return true in that case and false otherwise.
//! 
//! 描述在某个算法或过程执行期间发生的各种情况的类层次结构的基类
//! 
//! Alert 应提供唯一的文本标识符，可用于区分特定类型的警报，
//! 例如获取描述它的文本消息字符串。
//! 参见方法 GetMessageKey()；默认情况下，使用动态类型名称
//! 
//! Alert 可以包含一些数据。为了避免数据重复，新的 alert 
//! 可以与同类型的另一个合并。方法 SupportsMerge() 
//! 应在支持合并时返回 true；方法 Merge() 应尽可能进行合并，
//! 如果成功则返回 true，否则返回 false
class Message_Alert : public Standard_Transient
{
public:

    //! Return a C string to be used as a key for generating text user 
    //! messages describing this alert.
    //! The messages are generated with help of Message_Msg class, in
    //! Message_Report::Dump().
    //! Base implementation returns dynamic type name of the instance.
    //! 
    //! 返回一个 C 字符串，用作生成描述此警报的文本用户消息的键
    //! 消息在 Message_Report::Dump() 中借助 Message_Msg 类生成
    //! 基础实现返回实例的动态类型名称
    virtual Standard_EXPORT Standard_CString GetMessageKey() const;

    //! Return true if this type of alert can be merged with other
    //! of the same type to avoid duplication.
    //! Basis implementation returns true.
    //! 
    //! 返回 true 如果此类型的警报可以与同类型的其他警报合并以避免重复
    //! 基础实现返回 true
    virtual Standard_EXPORT Standard_Boolean SupportsMerge() const;

    //! If possible, merge data contained in this alert to theTarget.
    //! @return True if merged.
    //! Base implementation always returns true.
    //! 
    //! 如果可能，将此警报中包含的数据合并到 theTarget
    //! @return 如果合并则返回 True
    //! 基础实现始终返回 true
    virtual Standard_EXPORT Standard_Boolean Merge(const Handle(Message_Alert)& theTarget);

    //! Dumps the content of me into the stream
    //! 将内容转储到流中
    virtual Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

    // OCCT RTTI
    DEFINE_STANDARD_RTTIEXT(Message_Alert, Standard_Transient)
};

//! Macro allowing to define simple alert (without data) in single line of code
//! 允许在单行代码中定义简单警报（无数据）的宏
#define DEFINE_SIMPLE_ALERT(Alert) class Alert : public Message_Alert { DEFINE_STANDARD_RTTI_INLINE(Alert,Message_Alert) };

#endif // _Message_Alert_HeaderFile
