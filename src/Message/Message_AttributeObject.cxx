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

#include <Message_AttributeObject.hxx>
#include <Standard_Dump.hxx>

// 为 Message_AttributeObject 类注册 RTTI
// 第二个参数 Message_Attribute 表示基类
IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeObject, Message_Attribute)

//=======================================================================
// function : Constructor
// purpose  : 使用对象和名称初始化对象属性
//
// 说明：
//   - Message_AttributeObject 是用于在警报中附加任意对象的属性
//   - 它将一个 OCCT 对象（例如几何体、产品等）与警报关联
//   - 这样可以将警报与具体的对象相关联，便于定位问题
//
// 参数说明：
//   - theObject：要附加到警报的对象
//     * 可以是任何 OCCT 的 Transient 对象
//     * 例如：Shape（几何体）、Document（文档）等
//   - theName：属性的名称或标签
//     * 用于描述对象的用途或含义
//     * 例如："ProcessedShape", "CurrentDocument"
//
// 与其他属性的区别：
//   - Message_Attribute：存储简单的字符串信息
//   - Message_AttributeStream：存储多行文本内容
//   - Message_AttributeMeter：存储性能度量数据
//   - Message_AttributeObject：存储对象引用（本类）
//
// 使用示例：
//   // 处理一个形状时发生错误
//   Handle(TopoDS_Shape) aShape = ...;
//   Handle(Message_AttributeObject) attr =
//       new Message_AttributeObject(aShape, "FailedShape");
//   // 现在警报可以包含指向出错的具体形状的引用
//
// 实现细节：
//   - 调用基类构造函数初始化名称
//   - 存储对象到 myObject 成员变量
//   - 使用 Handle<> 智能指针管理对象生命周期
//     * Handle<> 会自动计数和释放资源
//     * 防止内存泄漏
//=======================================================================
Message_AttributeObject::Message_AttributeObject(const Handle(Standard_Transient) & theObject,
                                                 const TCollection_AsciiString& theName)
    : Message_Attribute(theName) // 调用基类构造函数，初始化名称
{
    // 存储传入的对象
    myObject = theObject;
}

//=======================================================================
// function : DumpJson
// purpose  : 将对象内容导出为 JSON 格式（用于调试）
//
// 参数说明：
//   - theOStream：输出流，JSON 将被写入此流
//   - theDepth：递归深度（用于控制嵌套对象的深度）
//
// 说明：
//   - 生成此属性对象的 JSON 表示
//   - 包括基类的属性和存储的对象指针
//   - 用于调试和监控警报中的对象信息
//
// 关键宏说明：
//   - OCCT_DUMP_TRANSIENT_CLASS_BEGIN()：开始输出对象的 JSON
//   - OCCT_DUMP_BASE_CLASS()：输出基类的字段
//   - OCCT_DUMP_FIELD_VALUE_POINTER()：输出对象指针
//
// 输出示例：
//   {
//     "Message_AttributeObject": {
//       "Message_Attribute": {
//         "myName": "ErrorShape"
//       },
//       "myObject": "0x00007FFF12345678"
//     }
//   }
//
// 为什么只输出指针而不输出对象内容：
//   - 对象可能非常大（例如复杂的 3D 形状）
//   - 对象的详细信息可以通过指针在调试器中查看
//   - 避免 JSON 输出变得过于巨大
//   - 这是一个合理的权衡
//
// JSON 中的指针值：
//   - 十六进制格式（例如 0x12345678）
//   - 代表内存中对象的地址
//   - 在同一次运行中，相同对象总是有相同的地址
//=======================================================================
void Message_AttributeObject::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const {
    // 开始输出此对象的 JSON 表示
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

    // 输出基类（Message_Attribute）的内容
    // theDepth 参数用于递归调用时控制深度
    OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Message_Attribute)

    // 输出存储的对象的指针值
    // 这允许使用指针在调试器中查找对象
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myObject.get())
}
