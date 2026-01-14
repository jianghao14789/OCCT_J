// Created on: 2001-04-26
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

#ifndef _Message_MsgFile_HeaderFile
#define _Message_MsgFile_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class TCollection_AsciiString;
class TCollection_ExtendedString;

//! A tool providing facility to load definitions of message strings from
//! resource file(s).
//!
//! The message file is an ASCII file which defines a set of messages.
//! Each message is identified by its keyword (string).
//!
//! All lines in the file starting with the exclamation sign
//! (perhaps preceding by spaces and/or tabs) are ignored as comments.
//!
//! Each line in the file starting with the dot character "."
//! (perhaps preceding by spaces and/or tabs) defines the keyword.
//! The keyword is a string starting from the next symbol after dot
//! and ending at the symbol preceding ending newline character "\n".
//!
//! All the lines in the file after the keyword and before next
//! keyword (and which are not comments) define the message for that
//! keyword. If the message consists of several lines, the message
//! string will contain newline symbols "\n" between parts (but not
//! at the end).
//!
//! The experimental support of Unicode message files is provided.
//! These are distinguished by two bytes FF.FE or FE.FF at the beginning.
//!
//! The loaded messages are stored in static data map; all methods of that
//! class are also static.
//! 
//! 一个工具，提供从资源文件加载消息字符串定义的便利。
//! 
//! 消息文件是一个 ASCII 文件，定义一组消息。每个消息由其关键字（字符串）标识。
//! 
//! 文件中以感叹号 "!" 开头的所有行（可能在空格和/或制表符前面）都被忽略为注释。
//! 
//! 文件中以点字符 "." 开头的每一行（可能在空格和/或制表符前面）定义关键字。
//! 关键字是从点之后的下一个符号开始并在结束换行字符 "\n" 前面的符号处结束的字符串。
//! 
//! 文件中关键字之后和下一个关键字之前的所有行（不是注释的行）定义该关键字的消息。
//! 如果消息由多行组成，消息字符串将在各部分之间包含换行符 "\n"（但不在末尾）。
//! 
//! 提供了 Unicode 消息文件的实验性支持。这些文件在开头由两个字节 FF.FE 或 FE.FF 区分。
//! 
//! 加载的消息存储在静态数据映射中；该类的所有方法也是静态的。
class Message_MsgFile
{
public:

    DEFINE_STANDARD_ALLOC;

    //! Load message file <theFileName> from directory <theDirName>
    //! or its sub-directory
    //! 
    //! 从目录 <theDirName> 或其子目录加载消息文件 <theFileName>
    Standard_EXPORT static Standard_Boolean Load(const Standard_CString theDirName, const Standard_CString theFileName);

    //! Load the messages from the given file, additive to any previously
    //! loaded messages. Messages with same keywords, if already present,
    //! are replaced with the new ones.
    //! 
    //! 从给定文件加载消息，与任何先前加载的消息相加。
    //! 如果已经存在具有相同关键字的消息，则将其替换为新消息。
    Standard_EXPORT static Standard_Boolean LoadFile(const Standard_CString theFName);

    //! Loads the messages from the file with name (without extension) given by environment variable.
    //! Extension of the file name is given separately. If its not defined, it is taken:
    //! - by default from environment CSF_LANGUAGE,
    //! - if not defined either, as "us".
    //! @name theEnvName  environment variable name
    //! @name theFileName file name without language suffix
    //! @name theLangExt  language file name extension
    //! @return TRUE on success
    //! 
    //! 从由环境变量给出的名称（没有扩展名）的文件加载消息。文件名的扩展名单独给出。
    //! 如果未定义，则从以下位置获取：
    //! - 默认情况下来自环境变量 CSF_LANGUAGE，
    //! - 如果也未定义，则为 "us"。
    //! @param theEnvName  环境变量名称
    //! @param theFileName 不含语言后缀的文件名
    //! @param theLangExt  语言文件名扩展名
    //! @return 成功时返回 TRUE
    Standard_EXPORT static Standard_Boolean LoadFromEnv(const Standard_CString theEnvName,
        const Standard_CString theFileName,
        const Standard_CString theLangExt = "");

    //! Loads the messages from the given text buffer.
    //! @param theContent string containing the messages
    //! @param theLength  length of the buffer;
    //!                   when -1 specified - theContent will be considered as NULL-terminated string
    //! 
    //! 从给定的文本缓冲区加载消息。
    //! @param theContent 包含消息的字符串
    //! @param theLength  缓冲区的长度；
    //!                   当指定 -1 时 - theContent 将被视为 NULL 终止字符串
    Standard_EXPORT static Standard_Boolean LoadFromString(const Standard_CString theContent,
        const Standard_Integer theLength = -1);

    //! Adds new message to the map. Parameter <key> gives
    //! the key of the message, <text> defines the message itself.
    //! If there already was defined the message identified by the
    //! same keyword, it is replaced with the new one.
    //! 
    //! 向映射添加新消息。参数 <key> 给出消息的键，<text> 定义消息本身。
    //! 如果已经定义了由相同关键字标识的消息，则将其替换为新消息。
    Standard_EXPORT static Standard_Boolean AddMsg(const TCollection_AsciiString& key, const TCollection_ExtendedString& text);

    //! Returns True if message with specified keyword is registered
    //! 如果已注册具有指定关键字的消息，返回 True
    Standard_EXPORT static Standard_Boolean HasMsg(const TCollection_AsciiString& key);

    Standard_EXPORT static const TCollection_ExtendedString& Msg(const Standard_CString key);

    //! Gives the text for the message identified by the keyword <key>.
    //! If there are no messages with such keyword defined, the error message is returned.
    //! In that case reference to static string is returned, it can be changed with next call(s) to Msg().
    //! Note: The error message is constructed like 'Unknown message: <key>', and can
    //! itself be customized by defining message with key Message_Msg_BadKeyword.
    //! 
    //! 给出由关键字 <key> 标识的消息的文本。如果没有定义具有此类关键字的消息，则返回错误消息。
    //! 在这种情况下，返回对静态字符串的引用，可以通过后续对 Msg() 的调用来更改它。
    //! 注意：错误消息的格式为 'Unknown message: <key>'，可以通过定义键为 
    //! Message_Msg_BadKeyword 的消息来自定义。
    Standard_EXPORT static const TCollection_ExtendedString& Msg(const TCollection_AsciiString& key);

};

#endif // _Message_MsgFile_HeaderFile
