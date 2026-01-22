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

#ifndef _Message_PrinterToReport_HeaderFile
#define _Message_PrinterToReport_HeaderFile

#include <Message_Printer.hxx>
#include <Standard_Address.hxx>
#include <Standard_OStream.hxx>
#include <TCollection_AsciiString.hxx>

class Message_Report;

//! Implementation of a message printer associated with Message_Report
//! Send will create a new alert of the report. If string is sent, an alert is created by Eol only.
//! The alerts are sent into set report or default report of Message.
//!
//! 与 Message_Report 关联的消息打印器的实现
//! Send 将创建报告的新警报。如果发送字符串，警报仅由 Eol（行尾）创建。
//! 警报被发送到设置的报告或 Message 的默认报告。
class Message_PrinterToReport : public Message_Printer {
    DEFINE_STANDARD_RTTIEXT(Message_PrinterToReport, Message_Printer)
public:
    //! Create printer for redirecting messages into report.
    //! 创建用于将消息重定向到报告的打印器
    Message_PrinterToReport() {}

    //! Destructor
    //! 析构函数
    virtual ~Message_PrinterToReport() {}

    //! Returns the current or default report
    //! 返回当前或默认报告
    Standard_EXPORT const Handle(Message_Report) & Report() const;

    //! Sets the printer report
    //! @param theReport report for messages processing, if NULL, the default report is used
    //!
    //! 设置打印器报告
    //! @param theReport 用于消息处理的报告，如果为 NULL，则使用默认报告
    void SetReport(const Handle(Message_Report) & theReport) {
        myReport = theReport;
    }

    //! Send a string message with specified trace level.
    //! Stream is converted to string value.
    //! Default implementation calls first method Send().
    //!
    //! 发送带有指定跟踪级别的字符串消息（从流）。
    //! 流被转换为字符串值。默认实现调用第一个 Send() 方法。
    Standard_EXPORT virtual void SendStringStream(const Standard_SStream& theStream,
                                                  const Message_Gravity theGravity) const Standard_OVERRIDE;

    //! Send a string message with specified trace level.
    //! The object is converted to string in format: <object kind> : <object pointer>.
    //! The parameter theToPutEol specified whether end-of-line should be added to the end of the message.
    //! Default implementation calls first method Send().
    //!
    //! 发送带有指定跟踪级别的字符串消息（从 Transient 对象）。
    //! 对象被转换为字符串格式：<object kind> : <object pointer>。
    //! 参数 theToPutEol 指定是否应在消息末尾添加行尾。
    //! 默认实现调用第一个 Send() 方法。
    Standard_EXPORT virtual void SendObject(const Handle(Standard_Transient) & theObject,
                                            const Message_Gravity theGravity) const Standard_OVERRIDE;

protected:
    //! Send a string message with specified trace level.
    //! This method must be redefined in descendant.
    //! 发送带有指定跟踪级别的字符串消息。此方法必须在派生类中重新定义。
    Standard_EXPORT virtual void send(const TCollection_AsciiString& theString,
                                      const Message_Gravity theGravity) const Standard_OVERRIDE;

    //! Send an alert with metrics active in the current report
    //! 使用当前报告中活动的指标发送警报
    Standard_EXPORT void sendMetricAlert(const TCollection_AsciiString theValue,
                                         const Message_Gravity theGravity) const;

private:
    mutable TCollection_AsciiString myName; // 名称
    Handle(Message_Report) myReport;        //!< 用于发送警报的报告
                                            //!< the report for sending alerts
};

DEFINE_STANDARD_HANDLE(Message_PrinterToReport, Message_Printer)

#endif // _Message_PrinterToReport_HeaderFile
