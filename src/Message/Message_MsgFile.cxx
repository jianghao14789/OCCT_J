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

#include <Message_MsgFile.hxx>

#include <NCollection_Buffer.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Mutex.hxx>
#include <OSD_OpenFile.hxx>

#include <stdlib.h>
#include <stdio.h>

// 扩展字符串的数据映射
typedef NCollection_DataMap<TCollection_AsciiString, TCollection_ExtendedString> Message_DataMapOfExtendedString;

// 消息数据映射的全局实例
static Message_DataMapOfExtendedString& msgsDataMap() {
    static Message_DataMapOfExtendedString aDataMap;
    return aDataMap;
}

// 用于防止对消息注册表的并发访问的互斥锁
// mutex used to prevent concurrent access to message registry
static Standard_Mutex& Message_MsgFile_Mutex() {
    static Standard_Mutex theMutex;
    return theMutex;
}

// 文件加载状态枚举
typedef enum {
    MsgFile_WaitingKeyword,     // 等待关键字
    MsgFile_WaitingMessage,     // 等待消息
    MsgFile_WaitingMoreMessage, // 等待更多消息行
    MsgFile_Indefinite          // 未定义状态
} LoadingState;

//=======================================================================
// function : Message_MsgFile
// purpose  : 从给定的目录加载文件
//           theDirName 可能表示为列表："/ dirA / dirB / dirA / dirC"
//=======================================================================

Standard_Boolean Message_MsgFile::Load(const Standard_CString theDirName, const Standard_CString theFileName) {
    if (!theDirName || !theFileName) return Standard_False;

    Standard_Boolean ret = Standard_True;
    TCollection_AsciiString aDirList(theDirName);
    //  尝试从列表中的所有连续目录加载
    //  Try to load from all consecutive directories in list
    for (int i = 1;; i++) {
        TCollection_AsciiString aFileName = aDirList.Token(" \t\n", i);
        if (aFileName.IsEmpty()) break;
#ifdef _WIN32
        aFileName += '\\';
#else
        aFileName += '/';
#endif
        aFileName += theFileName;
        if (!LoadFile(aFileName.ToCString())) ret = Standard_False;
    }
    return ret;
}

//=======================================================================
// function : getString
// purpose  : 从 ASCII 或 Unicode 字符串中取出 TCollection_ExtendedString
//           字符串左修剪；以 '!' 开头的省略
// Called   : from loadFile()
//=======================================================================

template <typename CharType> struct TCollection_String;
template <> struct TCollection_String<Standard_Character> {
    typedef TCollection_AsciiString type;
};
template <> struct TCollection_String<Standard_ExtCharacter> {
    typedef TCollection_ExtendedString type;
};

template <class CharType>
static inline Standard_Boolean getString(CharType*& thePtr, TCollection_ExtendedString& theString,
                                         Standard_Integer& theLeftSpaces) {
    CharType* anEndPtr = thePtr;
    CharType* aPtr;
    Standard_Integer aLeftSpaces;

    do {
        //    跳过字符串开头的空白
        //    Skip whitespaces in the beginning of the string
        aPtr = anEndPtr;
        aLeftSpaces = 0;
        for (;;) {
            CharType aChar = *aPtr;
            if (aChar == ' ')
                aLeftSpaces++;
            else if (aChar == '\t')
                aLeftSpaces += 8;
            else if (aChar == '\r' || *aPtr == '\n')
                aLeftSpaces = 0;
            else
                break;
            aPtr++;
        }

        //    查找字符串的结尾
        //    Find the end of the string
        for (anEndPtr = aPtr; *anEndPtr; anEndPtr++)
            if (anEndPtr[0] == '\n') {
                if (anEndPtr[-1] == '\r') anEndPtr--;
                break;
            }

    } while (aPtr[0] == '!');

    //    形成结果
    //    form the result
    if (aPtr == anEndPtr) return Standard_False;
    thePtr = anEndPtr;
    if (*thePtr) *thePtr++ = '\0';
    theString = typename TCollection_String<CharType>::type(aPtr);
    theLeftSpaces = aLeftSpaces;
    return Standard_True;
}

//=======================================================================
// function : loadFile
// purpose  : 静态函数，从 ASCII 或 Unicode 填充消息的数据映射
// Called   : from LoadFile()
//=======================================================================

template <class _Char> static inline Standard_Boolean loadFile(_Char* theBuffer) {
    TCollection_AsciiString aKeyword;
    TCollection_ExtendedString aMessage, aString;
    LoadingState aState = MsgFile_WaitingKeyword;
    _Char* sCurrentString = theBuffer;
    Standard_Integer aLeftSpaces = 0, aFirstLeftSpaces = 0;

    //    逐个获取字符串；注释已被过滤
    //    Take strings one-by-one; comments already screened
    while (::getString(sCurrentString, aString, aLeftSpaces)) {
        Standard_Boolean isKeyword = (aString.Value(1) == '.');
        switch (aState) {
            case MsgFile_WaitingMoreMessage:
                if (isKeyword) Message_MsgFile::AddMsg(aKeyword, aMessage); // 终止前一个
                //      从这里传递到 'case MsgFile_WaitingKeyword'
                else {
                    //      将另一行添加到已在缓冲区 'aMessage' 中的消息
                    aMessage += '\n';
                    aLeftSpaces -= aFirstLeftSpaces;
                    if (aLeftSpaces > 0) aMessage += TCollection_ExtendedString(aLeftSpaces, ' ');
                    aMessage += aString;
                    break;
                }
            Standard_FALLTHROUGH case MsgFile_WaitingMessage:
                if (isKeyword == Standard_False) {
                    aMessage = aString;
                    aFirstLeftSpaces = aLeftSpaces; // 记住起始位置
                    aState = MsgFile_WaitingMoreMessage;
                    break;
                }
            //      从这里传递到 'case MsgFile_WaitingKeyword'
            Standard_FALLTHROUGH case MsgFile_WaitingKeyword:
                if (isKeyword) {
                    // 移除第一个点字符和所有后续空格 + 右修剪
                    aKeyword = TCollection_AsciiString(aString.Split(1));
                    aKeyword.LeftAdjust();
                    aKeyword.RightAdjust();
                    aState = MsgFile_WaitingMessage;
                }
                break;
            default:
                break;
        }
    }
    //    处理仍在缓冲区中的最后一个字符串
    //    Process the last string still remaining in the buffer
    if (aState == MsgFile_WaitingMoreMessage) Message_MsgFile::AddMsg(aKeyword, aMessage);
    return Standard_True;
}

//=======================================================================
// function : GetFileSize
// purpose  : 获取文件大小
//=======================================================================

static Standard_Integer GetFileSize(FILE* theFile) {
    if (!theFile) return -1;

    // 获取真实文件大小
    // get real file size
    long nRealFileSize = 0;
    if (fseek(theFile, 0, SEEK_END) != 0) return -1;
    nRealFileSize = ftell(theFile);
    if (fseek(theFile, 0, SEEK_SET) != 0) return -1;

    return (Standard_Integer)nRealFileSize;
}

//=======================================================================
// function : LoadFile
// purpose  : 从文件加载消息列表
//=======================================================================

Standard_Boolean Message_MsgFile::LoadFile(const Standard_CString theFileName) {
    if (theFileName == NULL || *theFileName == '\0') return Standard_False;

    //    打开文件
    //    Open the file
    FILE* anMsgFile = OSD_OpenFile(theFileName, "rb");
    if (!anMsgFile) return Standard_False;

    const Standard_Integer aFileSize = GetFileSize(anMsgFile);
    NCollection_Buffer aBuffer(NCollection_BaseAllocator::CommonBaseAllocator());
    if (aFileSize <= 0 || !aBuffer.Allocate(aFileSize + 2)) {
        fclose(anMsgFile);
        return Standard_False;
    }

    char* anMsgBuffer = reinterpret_cast<char*>(aBuffer.ChangeData());
    const Standard_Integer nbRead = static_cast<Standard_Integer>(fread(anMsgBuffer, 1, aFileSize, anMsgFile));

    fclose(anMsgFile);
    if (nbRead != aFileSize) return Standard_False;

    anMsgBuffer[aFileSize] = 0;
    anMsgBuffer[aFileSize + 1] = 0;

    // 读取文件中的消息并将其附加到全局数据映射
    // Read the messages in the file and append them to the global DataMap
    Standard_Boolean isLittleEndian = (anMsgBuffer[0] == '\xff' && anMsgBuffer[1] == '\xfe');
    Standard_Boolean isBigEndian = (anMsgBuffer[0] == '\xfe' && anMsgBuffer[1] == '\xff');
    if (isLittleEndian || isBigEndian) {
        Standard_ExtCharacter* aUnicodeBuffer = reinterpret_cast<Standard_ExtCharacter*>(&anMsgBuffer[2]);
        // 将 Unicode 表示转换为当前平台上采用的顺序
        // Convert Unicode representation to order adopted on current platform
#if defined(__sparc) && defined(__sun)
        if (isLittleEndian)
#else
        if (isBigEndian)
#endif
        {
            // 反转整个缓冲区中的字节
            // Reverse the bytes throughout the buffer
            const Standard_ExtCharacter* const anEnd =
                reinterpret_cast<const Standard_ExtCharacter*>(&anMsgBuffer[aFileSize]);

            for (Standard_ExtCharacter* aPtr = aUnicodeBuffer; aPtr < anEnd; aPtr++) {
                unsigned short aWord = *aPtr;
                *aPtr = (aWord & 0x00ff) << 8 | (aWord & 0xff00) >> 8;
            }
        }
        return ::loadFile(aUnicodeBuffer);
    } else
        return ::loadFile(anMsgBuffer);
}

//=======================================================================
// function : LoadFromEnv
// purpose  : 从环境变量中加载消息文件
//=======================================================================
Standard_Boolean Message_MsgFile::LoadFromEnv(const Standard_CString theEnvName, const Standard_CString theFileName,
                                              const Standard_CString theLangExt) {
    TCollection_AsciiString aLangExt(theLangExt != NULL ? theLangExt : "");
    if (aLangExt.IsEmpty()) {
        OSD_Environment aLangEnv("CSF_LANGUAGE");
        aLangExt = aLangEnv.Value();
        if (aLangExt.IsEmpty()) {
            aLangExt = "us";
        }
    }

    TCollection_AsciiString aFilePath(theFileName);
    if (theEnvName != NULL && theEnvName[0] != '\0') {
        OSD_Environment aNameEnv(theEnvName);
        TCollection_AsciiString aNameEnvStr = aNameEnv.Value();
        if (!aNameEnvStr.IsEmpty()) {
            if (aNameEnvStr.Value(aNameEnvStr.Length()) != '/') {
                aFilePath.Insert(1, '/');
            }
            aFilePath.Insert(1, aNameEnvStr);
        }
    }

    if (aLangExt.Value(1) != '.') {
        aFilePath.AssignCat('.');
    }
    aFilePath.AssignCat(aLangExt);

    return Message_MsgFile::LoadFile(aFilePath.ToCString());
}

//=======================================================================
// function : LoadFromString
// purpose  : 从字符串内容加载消息
//=======================================================================
Standard_Boolean Message_MsgFile::LoadFromString(const Standard_CString theContent, const Standard_Integer theLength) {
    Standard_Integer aStringSize = theLength >= 0 ? theLength : (Standard_Integer)strlen(theContent);
    NCollection_Buffer aBuffer(NCollection_BaseAllocator::CommonBaseAllocator());
    if (aStringSize <= 0 || !aBuffer.Allocate(aStringSize + 2)) {
        return Standard_False;
    }

    memcpy(aBuffer.ChangeData(), theContent, aStringSize);
    aBuffer.ChangeData()[aStringSize + 0] = '\0';
    aBuffer.ChangeData()[aStringSize + 1] = '\0';
    char* anMsgBuffer = reinterpret_cast<char*>(aBuffer.ChangeData());
    return ::loadFile(anMsgBuffer);
}

//=======================================================================
// function : AddMsg
// purpose  : 向全局表添加一条消息。如果相同的关键字
//           已在表中，则失败
//=======================================================================
Standard_Boolean Message_MsgFile::AddMsg(const TCollection_AsciiString& theKeyword,
                                         const TCollection_ExtendedString& theMessage) {
    Message_DataMapOfExtendedString& aDataMap = ::msgsDataMap();

    Standard_Mutex::Sentry aSentry(Message_MsgFile_Mutex());
    aDataMap.Bind(theKeyword, theMessage);
    return Standard_True;
}

//=======================================================================
// function : getMsg
// purpose  : 检索为给定关键字定义的消息
//=======================================================================
const TCollection_ExtendedString& Message_MsgFile::Msg(const Standard_CString theKeyword) {
    TCollection_AsciiString aKey(theKeyword);
    return Msg(aKey);
}

//=======================================================================
// function : HasMsg
// purpose  : 检查给定关键字是否存在消息
//=======================================================================
Standard_Boolean Message_MsgFile::HasMsg(const TCollection_AsciiString& theKeyword) {
    Standard_Mutex::Sentry aSentry(Message_MsgFile_Mutex());
    return ::msgsDataMap().IsBound(theKeyword);
}

//=======================================================================
// function : Msg
// purpose  : 检索为给定关键字定义的消息
//=======================================================================
const TCollection_ExtendedString& Message_MsgFile::Msg(const TCollection_AsciiString& theKeyword) {
    // 在映射中查找消息
    // find message in the map
    Message_DataMapOfExtendedString& aDataMap = ::msgsDataMap();
    Standard_Mutex::Sentry aSentry(Message_MsgFile_Mutex());

    // 如果未找到消息，生成错误消息并将其添加到映射中以最小化开销
    // 对后续调用具有相同的键
    // if message is not found, generate error message and add it to the map to minimize overhead
    // on consequent calls with the same key
    const TCollection_ExtendedString* aValPtr = aDataMap.Seek(theKeyword);
    if (aValPtr == NULL) {
        // 错误消息的文本本身可以在映射中定义
        // text of the error message can be itself defined in the map
        static const TCollection_AsciiString aPrefixCode("Message_Msg_BadKeyword");
        static const TCollection_ExtendedString aDefPrefix("Unknown message invoked with the keyword ");
        const TCollection_ExtendedString* aPrefValPtr = aDataMap.Seek(aPrefixCode);
        TCollection_AsciiString aErrorMessage = (aPrefValPtr != NULL ? *aPrefValPtr : aDefPrefix);
        aErrorMessage += theKeyword;
        aDataMap.Bind(theKeyword, aErrorMessage); // 不要在此处使用 AddMsg() 以避免互斥死锁
        aValPtr = aDataMap.Seek(theKeyword);
    }

    return *aValPtr;
}
