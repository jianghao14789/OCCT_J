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

#ifdef _WIN32
#include <windows.h>
#endif

#include <Message_PrinterOStream.hxx>

#include <OSD_OpenFile.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

// 为 Message_PrinterOStream 类注册 RTTI
// 第二个参数 Message_Printer 表示基类
IMPLEMENT_STANDARD_RTTIEXT(Message_PrinterOStream, Message_Printer)

#if !defined(_MSC_VER)
#include <strings.h>
#endif

//=======================================================================
// function : Constructor
// purpose  : 空构造函数，默认输出到标准输出流（通常是控制台）
//
// 参数说明：
//   - theTraceLevel：消息的过滤级别（默认为 Message_Info）
//
// 说明：
//   - OStream 指的是 C++ 的输出流（Output Stream）
//   - 这个打印机将消息输出到 C++ 流对象
//   - 默认流是 std::cout（标准输出），通常显示在控制台
//
// 成员变量初始化：
//   - myStream：指向 std::cout 的指针
//   - myIsFile：设置为 false（表示这不是文件流）
//   - myToColorize：设置为 true（表示启用颜色输出）
//
// 彩色输出的优势：
//   - Info 消息显示为绿色，易于识别
//   - Warning 消息显示为黄色，表示需要注意
//   - Error 消息显示为红色，表示严重问题
//   - 提高了消息输出的可读性
//
// 流的概念：
//   - 流是 C++ 中处理输入/输出的抽象
//   - 类似于在管道中流动的数据
//   - std::cout 是一个特殊的流，连接到标准输出设备
//=======================================================================
Message_PrinterOStream::Message_PrinterOStream(const Message_Gravity theTraceLevel)
    : myStream(&std::cout), myIsFile(Standard_False), myToColorize(Standard_True) {
    myTraceLevel = theTraceLevel;
}

//=======================================================================
// function : Constructor
// purpose  : 打开文件作为输出流，或创建标准流
//
// 参数说明：
//   - theFileName：文件名或特殊流名
//     * "cout"：输出到标准输出
//     * "cerr"：输出到标准错误流
//     * 其他字符串：视为文件名，尝试打开该文件
//   - theToAppend：是否以追加模式打开文件
//     * Standard_True：在文件末尾追加内容（append mode）
//     * Standard_False：覆盖文件内容（write mode）
//   - theTraceLevel：消息的过滤级别
//
// 说明：
//   - 这个构造函数提供了灵活的输出选项
//   - 可以输出到标准流或自定义文件
//   - 文件操作由 OSD_OpenStream 处理
//
// 实现细节：
//   - strcasecmp()：不区分大小写的字符串比较
//   - 首先检查特殊文件名（"cout"、"cerr"）
//   - 如果不是特殊名，则尝试打开为文件
//   - 在 Windows 上，将正斜杠转换为反斜杠（路径分隔符）
//
// 示例：
//   // 输出到控制台
//   Message_PrinterOStream printer1("cout");
//
//   // 输出到错误流
//   Message_PrinterOStream printer2("cerr");
//
//   // 输出到文件（覆盖模式）
//   Message_PrinterOStream printer3("output.log", Standard_False);
//
//   // 输出到文件（追加模式）
//   Message_PrinterOStream printer4("output.log", Standard_True);
//=======================================================================
Message_PrinterOStream::Message_PrinterOStream(const Standard_CString theFileName, const Standard_Boolean theToAppend,
                                               const Message_Gravity theTraceLevel)
    : myStream(&std::cout), myIsFile(Standard_False), myToColorize(Standard_True) {
    myTraceLevel = theTraceLevel;
    // 检查是否要输出到错误流
    if (strcasecmp(theFileName, "cerr") == 0) {
        myStream = &std::cerr;
        return;
    }
    // 检查是否要输出到标准输出
    else if (strcasecmp(theFileName, "cout") == 0) {
        myStream = &std::cout;
        return;
    }

    // 转换为 OCCT 字符串便于处理
    TCollection_AsciiString aFileName(theFileName);
#ifdef _WIN32
    // 在 Windows 上，将正斜杠替换为反斜杠
    // 这是 Windows 系统的路径分隔符标准
    aFileName.ChangeAll('/', '\\');
#endif

    // 创建新的文件流对象
    std::ofstream* aFile = new std::ofstream();
    // 打开文件，选择模式：追加或覆盖
    OSD_OpenStream(*aFile, aFileName.ToCString(),
                   (theToAppend ? (std::ios_base::app | std::ios_base::out) : std::ios_base::out));

    // 检查文件是否成功打开
    if (aFile->is_open()) {
        // 成功打开：使用文件流
        myStream = (Standard_OStream*)aFile;
        myIsFile = Standard_True;
        myToColorize = Standard_False; // 文件不支持彩色
    } else {
        // 打开失败：清理资源，回退到标准输出
        delete aFile;
        myStream = &std::cout;
#ifdef OCCT_DEBUG
        // 在调试模式下输出错误消息
        std::cerr << "Error opening " << theFileName << std::endl << std::flush;
#endif
    }
}

//=======================================================================
// function : Close
// purpose  : 关闭输出流（如果是文件则关闭文件）
//
// 说明：
//   - 当不再需要输出时应该调用此方法
//   - 确保所有数据都被写入并且资源被释放
//   - 对于标准流（cout、cerr）不做任何操作
//   - 对于文件流，关闭文件句柄并释放内存
//
// 实现细节：
//   - flush()：强制将缓冲区中的数据写入目的地
//   - 如果是文件流，关闭文件并删除对象
//   - 将 myStream 设置为空以防止重复操作
//
// 何时调用：
//   - 打印机生命周期结束时
//   - 需要刷新和关闭文件时
//   - 不应该手动调用（通常由析构函数调用）
//=======================================================================

void Message_PrinterOStream::Close() {
    // 检查流是否有效
    if (!myStream) return;
    Standard_OStream* ostr = (Standard_OStream*)myStream;
    myStream = 0;

    // 刷新缓冲区：确保所有数据都被写出
    // flush() 强制将缓冲区数据写入底层设备
    ostr->flush();

    // 如果这是一个文件流，需要显式关闭
    if (myIsFile) {
        // 将通用流指针转换回文件流指针
        std::ofstream* ofile = (std::ofstream*)ostr;
        // 关闭文件
        ofile->close();
        // 删除对象，释放内存
        delete ofile;
        myIsFile = Standard_False;
    }
}

//=======================================================================
// function : send
// purpose  : 将消息发送到输出流，支持彩色文本（虚函数的实现）
//
// 参数说明：
//   - theString：要输出的消息文本
//   - theGravity：消息的严重级别
//
// 说明：
//   - 这个函数是基类 Message_Printer::send() 的实现
//   - 负责处理颜色设置和实际输出
//   - 当严重级别低于过滤阈值时，消息被忽略
//
// 工作流程：
//   1. 检查消息级别和流有效性
//   2. 根据消息级别确定颜色
//   3. 设置终端颜色（如果启用）
//   4. 输出消息
//   5. 恢复默认颜色
//
// 颜色映射表：
//   - Trace（跟踪）：黄色
//   - Info（信息）：亮绿色
//   - Warning（警告）：亮黄色
//   - Alarm（报警）：亮红色
//   - Fail（失败）：亮红色
//
// 颜色不支持的情况：
//   - 输出到文件时（myIsFile = true）
//   - myToColorize = false 时
//   - 某些终端环境下
//=======================================================================
void Message_PrinterOStream::send(const TCollection_AsciiString& theString, const Message_Gravity theGravity) const {
    // 双重检查：消息级别充分 && 流有效
    if (theGravity < myTraceLevel || myStream == NULL) {
        return;
    }

    // 根据严重级别选择文本颜色和亮度
    Message_ConsoleColor aColor = Message_ConsoleColor_Default;
    bool toIntense = false;
    // 只有在启用彩色且不是文件时才应用颜色
    if (myToColorize && !myIsFile) {
        switch (theGravity) {
            case Message_Trace:
                // 跟踪消息：正常黄色
                aColor = Message_ConsoleColor_Yellow;
                break;
            case Message_Info:
                // 信息消息：亮绿色
                aColor = Message_ConsoleColor_Green;
                toIntense = true;
                break;
            case Message_Warning:
                // 警告消息：亮黄色
                aColor = Message_ConsoleColor_Yellow;
                toIntense = true;
                break;
            case Message_Alarm:
                // 报警消息：亮红色
                aColor = Message_ConsoleColor_Red;
                toIntense = true;
                break;
            case Message_Fail:
                // 失败消息：亮红色
                aColor = Message_ConsoleColor_Red;
                toIntense = true;
                break;
        }
    }

    Standard_OStream* aStream = (Standard_OStream*)myStream;
    // 如果需要颜色（亮度或非默认颜色）
    if (toIntense || aColor != Message_ConsoleColor_Default) {
        // 设置颜色
        SetConsoleTextColor(aStream, aColor, toIntense);
        // 输出消息
        *aStream << theString;
        // 恢复默认颜色
        SetConsoleTextColor(aStream, Message_ConsoleColor_Default, false);
    } else {
        // 无需颜色，直接输出
        *aStream << theString;
    }
    // 输出换行符并刷新缓冲区
    (*aStream) << std::endl;
}

//=======================================================================
// function : SetConsoleTextColor
// purpose  : 设置控制台文本颜色（跨平台实现）
//
// 参数说明：
//   - theOStream：输出流
//   - theTextColor：要设置的颜色
//   - theIsIntenseText：是否使用亮度（粗体）
//
// 说明：
//   - 这个函数负责跨平台的颜色设置
//   - Windows 使用 API 调用
//   - Linux/Unix 使用 ANSI 转义序列
//   - Emscripten 不支持颜色
//
// ANSI 转义序列说明：
//   - "\e["：转义序列开始
//   - "30-37"：前景色代码（30=黑色，31=红色，等）
//   - "1"：粗体/高亮
//   - "m"：命令结束
//
// 示例：
//   - "\e[31m"：红色
//   - "\e[32m"：绿色
//   - "\e[33m"：黄色
//   - "\e[31;1m"：亮红色
//   - "\e[0m"：重置为默认
//=======================================================================
void Message_PrinterOStream::SetConsoleTextColor(Standard_OStream* theOStream, Message_ConsoleColor theTextColor,
                                                 bool theIsIntenseText) {
#ifdef _WIN32
    // Windows 实现：使用 Windows API 设置控制台属性
    // there is no difference between STD_OUTPUT_HANDLE/STD_ERROR_HANDLE for std::cout/std::cerr
    (void)theOStream; // 此参数在 Windows 实现中未使用

    // 获取标准输出的句柄（控制台设备）
    if (HANDLE anStdOut = GetStdHandle(STD_OUTPUT_HANDLE)) {
        WORD aFlags = 0; // 颜色标志
        // 如果需要高亮，添加强度位
        if (theIsIntenseText) {
            aFlags |= FOREGROUND_INTENSITY;
        }
        // 根据颜色设置相应的颜色位
        switch (theTextColor) {
            case Message_ConsoleColor_Default:
            case Message_ConsoleColor_White:
                // 白色 = 红 + 绿 + 蓝
                aFlags |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
            case Message_ConsoleColor_Black:
                // 黑色 = 无颜色
                break;
            case Message_ConsoleColor_Red:
                aFlags |= FOREGROUND_RED;
                break;
            case Message_ConsoleColor_Green:
                aFlags |= FOREGROUND_GREEN;
                break;
            case Message_ConsoleColor_Blue:
                aFlags |= FOREGROUND_BLUE;
                break;
            case Message_ConsoleColor_Yellow:
                // 黄色 = 红 + 绿
                aFlags |= FOREGROUND_RED | FOREGROUND_GREEN;
                break;
            case Message_ConsoleColor_Cyan:
                // 青色 = 绿 + 蓝
                aFlags |= FOREGROUND_GREEN | FOREGROUND_BLUE;
                break;
            case Message_ConsoleColor_Magenta:
                // 洋红色 = 红 + 蓝
                aFlags |= FOREGROUND_RED | FOREGROUND_BLUE;
                break;
        }
        // 应用颜色属性
        SetConsoleTextAttribute(anStdOut, aFlags);
    }
#elif defined(__EMSCRIPTEN__)
    // Emscripten（JavaScript）环境：不支持颜色
    // Terminal capabilities are undefined on this platform.
    // std::cout could be redirected to HTML page, into terminal or somewhere else.
    (void)theOStream;
    (void)theTextColor;
    (void)theIsIntenseText;
#else
    // Linux/Unix/Mac 实现：使用 ANSI 转义序列
    if (theOStream == NULL) {
        return;
    }

    // ANSI 颜色代码用于 Unix/Linux 终端
    const char* aCode = "\e[0m"; // 默认：重置颜色
    switch (theTextColor) {
        case Message_ConsoleColor_Default:
            // 默认颜色：不加粗返回 \e[0m，加粗返回 \e[0;1m
            aCode = theIsIntenseText ? "\e[0;1m" : "\e[0m";
            break;
        case Message_ConsoleColor_Black:
            aCode = theIsIntenseText ? "\e[30;1m" : "\e[30m";
            break;
        case Message_ConsoleColor_Red:
            aCode = theIsIntenseText ? "\e[31;1m" : "\e[31m";
            break;
        case Message_ConsoleColor_Green:
            aCode = theIsIntenseText ? "\e[32;1m" : "\e[32m";
            break;
        case Message_ConsoleColor_Yellow:
            aCode = theIsIntenseText ? "\e[33;1m" : "\e[33m";
            break;
        case Message_ConsoleColor_Blue:
            aCode = theIsIntenseText ? "\e[34;1m" : "\e[34m";
            break;
        case Message_ConsoleColor_Magenta:
            aCode = theIsIntenseText ? "\e[35;1m" : "\e[35m";
            break;
        case Message_ConsoleColor_Cyan:
            aCode = theIsIntenseText ? "\e[36;1m" : "\e[36m";
            break;
        case Message_ConsoleColor_White:
            aCode = theIsIntenseText ? "\e[37;1m" : "\e[37m";
            break;
    }
    // 输出 ANSI 转义序列到流
    *theOStream << aCode;
#endif
}
