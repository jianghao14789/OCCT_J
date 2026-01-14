// 中文注释版
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

#include <Message_AlertExtended.hxx>

#include <Message_Attribute.hxx>
#include <Message_CompositeAlerts.hxx>
#include <Message_Report.hxx>

#include <Precision.hxx>
#include <Standard_Assert.hxx>
#include <Standard_Dump.hxx>

// 为 Message_AlertExtended 类注册 RTTI
IMPLEMENT_STANDARD_RTTIEXT(Message_AlertExtended, Message_Alert)

//=======================================================================
//function : AddAlert
//purpose  : 为报告添加扩展警报并返回其句柄
//
// 参数说明：
//   - theReport：目标报告对象
//   - theAttribute：要附加到警报的属性（可选）
//   - theGravity：警报的严重级别
//
// 返回值：
//   - 返回新创建的警报对象的句柄
//   - 调用者可以保存此句柄以便后续使用
//
// 说明：
//   - 这是一个工厂方法（Factory Pattern）
//   - 创建新的 AlertExtended 对象
//   - 设置其属性
//   - 自动添加到报告中
//   - 返回句柄以便调用者可以保存或进一步操作警报
//
// 工作流程：
//   1. 创建新的 Message_AlertExtended 对象
//   2. 如果提供了属性，设置到警报中
//   3. 将警报添加到报告（通过 Report::AddAlert）
//   4. 返回警报的句柄
//
// 使用示例：
//   Handle(Message_Attribute) attr = new Message_Attribute("ProcessingError");
//   Handle(Message_Report) report = new Message_Report();
//   
//   Handle(Message_Alert) alert = Message_AlertExtended::AddAlert(
//       report, attr, Message_Warning);
//   
//   // 现在可以继续操作这个警报
//   if (!alert.IsNull())
//   {
//       // alert 已被添加到 report
//   }
//
// 设计优势：
//   - 简化了创建和添加警报的过程
//   - 减少了样板代码
//   - 返回句柄允许链式操作
//   - 属性参数可选（如果不需要属性，传递 Handle() 或省略）
//=======================================================================
Handle(Message_Alert) Message_AlertExtended::AddAlert(const Handle(Message_Report)& theReport,
    const Handle(Message_Attribute)& theAttribute,
    const Message_Gravity theGravity)
{
    // 创建新的扩展警报对象
    // new Message_AlertExtended() 创建一个堆上的新对象
    // Handle<> 会自动管理其生命周期
    Handle(Message_AlertExtended) anAlert = new Message_AlertExtended();
    
    // 为警报设置属性（如果提供了属性）
    // 属性可以包含警报的额外信息
    anAlert->SetAttribute(theAttribute);
    
    // 将警报添加到报告
    // 报告会管理警报，允许后续查询、合并、转储等操作
    theReport->AddAlert(theGravity, anAlert);
    
    // 返回警报句柄
    // 调用者可以保存此句柄以备后用
    return anAlert;
}

//=======================================================================
//function : GetMessageKey
//purpose  : 获取消息键，如果有属性则返回属性的键
//
// 说明：
//   - 这个方法覆盖基类的 GetMessageKey()
//   - 实现了属性优先的设计：
//     * 如果设置了属性，返回属性的键
//     * 如果没有属性，返回基类的默认键（类型名）
//
// 返回值：
//   - 返回 C 字符串指针，指向消息键
//   - 通常是一个有意义的文本标识符
//
// 为什么优先使用属性的键：
//   - 属性通常包含更具体的信息
//   - 例如："FileNotFound" 比 "Message_AlertExtended" 更有意义
//   - 允许多级化的错误描述
//
// 使用场景：
//   - 当消息系统查找本地化文本时，使用此键
//   - 例如，键 "FileNotFound" 可能对应英文 "File not found"
//   - 或对应中文 "文件未找到"
//   - 这样实现了多语言支持
//=======================================================================
Standard_CString Message_AlertExtended::GetMessageKey() const
{
    // 检查是否设置了属性
    if (myAttribute.IsNull())
    {
        // 如果没有属性，使用基类的实现
        // 基类返回动态类型名称（例如 "Message_AlertExtended"）
        return Message_Alert::GetMessageKey();
    }
    // 如果有属性，返回属性的消息键
    // 属性通常包含更具体的错误描述
    return myAttribute->GetMessageKey();
}

//=======================================================================
//function : CompositeAlerts
//purpose  : 获取或创建复合警报容器
//
// 参数说明：
//   - theToCreate：是否在容器不存在时创建新的
//     * Standard_True：如果不存在则创建
//     * Standard_False：如果不存在则返回空
//
// 返回值：
//   - 返回复合警报容器的句柄
//   - 可能为空（如果 theToCreate 为 false 且不存在）
//
// 说明：
//   - 这个方法实现了惰性初始化（Lazy Initialization）
//   - 复合警报容器用于存储嵌套的子警报
//   - 当不需要子警报时，不创建容器，节省内存
//   - 当需要添加子警报时，自动创建容器
//
// 容器的作用：
//   - 支持警报的树形结构
//   - 例如：主警报下可以有多个子警报
//   - 这样可以组织复杂的错误报告
//
// 使用示例：
//   Handle(Message_AlertExtended) parentAlert = ...;
//   
//   // 获取子警报容器（如果不存在则创建）
//   Handle(Message_CompositeAlerts) subAlerts = 
//       parentAlert->CompositeAlerts(Standard_True);
//   
//   // 现在可以向 subAlerts 中添加子警报
//   // ...
//
// 与 Report 的关系：
//   - Report 也有类似的 CompositeAlerts 方法
//   - Report 的容器存储顶级警报
//   - AlertExtended 的容器存储其子警报
//   - 形成树形结构
//=======================================================================
Handle(Message_CompositeAlerts) Message_AlertExtended::CompositeAlerts(const Standard_Boolean theToCreate)
{
    // 检查容器是否存在
    if (myCompositAlerts.IsNull() && theToCreate)
    {
        // 如果不存在且要求创建，创建新的容器
        myCompositAlerts = new Message_CompositeAlerts();
    }
    // 返回容器（可能为空）
    return myCompositAlerts;
}

//=======================================================================
//function : SupportsMerge
//purpose  : 检查是否支持将多个警报合并为一个
//
// 说明：
//   - 这个方法覆盖基类的实现
//   - 实现了有条件的合并支持：
//     * 如果没有子警报，支持合并
//     * 如果有子警报，不支持合并
//   - 原因：分层的警报结构无法简单地合并
//
// 返回值：
//   - Standard_True：可以合并
//   - Standard_False：不可以合并
//
// 设计理由：
//   - 简单的警报（没有子警报）可以合并以减少数量
//   - 复杂的分层警报无法合并，因为会丢失结构
//   - 例如：两个"处理失败"警报可以合并为一个计数为 2
//   - 但两个包含不同子警报的警报无法简单合并
//
// 使用场景：
//   - Report 在添加警报时，会检查是否可以合并
//   - 如果可以合并且警报相同，会增加计数而不是创建新警报
//   - 这样可以减少重复的警报并保持报告的紧凑
//=======================================================================
Standard_Boolean Message_AlertExtended::SupportsMerge() const
{
    // 检查是否有子警报容器
    if (myCompositAlerts.IsNull())
    {
        // 如果没有子警报，支持合并
        return Standard_True;
    }

    // 如果有容器，检查其中是否存在警报
    // hierarchical alerts can not be merged（分层警报无法合并）
    for (int aGravIter = Message_Trace; aGravIter <= Message_Fail; ++aGravIter)
    {
        // 检查此严重级别是否有警报
        if (!myCompositAlerts->Alerts((Message_Gravity)aGravIter).IsEmpty())
        {
            // 如果任何级别有警报，不支持合并
            return Standard_False;
        }
    }

    // 如果容器存在但为空，支持合并
    return Standard_True;
}

//=======================================================================
//function : Merge
//purpose  : 尝试将此警报与另一个警报合并
//
// 参数说明：
//   - theTarget：目标警报（此处未使用，仅为了兼容基类接口）
//
// 返回值：
//   - Standard_False：总是返回 false（不支持合并）
//
// 说明：
//   - 这个方法覆盖基类的实现
//   - ExtendedAlert 的默认实现总是返回 false
//   - 表示扩展警报不支持合并
//   - 如果子类需要支持合并，应重写此方法
//
// 为什么不支持合并：
//   - ExtendedAlert 通常包含属性或子警报
//   - 这些内容无法简单地合并
//   - 合并可能导致信息丢失
//   - 保守的做法是不支持合并
//
// 参数中的注释说明：
//   - `/*theTarget*/` 中的 `/* */` 表示此参数未使用
//   - 这是 C++ 中避免未使用参数编译警告的常见做法
//=======================================================================
Standard_Boolean Message_AlertExtended::Merge(const Handle(Message_Alert)& /*theTarget*/)
{
    // 默认情况下，ExtendedAlert 不支持合并
    // by default, merge trivially（这个注释有点误导，实际返回 false）
    return Standard_False;
}

//=======================================================================
//function : DumpJson
//purpose  : 将对象内容导出为 JSON 格式（用于调试）
//
// 参数说明：
//   - theOStream：输出流
//   - theDepth：递归深度（用于限制嵌套深度）
//
// 说明：
//   - 生成此警报的 JSON 表示
//   - 包括子警报容器和属性
//   - 用于调试和日志记录
//
// 输出示例：
//   {
//     "Message_AlertExtended": {
//       "myCompositAlerts": {
//         "Message_CompositeAlerts": {
//           ...
//         }
//       },
//       "myAttribute": {
//         "Message_Attribute": {
//           "myName": "ErrorDescription"
//         }
//       }
//     }
//   }
//
// 输出顺序：
//   1. 先输出子警报（如果存在）
//   2. 再输出属性（如果存在）
//   3. 只输出非空的成员
//
// 设计考虑：
//   - 如果没有子警报也没有属性，输出会很简洁
//   - 这反映了对象的实际内容
//   - 避免输出冗余的空结构
//=======================================================================
void Message_AlertExtended::DumpJson(Standard_OStream& theOStream,
    Standard_Integer theDepth) const
{
    // 开始输出对象的 JSON
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

    // 如果存在子警报容器，输出它
    if (!myCompositAlerts.IsNull())
    {
        OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCompositAlerts.get())
    }
    
    // 如果存在属性，输出它
    if (!myAttribute.IsNull())
    {
        OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myAttribute.get())
    }
}
