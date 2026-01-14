// Created on: 2001-04-27
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

#include <Message_Msg.hxx>
#include <Message_MsgFile.hxx>
#include <TCollection_AsciiString.hxx>
#include <stdio.h>

// 格式类型枚举：用于识别不同的格式说明符
// 例如：%d 是整数类型，%f 是浮点数类型
typedef enum
{
    Msg_IntegerType,     // 整数类型（%d, %i, %o, %u, %x, %X）
    Msg_RealType,        // 浮点数类型（%f, %e, %E, %g, %G）
    Msg_StringType,      // 字符串类型（%s）
    Msg_IndefiniteType   // 未定义类型（无法识别的格式）
} FormatType;

//=======================================================================
//function : Message_Msg()
//purpose  : 构造函数 - 创建空消息
//
// 说明：
//   - 这是最简单的构造函数
//   - 创建一个空的 Message_Msg 对象
//   - 之后可以通过 Set() 方法设置消息内容
//
// 使用示例：
//   Message_Msg msg;  // 创建空消息
//   msg.Set("Hello %s");  // 设置消息文本
//   msg.Arg("World");  // 添加参数
//=======================================================================

Message_Msg::Message_Msg()
{
}


//=======================================================================
//function : Message_Msg()
//purpose  : 构造函数 - 复制构造函数（拷贝另一个消息对象）
//
// 说明：
//   - 用于从另一个 Message_Msg 对象创建副本
//   - 复制所有内容，包括消息文本和参数列表
//   - 创建的对象是完全独立的，修改一个不会影响另一个
//
// 参数说明：
//   - theMsg：要复制的源 Message_Msg 对象
//
// 实现细节：
//   - 复制消息体（myMessageBody）
//   - 复制原始消息（myOriginal）
//   - 逐个复制格式说明符序列（mySeqOfFormats）
//
// 为什么需要：
//   - 当需要在函数间传递消息对象时
//   - 当需要保存消息的快照时
//   - 当需要基于现有消息创建新消息时
//=======================================================================

Message_Msg::Message_Msg(const Message_Msg& theMsg)
{
    myMessageBody = theMsg.myMessageBody;
    myOriginal = theMsg.myOriginal;
    // 逐个复制格式说明符，确保完整复制
    for (Standard_Integer i = 1, n = theMsg.mySeqOfFormats.Length(); i <= n; i++)
        mySeqOfFormats.Append(theMsg.mySeqOfFormats.Value(i));
}

//=======================================================================
//function : Message_Msg()
//purpose  : 构造函数 - 通过消息代码（键）初始化
//
// 说明：
//   - 从消息文件中查找并加载对应的消息文本
//   - theMessageCode 是消息的键，用于在消息表中查找
//   - 消息文件必须事先通过 Message_MsgFile::Load() 加载
//
// 参数说明：
//   - theMsgCode：消息代码（C 字符串），用作查找键
//     * 例如："MY_ERROR_CODE"
//     * 系统在消息文件中查找该代码对应的消息文本
//
// 工作流程：
//   1. 将 C 字符串转换为 ASCII 字符串
//   2. 调用 Message_MsgFile::Msg() 查找对应的消息
//   3. 调用 Set() 方法初始化消息文本和格式
//
// 前提条件：
//   - 消息文件必须已加载：Message_MsgFile::Load()
//   - 消息代码必须存在于文件中
//
// 示例：
//   Message_Msg msg("ERROR_FILE_NOT_FOUND");
//   // 系统查找 "ERROR_FILE_NOT_FOUND" 的翻译文本
//=======================================================================

Message_Msg::Message_Msg(const Standard_CString theMsgCode)
{
    TCollection_AsciiString aKey((char*)theMsgCode);
    Set(Message_MsgFile::Msg(aKey));
}

//=======================================================================
//function : Message_Msg()
//purpose  : 构造函数 - 通过扩展字符串消息代码初始化
//
// 说明：
//   - 与前一个构造函数类似，但接收扩展字符串（支持 Unicode）
//   - 用于需要多语言支持的场景
//
// 参数说明：
//   - theMsgCode：扩展字符串消息代码
//=======================================================================

Message_Msg::Message_Msg(const TCollection_ExtendedString& theMsgCode)
{
    Set(Message_MsgFile::Msg(theMsgCode));
}

//=======================================================================
//function : Set
//purpose  : 设置消息内容为 C 字符串
//
// 说明：
//   - 这是 Set() 方法的一个重载版本
//   - 接收 C 风格的字符串（const char*）
//   - 将其转换为 ASCII 字符串后调用另一个 Set() 版本
//=======================================================================

void Message_Msg::Set(const Standard_CString theMsg)
{
    TCollection_AsciiString aMsg((char*)theMsg);
    Set(aMsg);
}

//=======================================================================
//function : Set
//purpose  : 设置消息内容，解析格式说明符
//
// 说明：
//   - 这是核心的 Set 方法，真正完成解析工作
//   - 检查消息中的所有格式说明符（如 %d, %s, %f）
//   - 记录每个格式说明符的位置和类型
//   - 为之后的 Arg() 方法调用做准备
//
// 格式说明符说明：
//   - %d, %i, %o, %u, %x, %X：整数格式
//   - %f, %e, %E, %g, %G：浮点数格式
//   - %s：字符串格式
//   - %%：转义的百分号（表示一个 % 字符）
//
// 参数说明：
//   - theMsg：包含消息文本和格式说明符的字符串
//
// 实现细节：
//   - 逐字符扫描消息
//   - 找到 '%' 后，检查其后的字符
//   - 跳过格式标志（-, +, 空格, #）和字段宽度
//   - 识别格式类型并记录在 mySeqOfFormats 中
//   - 每个格式说明符记录三项信息：类型、位置、长度
//
// 数据结构说明：
//   - mySeqOfFormats：整数序列，按 3 个一组存储格式信息
//   - [0, 1, 2]：第一个格式（类型、位置、长度）
//   - [3, 4, 5]：第二个格式（类型、位置、长度）
//   - 以此类推...
//=======================================================================

void Message_Msg::Set(const TCollection_ExtendedString& theMsg)
{
    myMessageBody = theMsg;

    const Standard_ExtString anExtString = myMessageBody.ToExtString();
    Standard_Integer   anMsgLength = myMessageBody.Length();
    for (Standard_Integer i = 0; i < anMsgLength; i++)
    {
        //  搜索启动格式说明的 '%' 字符
        //  Search for '%' character starting a format specification
        if (ToCharacter(anExtString[i]) == '%')
        {
            Standard_Integer   aStart = i++;
            Standard_Character aChar = ToCharacter(anExtString[i]);
            //        检查格式 '%%'（转义的百分号）
            //        Check for format '%%'
            if (aChar == '%')
            {
                myMessageBody.Remove(i + 1);
                if (i >= --anMsgLength) break;
                aChar = ToCharacter(anExtString[i]);
            }
            //        跳过标志、字段宽度和精度
            //        Skip flags, field width and precision
            while (i < anMsgLength)
            {
                if (aChar == '-' || aChar == '+' || aChar == ' ' ||
                    aChar == '#' || (aChar >= '0' && aChar <= '9') || aChar == '.')
                    i++;
                else break;
                aChar = ToCharacter(anExtString[i]);
            }
            if (i >= anMsgLength) break;

            FormatType aFormatType;
            if (aChar == 'h' || aChar == 'l') aChar = ToCharacter(anExtString[++i]);
            // 检测格式说明的类型
            switch (aChar)
            {
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                aFormatType = Msg_IntegerType;
                break;
            case 'f':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                aFormatType = Msg_RealType;
                break;
            case 's':
                aFormatType = Msg_StringType;
                break;
            default:
                aFormatType = Msg_IndefiniteType;
                continue;
            }
            // 记录格式信息：类型、位置、长度
            mySeqOfFormats.Append(Standard_Integer(aFormatType));  // 类型
            mySeqOfFormats.Append(aStart);                         // 起始位置
            mySeqOfFormats.Append(i + 1 - aStart);                 // 长度
        }
    }
    // 保存原始消息（未修改的版本）
    myOriginal = myMessageBody;
}

//=======================================================================
//function : Arg (Standard_CString)
//purpose  : 用 C 字符串参数替换下一个字符串格式说明
//
// 说明：
//   - 查找消息中下一个 %s（字符串）格式说明符
//   - 将其替换为提供的字符串参数
//   - 如果没有更多的 %s，则返回而不做任何操作
//   - 这是链式调用的基础，支持流式语法
//
// 参数说明：
//   - theString：要替换的字符串值
//
// 返回值：
//   - *this：返回对象本身，支持链式调用
//
// 使用示例：
//   Message_Msg msg("User %s logged in");
//   msg.Arg("John").Arg(" at 10:30");
//   // 结果：用 "John" 替换第一个 %s
//
// 工作流程：
//   1. 调用 getFormat() 查找下一个字符串格式说明符
//   2. 获取该格式的完整说明（如 "%10s"）
//   3. 使用 sprintf 按照格式将字符串格式化
//   4. 调用 replaceText() 用格式化后的字符串替换原格式说明
//=======================================================================

Message_Msg& Message_Msg::Arg(const Standard_CString theString)
{
    // 获取位置和格式
    // get location and format
    TCollection_AsciiString aFormat;
    Standard_Integer aFirst = getFormat(Msg_StringType, aFormat);
    if (!aFirst)
        return *this;

    // 根据格式打印字符串
    // print string according to format
    char* sStringBuffer = new char[Max((Standard_Integer)strlen(theString) + 1, 1024)];
    Sprintf(sStringBuffer, aFormat.ToCString(), theString);
    TCollection_ExtendedString aStr(sStringBuffer, Standard_True);
    delete[] sStringBuffer;
    sStringBuffer = 0;

    // 用实际字符串替换格式占位符
    // replace the format placeholder by the actual string
    replaceText(aFirst, aFormat.Length(), aStr);

    return *this;
}

//=======================================================================
//function : Arg (TCollection_ExtendedString)
//purpose  : 用扩展字符串参数替换下一个字符串格式说明
//
// 说明：
//   - 与 Arg(CString) 类似，但接收扩展字符串（支持 Unicode）
//   - 扩展字符串直接插入，不需要按格式转换
//   - 这种方法用于多语言应用
//
// remark   : This type of string is inserted without conversion (i.e. like %s)
//=======================================================================

Message_Msg& Message_Msg::Arg(const TCollection_ExtendedString& theString)
{
    // 获取位置和格式
    // get location and format
    TCollection_AsciiString aFormat;
    Standard_Integer aFirst = getFormat(Msg_StringType, aFormat);
    if (!aFirst)
        return *this;

    // 用实际字符串替换格式占位符
    // replace the format placeholder by the actual string
    replaceText(aFirst, aFormat.Length(), theString);

    return *this;
}

//=======================================================================
//function : Arg (Standard_Integer)
//purpose  : 用整数参数替换下一个整数格式说明
//
// 说明：
//   - 查找消息中下一个整数格式说明符（%d, %i 等）
//   - 将整数按照格式说明符的规则进行格式化
//   - 然后替换原来的格式说明符
//
// 使用示例：
//   Message_Msg msg("Error code: %d");
//   msg.Arg(404);
//   // 结果："Error code: 404"
//
//   Message_Msg msg2("Hex: %x");
//   msg2.Arg(255);
//   // 结果："Hex: ff"（十六进制）
//=======================================================================

Message_Msg& Message_Msg::Arg(const Standard_Integer theValue)
{
    // 获取位置和格式
    // get location and format
    TCollection_AsciiString aFormat;
    Standard_Integer aFirst = getFormat(Msg_IntegerType, aFormat);
    if (!aFirst)
        return *this;

    // 根据格式打印字符串
    // print string according to format
    char sStringBuffer[64];
    Sprintf(sStringBuffer, aFormat.ToCString(), theValue);
    TCollection_ExtendedString aStr(sStringBuffer);

    // 用实际字符串替换格式占位符
    // replace the format placeholder by the actual string
    replaceText(aFirst, aFormat.Length(), aStr);

    return *this;
}

//=======================================================================
//function : Arg (Standard_Real)
//purpose  : 用浮点数参数替换下一个浮点数格式说明
//
// 说明：
//   - 查找消息中下一个浮点数格式说明符（%f, %e, %g 等）
//   - 根据格式说明符决定小数位数、科学计数法等
//
// 使用示例：
//   Message_Msg msg("Temperature: %.2f°C");
//   msg.Arg(36.789);
//   // 结果："Temperature: 36.79°C"
//
//   Message_Msg msg2("Value: %e");
//   msg2.Arg(0.00001234);
//   // 结果："Value: 1.234000e-05"（科学计数法）
//=======================================================================

Message_Msg& Message_Msg::Arg(const Standard_Real theValue)
{
    // 获取位置和格式
    // get location and format
    TCollection_AsciiString aFormat;
    Standard_Integer aFirst = getFormat(Msg_RealType, aFormat);
    if (!aFirst)
        return *this;

    // 根据格式打印字符串
    // print string according to format
    char sStringBuffer[64];
    Sprintf(sStringBuffer, aFormat.ToCString(), theValue);
    TCollection_ExtendedString aStr(sStringBuffer);

    // 用实际字符串替换格式占位符
    // replace the format placeholder by the actual string
    replaceText(aFirst, aFormat.Length(), aStr);

    return *this;
}

//=======================================================================
//function : Get
//purpose  : 获取最终消息，用于在 Message_Messenger 中分发时使用
//
// 说明：
//   - 这个方法完成消息的最终处理
//   - 移除所有未被替换的格式说明符
//   - 将它们替换为 "UNKNOWN"
//   - 返回可以输出的最终消息字符串
//
// 返回值：
//   - 返回对包含最终消息的扩展字符串的常引用
//
// 为什么需要这个步骤：
//   - 如果调用了 Arg(value) 但没有提供足够的参数
//   - 或者提供的参数类型不匹配
//   - 那么有些格式说明符可能仍然存在
//   - 这个方法确保它们被替换为 "UNKNOWN"，防止输出乱码
//
// 使用示例：
//   Message_Msg msg("Hello %s, your ID is %d");
//   msg.Arg("Alice");  // 只提供了一个参数
//   std::cout << msg.Get();
//   // 输出："Hello Alice, your ID is UNKNOWN"
//=======================================================================

const TCollection_ExtendedString& Message_Msg::Get()
{
    // 移除所有未初始化的格式说明
    // remove all non-initialised format specifications
    Standard_Integer i, anIncrement = 0;
    static const TCollection_ExtendedString anUnknown("UNKNOWN");
    for (i = 1; i < mySeqOfFormats.Length(); i += 3)
    {
        // 分割字符串：提取格式说明符后面的部分
        TCollection_ExtendedString aRightPart =
            myMessageBody.Split(mySeqOfFormats(i + 1) + anIncrement);
        // 移除原来的格式说明符
        aRightPart.Remove(1, mySeqOfFormats(i + 2));
        // 用 "UNKNOWN" 替换格式说明符
        myMessageBody += anUnknown;
        myMessageBody += aRightPart;
        // 调整后续格式说明符的位置
        anIncrement += (anUnknown.Length() - mySeqOfFormats(i + 2));
    }
    return myMessageBody;
}

//=======================================================================
//function : getFormat
//purpose  : 在消息中查找指定类型的下一个格式说明符
//
// 参数说明：
//   - theType：要查找的格式类型（整数、浮点数或字符串）
//   - theFormat：输出参数，返回找到的格式说明符字符串
//     * 例如："10d"（表示 %10d）、"5.2f"（表示 %.2f）
//
// 返回值：
//   - 返回格式说明符的起始位置（在字符串中）
//   - 如果未找到，返回 0
//
// 说明：
//   - 这是一个内部方法，用于支持 Arg() 方法
//   - 遍历 mySeqOfFormats 查找符合类型的第一个格式
//   - 提取格式字符串（例如从 "%10d" 中提取 "10d"）
//   - 删除已处理的格式记录，防止重复处理
//   - 每个类型的格式说明符只能使用一次
//
// 关键设计：
//   - 格式说明符的处理是"先进先出"（FIFO）的
//   - 第一个 Arg() 调用处理第一个格式说明符
//   - 如果 Arg() 调用的顺序与格式说明符的顺序不匹配，会导致错误
//=======================================================================

Standard_Integer Message_Msg::getFormat(const Standard_Integer theType,
    TCollection_AsciiString& theFormat)
{
    for (Standard_Integer i = 1; i <= mySeqOfFormats.Length(); i += 3)
        if (mySeqOfFormats(i) == theType)
        {
            // 提取格式
            // Extract format
            Standard_Integer aFirst = mySeqOfFormats(i + 1);
            Standard_Integer aLen = mySeqOfFormats(i + 2);
            theFormat = TCollection_AsciiString(aLen, ' ');
            for (Standard_Integer j = 1; j <= aLen; j++)
                if (IsAnAscii(myMessageBody.Value(aFirst + j)))
                    theFormat.SetValue(j, (Standard_Character)myMessageBody.Value(aFirst + j));
            // 删除此占位符的信息
            // delete information on this placeholder
            mySeqOfFormats.Remove(i, i + 2);
            // 返回起始位置
            // return start position
            return aFirst + 1;
        }
    return 0;
}

//=======================================================================
//function : replaceText
//purpose  : 用新字符串替换消息中的一段文本
//
// 参数说明：
//   - theFirst：要替换的文本的起始位置
//   - theNb：要替换的文本长度
//   - theStr：用于替换的新字符串
//
// 说明：
//   - 这是一个内部工具方法
//   - 用于将格式说明符替换为实际的参数值
//   - 替换后需要更新其他格式说明符的位置信息
//   - 因为整个字符串可能变长或变短
//
// 例子：
//   原消息："Hello %s, your age is %d"
//   替换 %s 为 "Alice"（长度 5）：
//     "Hello Alice, your age is %d"
//     后续的 %d 位置需要增加 5 个字符的偏移
//
// 实现细节：
//   - 先删除原来的格式说明符
//   - 再在其位置插入新字符串
//   - 计算长度变化量（anIncrement）
//   - 更新所有后续格式说明符的位置
//=======================================================================

void Message_Msg::replaceText(const Standard_Integer theFirst,
    const Standard_Integer theNb,
    const TCollection_ExtendedString& theStr)
{
    // 从消息中移除原来的格式说明符
    myMessageBody.Remove(theFirst, theNb);
    // 在原位置插入新字符串
    myMessageBody.Insert(theFirst, theStr);

    // 更新其余格式占位符的信息
    // update information on remaining format placeholders
    Standard_Integer anIncrement = theStr.Length() - theNb;
    if (!anIncrement) return;  // 如果长度没有变化，无需更新
    for (Standard_Integer i = 1; i <= mySeqOfFormats.Length(); i += 3)
        // 如果后续格式说明符的位置在替换点之后，更新其位置
        if (mySeqOfFormats(i + 1) > theFirst)
            mySeqOfFormats(i + 1) += anIncrement;
}
