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

#ifndef _Message_Attribute_HeaderFile
#define _Message_Attribute_HeaderFile

#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

DEFINE_STANDARD_HANDLE(Message_Attribute, Standard_Transient)

//! Additional information of extended alert attribute
//! To provide other custom attribute container, it might be redefined.
//! 
//! 扩展警报属性的附加信息。
//! 要提供其他自定义属性容器，可能需要重新定义。
class Message_Attribute : public Standard_Transient
{
    DEFINE_STANDARD_RTTIEXT(Message_Attribute, Standard_Transient)
public:
    //! Empty constructor
    //! 空构造函数
    Standard_EXPORT Message_Attribute(const TCollection_AsciiString& theName = TCollection_AsciiString());

    //! Return a C string to be used as a key for generating text user messages describing this alert.
    //! The messages are generated with help of Message_Msg class, in Message_Report::Dump().
    //! Base implementation returns dynamic type name of the instance.
    //! 
    //! 返回用作生成描述此警报的文本用户消息键的 C 字符串。
    //! 消息在 Message_Report::Dump() 中借助 Message_Msg 类生成。
    //! 基础实现返回实例的动态类型名称。
    Standard_EXPORT virtual Standard_CString GetMessageKey() const;

    //! Returns custom name of alert if it is set
    //! @return alert name
    //! 返回警报的自定义名称（如果已设置）
    //! @return 警报名称
    const TCollection_AsciiString& GetName() const { return myName; }

    //! Sets the custom name of alert
    //! @param theName a name for the alert
    //! 设置警报的自定义名称
    //! @param theName 警报的名称
    void SetName(const TCollection_AsciiString& theName) { myName = theName; }

    //! Dumps the content of me into the stream
    //! 将内容转储到流中
    virtual Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
    TCollection_AsciiString myName; //!< 警报名称，如果定义，将在 GetMessageKey 中使用
                                    //!< alert name, if defined is used in GetMessageKey

};

#endif // _Message_Attribute_HeaderFile
