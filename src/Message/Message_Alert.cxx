// Created on: 2017-06-26
// Created by: Andrey Betenev
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Message_Alert.hxx>
#include <Standard_Dump.hxx>

// IMPLEMENT_STANDARD_RTTIEXT 宏：为 Message_Alert 类注册运行时类型信息
// - RTTI 表示"运行时类型信息"(Run-Time Type Information)
// - 这样做可以在程序运行时动态判断对象的类型
// - 第一个参数是当前类名，第二个参数是父类名
IMPLEMENT_STANDARD_RTTIEXT(Message_Alert, Standard_Transient)

//=======================================================================
//function : GetMessageKey
//purpose  : 获取用于生成消息的键（通常用于多语言消息查找）
//
// 说明：
//   - 这个方法返回的键用于在消息表中查找对应的消息文本
//   - 默认实现返回类的动态类型名称（如 "Message_Alert"）
//   - 子类可以重写此方法返回更具体的消息键
//
// 返回值：
//   - 返回指向 C 字符串的指针
//   - 字符串的内容通常是类的类型名称
//
// 用途示例：
//   - 如果键是 "ERROR_INVALID_INPUT"，系统会在消息文件中查找该键对应的本地化消息
//=======================================================================

Standard_CString Message_Alert::GetMessageKey() const
{
    // 调用动态类型获取方法，获取当前对象的类名
    return DynamicType()->Name();
}

//=======================================================================
//function : SupportsMerge
//purpose  : 检查是否支持将多个相同类型的警报合并为一个
//
// 说明：
//   - 当多个相同的警报被添加时，系统可以选择合并它们而不是单独存储
//   - 例如：多个 "内存不足" 警报可以合并为一个计数为 3 的警报
//   - 这样可以减少存储空间，提高显示效率
//
// 返回值：
//   - Standard_True：支持合并
//   - Standard_False：不支持合并
//
// 默认行为：
//   - 基类返回 Standard_True（支持合并）
//   - 子类可以重写此方法改变合并行为
//=======================================================================

Standard_Boolean Message_Alert::SupportsMerge() const
{
    // by default, support merge
    // 默认情况下，所有警报都支持合并
    return Standard_True;
}

//=======================================================================
//function : Merge
//purpose  : 尝试将此警报与另一个警报合并
//
// 参数说明：
//   - theTarget：要合并的目标警报对象
//
// 返回值：
//   - Standard_True：合并成功
//   - Standard_False：合并失败
//
// 说明：
//   - 这个方法在 SupportsMerge() 返回 true 时才会被调用
//   - 合并操作由子类实现，基类只提供默认行为
//   - 合并通常涉及更新计数器或累加值
//
// 默认行为：
//   - 基类返回 Standard_True（合并成功）
//   - 子类可以重写此方法实现自己的合并逻辑
//
// 示例：
//   - 如果一个警报表示 "发现 3 个错误"
//   - 合并另一个 "发现 2 个错误" 时
//   - 可以更新为 "发现 5 个错误"
//=======================================================================

Standard_Boolean Message_Alert::Merge(const Handle(Message_Alert)& /*theTarget*/)
{
    // by default, merge trivially（默认情况下，合并总是成功的）
    // 参数名称前的 // 表示这个参数在此函数中未使用
    // 这避免了编译器的未使用参数警告
    return Standard_True;
}

//=======================================================================
//function : DumpJson
//purpose  : 将对象的内容以 JSON 格式输出（用于调试和日志记录）
//
// 参数说明：
//   - theOStream：输出流对象，JSON 将被写入此流
//   - theDepth：递归深度（默认为 -1，表示无限深度）
//     * 0：只输出顶层信息
//     * 1：输出顶层及其子对象的第一层
//     * -1：输出完整的递归结构
//
// 说明：
//   - 这个方法用于调试和日志记录
//   - JSON 格式使结构化数据易于解析和查看
//   - 通常在诊断问题时调用
//
// 例子输出：
//   {
//     "Message_Alert": {
//       "DynamicType": "Message_Alert"
//     }
//   }
//
// OCCT_DUMP_TRANSIENT_CLASS_BEGIN 宏：
//   - 开始 JSON 对象的声明
//   - 标记这是一个瞬态类（非永久存储的类）
//=======================================================================
void Message_Alert::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
    // 开始 JSON 对象的输出
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
    // 注意：这个基类的 DumpJson 实现很简单
    // 子类通常会添加更多的字段输出
}
