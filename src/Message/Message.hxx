// Created on: 1999-07-29
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Message_HeaderFile
#define _Message_HeaderFile

#include <Message_Messenger.hxx>
#include <Message_Gravity.hxx>
#include <Message_MetricType.hxx>
#include <NCollection_Vector.hxx>
#include <OSD_MemInfo.hxx>

#include <TCollection_AsciiString.hxx>

class Message_Report;

//! Defines
//! - tools to work with messages
//! - basic tools intended for progress indication
//! 
//! 定义了
//! - 处理消息的工具
//! - 用于进度指示的基本工具
class Message
{
public:

    DEFINE_STANDARD_ALLOC;

    //! Defines default messenger for OCCT applications.
    //! This is global static instance of the messenger.
    //! By default, it contains single printer directed to std::cout.
    //! It can be customized according to the application needs.
    //!
    //! The following syntax can be used to print messages:
    //! @begincode
    //!   Message::DefaultMessenger()->Send ("My Warning", Message_Warning);
    //!   Message::SendWarning ("My Warning"); // short-cut for Message_Warning
    //!   Message::SendWarning() << "My Warning with " << theCounter << " arguments";
    //!   Message::SendFail ("My Failure"); // short-cut for Message_Fail
    //! @endcode
    //! 
    //! 为 OCCT 应用定义默认的消息传递器
    //! 这是消息传递器的全局静态实例
    //! 默认情况下，它包含一个指向 std::cout 的单一打印器
    //! 可以根据应用需求进行自定义
    //! 
    //! 可以使用以下语法来打印消息：
    //! @begincode
    //!   Message::DefaultMessenger()->Send ("我的警告", Message_Warning);
    //!   Message::SendWarning ("我的警告"); // Message_Warning 的简化形式
    //!   Message::SendWarning() << "我的警告 " << theCounter << " 个参数";
    //!   Message::SendFail ("我的失败"); // Message_Fail 的简化形式
    //! @endcode
    Standard_EXPORT static const Handle(Message_Messenger)& DefaultMessenger();

public:
    //!@name Short-cuts to DefaultMessenger
    //!@name 到 DefaultMessenger 的快速调用方法

    // 根据重要级别发送消息流缓冲
    static Message_Messenger::StreamBuffer Send(Message_Gravity theGravity)
    {
        return DefaultMessenger()->Send(theGravity);
    }

    // 发送带有指定重要级别的消息字符串
    static void Send(const TCollection_AsciiString& theMessage, Message_Gravity theGravity)
    {
        DefaultMessenger()->Send(theMessage, theGravity);
    }

    // 发送失败级别的消息
    static Message_Messenger::StreamBuffer SendFail() { return DefaultMessenger()->SendFail(); }
    // 发送警报级别的消息
    static Message_Messenger::StreamBuffer SendAlarm() { return DefaultMessenger()->SendAlarm(); }
    // 发送警告级别的消息
    static Message_Messenger::StreamBuffer SendWarning() { return DefaultMessenger()->SendWarning(); }
    // 发送信息级别的消息
    static Message_Messenger::StreamBuffer SendInfo() { return DefaultMessenger()->SendInfo(); }
    // 发送跟踪级别的消息
    static Message_Messenger::StreamBuffer SendTrace() { return DefaultMessenger()->SendTrace(); }

    // 发送失败级别的消息（带消息文本）
    static void SendFail(const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendFail(theMessage); }
    // 发送警报级别的消息（带消息文本）
    static void SendAlarm(const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendAlarm(theMessage); }
    // 发送警告级别的消息（带消息文本）
    static void SendWarning(const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendWarning(theMessage); }
    // 发送信息级别的消息（带消息文本）
    static void SendInfo(const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendInfo(theMessage); }
    // 发送跟踪级别的消息（带消息文本）
    static void SendTrace(const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendTrace(theMessage); }

public:

    //! Returns the string filled with values of hours, minutes and seconds.
    //! Example:
    //! 1. (5, 12, 26.3345) returns "05h:12m:26.33s",
    //! 2. (0,  6, 34.496 ) returns "06m:34.50s",
    //! 3. (0,  0,  4.5   ) returns "4.50s"
    //! 
    //! 返回用时、分、秒填充的字符串
    //! 示例：
    //! 1. (5, 12, 26.3345) 返回 "05h:12m:26.33s",
    //! 2. (0,  6, 34.496 ) 返回 "06m:34.50s",
    //! 3. (0,  0,  4.5   ) 返回 "4.50s"
    Standard_EXPORT static TCollection_AsciiString FillTime(const Standard_Integer Hour, const Standard_Integer Minute, const Standard_Real Second);

public:
    //! returns the only one instance of Report
    //! When theToCreate is true - automatically creates message report when not exist.
    //! 
    //! 返回唯一的 Report（报告）实例
    //! 当 theToCreate 为 true 时 - 如果报告不存在则自动创建它
    Standard_EXPORT static const Handle(Message_Report)& DefaultReport(const Standard_Boolean theToCreate = Standard_False);

    //! Determines the metric from the given string identifier.
    //! @param theString string identifier
    //! @param theType detected type of metric
    //! @return TRUE if string identifier is known
    //! 
    //! 从给定的字符串标识符确定度量类型
    //! @param theString 字符串标识符
    //! @param theType 检测到的度量类型
    //! @return 如果字符串标识符已知则返回 TRUE
    Standard_EXPORT static Standard_Boolean MetricFromString(const Standard_CString theString,
        Message_MetricType& theType);

    //! Returns the string name for a given metric type.
    //! @param theType metric type
    //! @return string identifier from the list of Message_MetricType
    //! 
    //! 返回给定度量类型的字符串名称
    //! @param theType 度量类型
    //! @return Message_MetricType 列表中的字符串标识符
    Standard_EXPORT static Standard_CString MetricToString(const Message_MetricType theType);

    //! Returns the metric type from the given string identifier.
    //! @param theString string identifier
    //! @return metric type or Message_MetricType_None if string identifier is invalid
    //! 
    //! 从给定的字符串标识符返回度量类型
    //! @param theString 字符串标识符
    //! @return 度量类型，如果字符串标识符无效则返回 Message_MetricType_None
    static Message_MetricType MetricFromString(const Standard_CString theString)
    {
        Message_MetricType aMetric = Message_MetricType_None;
        MetricFromString(theString, aMetric);
        return aMetric;
    }

    //! Converts message metric to OSD memory info type.
    //! @param theMetric [in] message metric
    //! @param theMemInfo [out] filled memory info type
    //! @return true if converted
    //! 
    //! 将消息度量转换为 OSD 内存信息类型
    //! @param theMetric [in] 消息度量
    //! @param theMemInfo [out] 填充的内存信息类型
    //! @return 如果转换成功返回 true
    static Standard_EXPORT Standard_Boolean ToOSDMetric(const Message_MetricType theMetric, OSD_MemInfo::Counter& theMemInfo);

    //! Converts OSD memory info type to message metric.
    //! @param theMemInfo [int] memory info type
    //! @param theMetric [out] filled message metric
    //! @return true if converted
    //! 
    //! 将 OSD 内存信息类型转换为消息度量
    //! @param theMemInfo [int] 内存信息类型
    //! @param theMetric [out] 填充的消息度量
    //! @return 如果转换成功返回 true
    static Standard_EXPORT Standard_Boolean ToMessageMetric(const OSD_MemInfo::Counter theMemInfo, Message_MetricType& theMetric);

};

#endif // _Message_HeaderFile
