// Created on: 2001-01-18
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

#ifndef _Message_Msg_HeaderFile
#define _Message_Msg_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TColStd_SequenceOfInteger.hxx>

class TCollection_ExtendedString;
class TCollection_AsciiString;
class TCollection_HAsciiString;
class TCollection_HExtendedString;

//! This class provides a tool for constructing the parametrized message
//! basing on resources loaded by Message_MsgFile tool.
//!
//! A Message is created from a keyword: this keyword identifies the
//! message in a message file that should be previously loaded by call
//! to Message_MsgFile::LoadFile().
//!
//! The text of the message can contain placeholders for the parameters
//! which are to be filled by the proper values when the message
//! is prepared. Most of the format specifiers used in C can be used,
//! for instance, %s for string, %d for integer etc. In addition,
//! specifier %f is supported for double numbers (for compatibility
//! with previous versions).
//!
//! User fills the parameter fields in the text of the message by
//! calling corresponding methods Arg() or operators .
//!
//! The resulting message, filled with all parameters, can be obtained
//! by method Get(). If some parameters were not filled, the text
//! UNKNOWN is placed instead.
//!
//! 该类提供了一个工具，用于基于由 Message_MsgFile 工具加载的资源
//! 构造参数化消息
//!
//! 消息从关键字创建：此关键字标识消息文件中的消息，
//! 应通过调用 Message_MsgFile::LoadFile() 先前加载该消息文件
//!
//! 消息的文本可以包含参数的占位符，当准备消息时，
//! 这些占位符将被适当的值填充。可以使用 C 中使用的大多数格式说明符，
//! 例如，%s 用于字符串，%d 用于整数等。另外，
//! 对于双精度浮点数，支持 %f 说明符（为了向后兼容）
//!
//! 用户通过调用相应的 Arg() 方法或运算符来填充消息文本中的参数字段
//!
//! 可通过方法 Get() 获得用所有参数填充的最终消息。
//! 如果某些参数未填充，则改为放置文本 UNKNOWN
class Message_Msg {
public:
    DEFINE_STANDARD_ALLOC;

    //! Empty constructor
    //! 空构造函数
    Standard_EXPORT Message_Msg();

    //! Copy constructor
    //! 拷贝构造函数
    Standard_EXPORT Message_Msg(const Message_Msg& theMsg);

    //! Create a message using a corresponding entry in Message_MsgFile
    //! 使用 Message_MsgFile 中的对应条目创建消息
    Standard_EXPORT Message_Msg(const Standard_CString theKey);

    //! Create a message using a corresponding entry in Message_MsgFile
    //! 使用 Message_MsgFile 中的对应条目创建消息（扩展字符串）
    Standard_EXPORT Message_Msg(const TCollection_ExtendedString& theKey);

    //! Set a message body text -- can be used as alternative to
    //! using messages from resource file
    //!
    //! 设置消息体文本 -- 可作为使用资源文件中的消息的替代方案
    Standard_EXPORT void Set(const Standard_CString theMsg);

    //! Set a message body text -- can be used as alternative to
    //! using messages from resource file
    //!
    //! 设置消息体文本 -- 可作为使用资源文件中的消息的替代方案（扩展字符串）
    Standard_EXPORT void Set(const TCollection_ExtendedString& theMsg);

    //! Set a value for %..s conversion
    //! 为 %..s 转换设置值
    Standard_EXPORT Message_Msg& Arg(const Standard_CString theString);
    Message_Msg& operator<<(const Standard_CString theString) {
        return Arg(theString);
    }

    //! Set a value for %..s conversion
    //! 为 %..s 转换设置值（ASCII 字符串）
    Message_Msg& Arg(const TCollection_AsciiString& theString);
    Message_Msg& operator<<(const TCollection_AsciiString& theString) {
        return Arg(theString);
    }

    //! Set a value for %..s conversion
    //! 为 %..s 转换设置值（HAsciiString Handle）
    Message_Msg& Arg(const Handle(TCollection_HAsciiString) & theString);
    Message_Msg& operator<<(const Handle(TCollection_HAsciiString) & theString) {
        return Arg(theString);
    }

    //! Set a value for %..s conversion
    //! 为 %..s 转换设置值（扩展字符串）
    Standard_EXPORT Message_Msg& Arg(const TCollection_ExtendedString& theString);
    Message_Msg& operator<<(const TCollection_ExtendedString& theString) {
        return Arg(theString);
    }

    //! Set a value for %..s conversion
    //! 为 %..s 转换设置值（HExtendedString Handle）
    Message_Msg& Arg(const Handle(TCollection_HExtendedString) & theString);
    Message_Msg& operator<<(const Handle(TCollection_HExtendedString) & theString) {
        return Arg(theString);
    }

    //! Set a value for %..d, %..i, %..o, %..u, %..x or %..X conversion
    //! 为 %..d、%..i、%..o、%..u、%..x 或 %..X 转换设置值
    Standard_EXPORT Message_Msg& Arg(const Standard_Integer theInt);
    Message_Msg& operator<<(const Standard_Integer theInt) {
        return Arg(theInt);
    }

    //! Set a value for %..f, %..e, %..E, %..g or %..G conversion
    //! 为 %..f、%..e、%..E、%..g 或 %..G 转换设置值
    Standard_EXPORT Message_Msg& Arg(const Standard_Real theReal);
    Message_Msg& operator<<(const Standard_Real theReal) {
        return Arg(theReal);
    }

    //! Returns the original message text
    //! 返回原始消息文本
    const TCollection_ExtendedString& Original() const;

    //! Returns current state of the message text with
    //! parameters to the moment
    //!
    //! 返回消息文本的当前状态（包含到目前为止填充的参数）
    const TCollection_ExtendedString& Value() const;

    //! Tells if Value differs from Original
    //! 告诉 Value 是否与 Original 不同
    Standard_Boolean IsEdited() const;

    //! Return the resulting message string with all parameters
    //! filled. If some parameters were not yet filled by calls
    //! to methods Arg (or <<), these parameters are filled by
    //! the word UNKNOWN
    //!
    //! 返回所有参数都已填充的最终消息字符串。
    //! 如果某些参数尚未通过调用 Arg() 方法（或 <<）填充，
    //! 这些参数将被单词 UNKNOWN 填充
    Standard_EXPORT const TCollection_ExtendedString& Get();
    operator const TCollection_ExtendedString&() {
        return Get();
    }

protected:
private:
    // 获取格式说明符的类型和格式字符串
    Standard_EXPORT Standard_Integer getFormat(const Standard_Integer theType, TCollection_AsciiString& theFormat);

    // 替换消息文本中指定位置的子字符串
    Standard_EXPORT void replaceText(const Standard_Integer theFirst, const Standard_Integer theNb,
                                     const TCollection_ExtendedString& theStr);

    // 原始消息文本（未填充参数）
    TCollection_ExtendedString myOriginal;
    // 消息体文本（当前状态，可能已填充部分参数）
    TCollection_ExtendedString myMessageBody;
    // 格式说明符的序列
    TColStd_SequenceOfInteger mySeqOfFormats;
};

#include <Message_Msg.lxx>

#endif // _Message_Msg_HeaderFile
