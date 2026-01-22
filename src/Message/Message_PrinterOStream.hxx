// Created on: 2001-01-06
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

#ifndef _Message_PrinterOStream_HeaderFile
#define _Message_PrinterOStream_HeaderFile

#include <Message_ConsoleColor.hxx>
#include <Message_Printer.hxx>
#include <Standard_Address.hxx>
#include <Standard_OStream.hxx>

class Message_PrinterOStream;
DEFINE_STANDARD_HANDLE(Message_PrinterOStream, Message_Printer)

//! Implementation of a message printer associated with an std::ostream
//! The std::ostream may be either externally defined one (e.g. std::cout),
//! or file stream maintained internally (depending on constructor).
//!
//! 与 std::ostream 关联的消息打印器的实现
//! std::ostream 可以是外部定义的（例如 std::cout），
//! 或由内部维护的文件流（取决于构造函数）
class Message_PrinterOStream : public Message_Printer {
    DEFINE_STANDARD_RTTIEXT(Message_PrinterOStream, Message_Printer)
public:
    //! Setup console text color.
    //!
    //! On Windows, this would affect active terminal color output.
    //! On other systems, this would put special terminal codes;
    //! the terminal should support these codes or them will appear in text otherwise.
    //! The same will happen when stream is redirected into text file.
    //!
    //! Beware that within multi-threaded environment inducing console colors
    //! might lead to colored text mixture due to concurrency.
    //!
    //! 设置控制台文本颜色
    //!
    //! 在 Windows 上，这会影响活动终端的颜色输出。
    //! 在其他系统上，这会输出特殊的终端代码；
    //! 终端应支持这些代码，否则它们将在文本中显示。
    //! 当流重定向到文本文件时也会发生相同的情况。
    //!
    //! 注意：在多线程环境中，启用控制台颜色可能会因并发而导致彩色文本混合
    Standard_EXPORT static void SetConsoleTextColor(Standard_OStream* theOStream, Message_ConsoleColor theTextColor,
                                                    bool theIsIntenseText = false);

public:
    //! Empty constructor, defaulting to cout
    //! 空构造函数，默认输出到 cout
    Standard_EXPORT Message_PrinterOStream(const Message_Gravity theTraceLevel = Message_Info);

    //! Create printer for output to a specified file.
    //! The option theDoAppend specifies whether file should be
    //! appended or rewritten.
    //! For specific file names (cout, cerr) standard streams are used
    //!
    //! 创建用于输出到指定文件的打印器
    //! 选项 theDoAppend 指定是否应追加或重写文件
    //! 对于特定的文件名（cout、cerr），使用标准流
    Standard_EXPORT Message_PrinterOStream(const Standard_CString theFileName, const Standard_Boolean theDoAppend,
                                           const Message_Gravity theTraceLevel = Message_Info);

    //! Flushes the output stream and destroys it if it has been
    //! specified externally with option doFree (or if it is internal
    //! file stream)
    //!
    //! 刷新输出流，如果已通过选项指定外部指定或如果是内部文件流，则销毁它
    Standard_EXPORT void Close();
    ~Message_PrinterOStream() {
        Close();
    }

    //! Returns reference to the output stream
    //! 返回对输出流的引用
    Standard_OStream& GetStream() const {
        return *(Standard_OStream*)myStream;
    }

    //! Returns TRUE if text output into console should be colorized depending on message gravity; TRUE by default.
    //! 返回 TRUE 如果应根据消息严重性对输出到控制台的文本进行着色；默认为 TRUE
    Standard_Boolean ToColorize() const {
        return myToColorize;
    }

    //! Set if text output into console should be colorized depending on message gravity.
    //! 设置是否应根据消息严重性对输出到控制台的文本进行着色
    void SetToColorize(Standard_Boolean theToColorize) {
        myToColorize = theToColorize;
    }

protected:
    //! Puts a message to the current stream
    //! if its gravity is equal or greater
    //! to the trace level set by SetTraceLevel()
    //!
    //! 将消息放到当前流中，
    //! 如果其严重性等于或大于由 SetTraceLevel() 设置的跟踪级别
    Standard_EXPORT virtual void send(const TCollection_AsciiString& theString,
                                      const Message_Gravity theGravity) const Standard_OVERRIDE;

private:
    Standard_Address myStream;     // 输出流的地址
    Standard_Boolean myIsFile;     // 是否是文件流
    Standard_Boolean myToColorize; // 是否对文本着色
};

#endif // _Message_PrinterOStream_HeaderFile
