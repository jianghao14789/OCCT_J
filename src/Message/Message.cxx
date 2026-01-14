// Created on: 1999-11-23
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Report.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
#include <string.h>

namespace
{
    // 全局静态数组：存储 13 种度量类型的字符串名称
    // 这个数组用于在度量类型和字符串表示之间进行转换
    static Standard_CString Message_Table_PrintMetricTypeEnum[13] =
    {
      "NONE",                   // 未定义的度量
      "ThreadCPUUserTime",      // 线程用户 CPU 时间
      "ThreadCPUSystemTime",    // 线程系统 CPU 时间
      "ProcessCPUUserTime",     // 进程用户 CPU 时间
      "ProcessCPUSystemTime",   // 进程系统 CPU 时间
      "WallClock",              // 实时时钟
      "MemPrivate",             // 私有内存使用
      "MemVirtual",             // 虚拟内存使用
      "MemWorkingSet",          // 工作集内存
      "MemWorkingSetPeak",      // 工作集峰值
      "MemSwapUsage",           // 交换内存使用
      "MemSwapUsagePeak",       // 交换内存峰值
      "MemHeapUsage"            // 堆内存使用
    };
}

//=======================================================================
//function : DefaultMessenger
//purpose  : 返回全局默认的 Messenger 对象（单例模式）
//
// 说明：
//   - Messenger 是消息传递系统的核心，用于管理消息打印机
//   - 使用 static 关键字保证整个程序中只有一个 Messenger 实例
//   - 第一次调用时创建对象，之后每次调用返回相同的对象
//   - Handle<> 是智能指针，自动管理对象的生命周期
//
// 返回值：
//   - 返回的是对 Message_Messenger 对象的常引用
//   - Handle<> 类似于 std::shared_ptr，但用于 OCCT 库
//=======================================================================
const Handle(Message_Messenger)& Message::DefaultMessenger()
{
    // 声明静态变量，只初始化一次
    static Handle(Message_Messenger) aMessenger = new Message_Messenger;
    return aMessenger;
}

//=======================================================================
//function : FillTime
//purpose  : 将时、分、秒的三个数值格式化为可读的时间字符串
//
// 参数说明：
//   - hour：小时数（>=0）
//   - minute：分钟数（0-59）
//   - second：秒数（浮点数，可以包含小数部分）
//
// 返回值：
//   - 返回格式化后的时间字符串，例如：
//     * 如果输入 1, 30, 45.5，返回 "01h:30m:45.50s"
//     * 如果输入 0, 5, 30.0，返回 "05m:30.00s"
//     * 如果输入 0, 0, 5.5，返回 "5.50s"
//
// 实现细节：
//   - 使用 sprintf 进行格式化（将数据写入字符数组）
//   - %02d 表示以 2 位数字显示，不足用 0 填充
//   - %.2f 表示显示 2 位小数的浮点数
//   - 根据小时和分钟的值选择不同的格式
//=======================================================================

TCollection_AsciiString Message::FillTime(const Standard_Integer hour,
    const Standard_Integer minute,
    const Standard_Real second)
{
    char t[30];
    if (hour > 0)
        // 如果有小时数，显示 "小时h:分钟m:秒s" 的格式
        Sprintf(t, "%02dh:%02dm:%.2fs", hour, minute, second);
    else if (minute > 0)
        // 如果只有分钟和秒，显示 "分钟m:秒s" 的格式
        Sprintf(t, "%02dm:%.2fs", minute, second);
    else
        // 如果只有秒，显示 "秒s" 的格式
        Sprintf(t, "%.2fs", second);
    // 将 C 风格的字符数组转换为 OCCT 的 TCollection_AsciiString
    return TCollection_AsciiString(t);
}

//=======================================================================
//function : DefaultReport
//purpose  : 返回全局默认的 Report 对象（单例模式，可选创建）
//
// 参数说明：
//   - theToCreate：是否在不存在时创建新的 Report
//     * Standard_True：如果为空则创建新实例
//     * Standard_False：如果为空则返回空指针
//
// 返回值：
//   - 返回对 Message_Report 对象的常引用
//   - 如果参数为 false 且对象未创建，则返回空引用
//
// 用途：
//   - Report 用于收集和管理所有的警报和错误信息
//   - 通常与 Messenger 配合使用
//=======================================================================
const Handle(Message_Report)& Message::DefaultReport(const Standard_Boolean theToCreate)
{
    // 声明静态变量，保持全局唯一
    static Handle(Message_Report) MyReport;
    // 懒加载：只在需要时创建对象
    if (MyReport.IsNull() && theToCreate)
    {
        MyReport = new Message_Report();
    }
    return MyReport;
}

//=======================================================================
//function : MetricToString
//purpose  : 将度量类型枚举值转换为对应的字符串名称
//
// 参数说明：
//   - theType：度量类型的枚举值（0-12）
//
// 返回值：
//   - 返回度量类型的字符串表示，例如 "MemPrivate"、"WallClock" 等
//   - 返回的是指向静态数组的指针，不应修改内容
//
// 实现细节：
//   - 直接通过数组索引快速查找字符串
//   - 时间复杂度为 O(1)（常数时间）
//=======================================================================
Standard_CString Message::MetricToString(const Message_MetricType theType)
{
    // 数组索引查询，直接返回对应的字符串
    return Message_Table_PrintMetricTypeEnum[theType];
}

//=======================================================================
//function : MetricFromString
//purpose  : 将字符串转换为对应的度量类型枚举值
//
// 参数说明：
//   - theString：输入的字符串（例如 "MemPrivate"）
//   - theGravity：输出参数，存储转换后的度量类型
//
// 返回值：
//   - Standard_True：如果字符串与某个度量类型匹配
//   - Standard_False：如果未找到匹配的度量类型
//
// 实现细节：
//   - 将输入字符串转换为 ASCII 字符串进行比较
//   - 遍历整个枚举表进行逐一比较
//   - 如果找到匹配，立即返回 true，否则返回 false
//   - 时间复杂度为 O(n)，n 为度量类型的总数（13）
//=======================================================================
Standard_Boolean Message::MetricFromString(const Standard_CString theString,
    Message_MetricType& theGravity)
{
    // 将 C 风格字符串转换为 OCCT 的 ASCII 字符串对象
    TCollection_AsciiString aName(theString);
    // 遍历所有度量类型，查找匹配的字符串
    for (Standard_Integer aMetricIter = 0; aMetricIter <= Message_MetricType_MemHeapUsage; ++aMetricIter)
    {
        // 获取当前枚举值对应的字符串
        Standard_CString aMetricName = Message_Table_PrintMetricTypeEnum[aMetricIter];
        if (aName == aMetricName)
        {
            // 找到匹配：将枚举值赋给输出参数，返回 true
            theGravity = Message_MetricType(aMetricIter);
            return Standard_True;
        }
    }
    // 遍历完成后未找到匹配，返回 false
    return Standard_False;
}

// =======================================================================
// function : ToOSDMetric
// purpose  : 将 Message 库的度量类型转换为 OSD 库的内存计数器类型
//
// 说明：
//   - Message 库和 OSD 库都有各自的度量类型定义
//   - 这个函数用于在两个库的类型系统之间进行转换
//   - 通过 switch 语句进行一一对应的映射
//
// 参数说明：
//   - theMetric：Message 库的度量类型
//   - theMemInfo：输出参数，存储对应的 OSD 度量类型
//
// 返回值：
//   - Standard_True：如果度量类型是内存相关的（可以转换）
//   - Standard_False：如果度量类型无法转换（例如时间相关的度量）
// =======================================================================
Standard_Boolean Message::ToOSDMetric(const Message_MetricType theMetric, OSD_MemInfo::Counter& theMemInfo)
{
    switch (theMetric)
    {
    // 私有内存映射
    case Message_MetricType_MemPrivate:
        theMemInfo = OSD_MemInfo::MemPrivate;
        break;
    // 虚拟内存映射
    case Message_MetricType_MemVirtual:
        theMemInfo = OSD_MemInfo::MemVirtual;
        break;
    // 工作集内存映射
    case Message_MetricType_MemWorkingSet:
        theMemInfo = OSD_MemInfo::MemWorkingSet;
        break;
    // 工作集峰值映射
    case Message_MetricType_MemWorkingSetPeak:
        theMemInfo = OSD_MemInfo::MemWorkingSetPeak;
        break;
    // 交换内存使用映射
    case Message_MetricType_MemSwapUsage:
        theMemInfo = OSD_MemInfo::MemSwapUsage;
        break;
    // 交换内存峰值映射
    case Message_MetricType_MemSwapUsagePeak:
        theMemInfo = OSD_MemInfo::MemSwapUsagePeak;
        break;
    // 堆内存使用映射
    case Message_MetricType_MemHeapUsage:
        theMemInfo = OSD_MemInfo::MemHeapUsage;
        break;
    // 无法映射的类型（如时间相关的度量）
    default:
        return Standard_False;
    }
    return Standard_True;
}

// =======================================================================
// function : ToMessageMetric
// purpose  : 将 OSD 库的内存计数器类型转换为 Message 库的度量类型
//
// 说明：
//   - 这是 ToOSDMetric 的反向转换
//   - 用于从 OSD 系统获取的度量值转换为 Message 库的格式
//
// 参数说明：
//   - theMemInfo：OSD 库的内存计数器类型
//   - theMetric：输出参数，存储对应的 Message 度量类型
//
// 返回值：
//   - Standard_True：转换成功
//   - Standard_False：无法转换
// =======================================================================
Standard_Boolean Message::ToMessageMetric(const OSD_MemInfo::Counter theMemInfo, Message_MetricType& theMetric)
{
    switch (theMemInfo)
    {
    // 私有内存反向映射
    case OSD_MemInfo::MemPrivate:
        theMetric = Message_MetricType_MemPrivate;
        break;
    // 虚拟内存反向映射
    case OSD_MemInfo::MemVirtual:
        theMetric = Message_MetricType_MemVirtual;
        break;
    // 工作集反向映射
    case OSD_MemInfo::MemWorkingSet:
        theMetric = Message_MetricType_MemWorkingSet;
        break;
    // 工作集峰值反向映射
    case OSD_MemInfo::MemWorkingSetPeak:
        theMetric = Message_MetricType_MemWorkingSetPeak;
        break;
    // 交换内存使用反向映射
    case OSD_MemInfo::MemSwapUsage:
        theMetric = Message_MetricType_MemSwapUsage;
        break;
    // 交换内存峰值反向映射
    case OSD_MemInfo::MemSwapUsagePeak:
        theMetric = Message_MetricType_MemSwapUsagePeak;
        break;
    // 堆内存使用反向映射
    case OSD_MemInfo::MemHeapUsage:
        theMetric = Message_MetricType_MemHeapUsage;
        break;
    // 无法映射的类型
    default:
        return Standard_False;
    }
    return Standard_True;
}
