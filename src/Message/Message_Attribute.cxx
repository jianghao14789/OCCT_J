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

#include <Message_Attribute.hxx>

#include <Standard_Assert.hxx>
#include <Standard_Dump.hxx>

// 为 Message_Attribute 类注册 RTTI
IMPLEMENT_STANDARD_RTTIEXT(Message_Attribute, Standard_Transient)

//=======================================================================
//function : Constructor
//purpose  : 使用给定的名称初始化属性对象
//
// 说明：
//   - Message_Attribute 是扩展警报的附加属性容器
//   - 每个属性都有一个名称，用于识别它
//   - 属性可以存储额外的信息，例如：
//     * 错误发生的位置（文件、行号）
//     * 受影响的对象名称
//     * 额外的上下文信息
//
// 参数说明：
//   - theName：属性的名称或标签
//     * 通常是有意义的文本，便于识别属性的用途
//     * 例如："OutputFile"、"CurrentProcessingItem"
//
// 属性与警报的关系：
//   - 警报是事件本身（例如"发生错误"）
//   - 属性是关于这个事件的额外信息
//   - 一个警报可以有多个属性
//   - 属性被组织在 AlertExtended 对象中
//=======================================================================
Message_Attribute::Message_Attribute(const TCollection_AsciiString& theName)
    : myName(theName)
{
}

//=======================================================================
//function : GetMessageKey
//purpose  : 获取用于消息处理的键，返回属性的名称
//
// 说明：
//   - 这个方法返回属性的名称
//   - 名称可用作在消息表中查找本地化文本的键
//   - 例如，属性名为 "InvalidParameter"，系统可以查找对应的错误描述
//
// 返回值：
//   - 返回指向属性名称的 C 字符串指针
//   - 如果名称为空，返回空字符串指针 ""
//
// 实现细节：
//   - 检查 myName 是否为空
//   - 如果为空，返回空字符串；否则返回名称
//   - ToCString() 将 OCCT 字符串转换为 C 风格字符串
//
// 示例：
//   Message_Attribute attr("FileNotFound");
//   const char* key = attr.GetMessageKey();  // 返回 "FileNotFound"
//=======================================================================
Standard_CString Message_Attribute::GetMessageKey() const
{
    // 三元运算符：如果 myName 不为空，返回其 C 字符串表示；否则返回空字符串
    return !myName.IsEmpty() ? myName.ToCString() : "";
}

//=======================================================================
//function : DumpJson
//purpose  : 将对象内容导出为 JSON 格式（用于调试和日志记录）
//
// 参数说明：
//   - theOStream：输出流，JSON 将被写入此流
//   - theDepth：递归深度（在此函数中未使用）
//
// 说明：
//   - 这个方法生成属性对象的 JSON 表示
//   - JSON 格式使数据易于解析和显示
//   - 通常在诊断问题或记录状态时使用
//
// 输出示例：
//   {
//     "Message_Attribute": {
//       "myName": "InvalidParameter"
//     }
//   }
//
// 关键宏说明：
//   - OCCT_DUMP_TRANSIENT_CLASS_BEGIN()：开始对象的 JSON 输出
//   - OCCT_DUMP_FIELD_VALUE_STRING()：输出字符串字段
//=======================================================================
void Message_Attribute::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
    // 开始输出瞬态类（非永久存储的对象）的 JSON
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
    
    // 输出属性的名称字段
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myName)
}
