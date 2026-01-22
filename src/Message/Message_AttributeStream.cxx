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

#include <Message_AttributeStream.hxx>
#include <Standard_Dump.hxx>

// 为 Message_AttributeStream 类注册 RTTI
// 第二个参数 Message_Attribute 表示基类
IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeStream, Message_Attribute)

//=======================================================================
// function : Constructor
// purpose  : 使用流和名称初始化流属性对象
//
// 说明：
//   - Message_AttributeStream 是 Message_Attribute 的子类
//   - 用于存储多行或结构化的文本内容
//   - 适合存储日志信息、错误堆栈跟踪、调试输出等
//
// 参数说明：
//   - theStream：包含消息内容的字符串流对象
//     * Standard_SStream 是 C++ 的 std::stringstream 包装
//     * 可以包含多行内容、格式化数据等
//   - theName：属性的名称或标签
//     * 用于识别这个流属性的用途
//     * 如果为空，则使用默认名称
//
// 流与字符串的区别：
//   - Message_Attribute：存储单行或简单的字符串
//   - Message_AttributeStream：存储多行或结构化的内容
//   - 流可以包含格式化的对象状态、调试输出等
//
// 实现细节：
//   - 调用基类构造函数初始化 myName（属性名称）
//   - 调用 SetStream() 方法存储流的内容
//=======================================================================
Message_AttributeStream::Message_AttributeStream(const Standard_SStream& theStream,
                                                 const TCollection_AsciiString& theName)
    : Message_Attribute(theName) {
    SetStream(theStream);
}

//=======================================================================
// function : SetStream
// purpose  : 设置或更新流的值
//
// 参数说明：
//   - theStream：新的流对象
//
// 说明：
//   - 将传入的流内容复制到内部的 myStream 对象中
//   - 用于更新或修改已存在的属性的流内容
//
// 实现细节：
//   - myStream.str("") 清空内部流的内容
//   - theStream.str().c_str() 从输入流中提取 C 字符串
//   - << 运算符将字符串写入内部流
//   - 这样做是为了复制流的内容而不仅仅是引用
//
// 例子：
//   std::stringstream ss;
//   ss << "Error log line 1\n" << "Error log line 2\n";
//   attr.SetStream(ss);
//   // 现在 attr 包含了 ss 的内容的副本
//=======================================================================
void Message_AttributeStream::SetStream(const Standard_SStream& theStream) {
    // 清空内部流：将其内容设置为空字符串
    myStream.str("");
    // 从输入流提取内容并写入内部流
    // theStream.str() 返回流的字符串内容
    // .c_str() 将其转换为 C 风格字符串指针
    myStream << theStream.str().c_str();
}

//=======================================================================
// function : DumpJson
// purpose  : 将流属性对象导出为 JSON 格式
//
// 参数说明：
//   - theOStream：输出流
//   - theDepth：JSON 输出的递归深度（用于限制嵌套对象的深度）
//     * 0：只输出本对象，不递归输出子对象
//     * -1：无限深度递归（默认）
//     * n > 0：最多递归 n 层
//
// 说明：
//   - 这个方法输出对象本身的 JSON 表示
//   - 包括属性的名称（来自基类）
//   - 包括存储的流内容
//
// 输出示例：
//   {
//     "Message_AttributeStream": {
//       "Message_Attribute": {
//         "myName": "ErrorLog"
//       },
//       "myStream": "Error log line 1\nError log line 2\n"
//     }
//   }
//
// 关键宏说明：
//   - OCCT_DUMP_TRANSIENT_CLASS_BEGIN()：开始对象的 JSON 输出
//   - OCCT_DUMP_BASE_CLASS()：输出基类的字段
//   - OCCT_DUMP_STREAM_VALUE_DUMPED()：输出流的内容
//=======================================================================
void Message_AttributeStream::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const {
    // 开始输出对象的 JSON
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

    // 输出基类（Message_Attribute）的字段
    // theDepth 用于控制基类字段的递归深度
    OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Message_Attribute)

    // 输出此类特有的字段：流内容
    OCCT_DUMP_STREAM_VALUE_DUMPED(theOStream, myStream)
}
