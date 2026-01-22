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

#include <Message_AttributeMeter.hxx>

#include <Message_AlertExtended.hxx>
#include <Message.hxx>
#include <Message_Report.hxx>
#include <OSD_Chronometer.hxx>
#include <OSD_Timer.hxx>

#include <Precision.hxx>
#include <Standard_Dump.hxx>

// 为 Message_AttributeMeter 类注册 RTTI
IMPLEMENT_STANDARD_RTTIEXT(Message_AttributeMeter, Message_Attribute)

//=======================================================================
// function : Constructor
// purpose  : 构造函数，初始化度量属性
//
// 说明：
//   - Message_AttributeMeter 是用于性能监测的属性类
//   - 它跟踪和记录各种性能指标（度量）
//   - 包括：时间、内存使用、CPU 时间等
//   - 每个度量都有"起始值"和"停止值"，用于计算差值
//
// 参数说明：
//   - theName：度量的名称或标签
//     * 例如："FileProcessing", "DataConversion"
//     * 用于标识这个度量代表什么操作
//
// 成员变量说明：
//   - myMetrics：映射容器，存储度量类型到(开始值, 结束值)的映射
//     * 键：Message_MetricType（度量类型枚举）
//     * 值：std::pair<Standard_Real, Standard_Real>（开始值和结束值）
//
// 使用示例：
//   Message_AttributeMeter meter("ImageLoading");
//   // meter 现在可以用来记录图像加载的性能数据
//   // 如 CPU 时间、内存使用等
//=======================================================================
Message_AttributeMeter::Message_AttributeMeter(const TCollection_AsciiString& theName) : Message_Attribute(theName) {}

//=======================================================================
// function : HasMetric
// purpose  : 检查是否存在给定的度量类型
//
// 参数说明：
//   - theMetric：要检查的度量类型
//     * 例如：Message_MetricType_MemPrivate（私有内存）
//     * Message_MetricType_WallClock（实时时钟）
//
// 返回值：
//   - Standard_True：该度量已被记录
//   - Standard_False：该度量不存在
//
// 说明：
//   - 这是一个简单的存在性检查
//   - 用于确定是否可以查询某个度量的值
//   - 不检查度量的有效性（是否有完整的开始/结束值）
//
// 使用示例：
//   if (meter.HasMetric(Message_MetricType_WallClock))
//   {
//       double elapsed = meter.StopValue(...) - meter.StartValue(...);
//   }
//=======================================================================
Standard_Boolean Message_AttributeMeter::HasMetric(const Message_MetricType& theMetric) const {
    // 使用容器的 Contains 方法检查键是否存在
    return myMetrics.Contains(theMetric);
}

//=======================================================================
// function : IsMetricValid
// purpose  : 检查度量是否有效（既有起始值也有停止值）
//
// 参数说明：
//   - theMetric：要检查的度量类型
//
// 返回值：
//   - Standard_True：度量既有起始值又有停止值，且都不是"未定义"
//   - Standard_False：度量缺失或不完整
//
// 说明：
//   - 有效的度量必须满足两个条件：
//     * 1. 起始值不等于未定义值
//     * 2. 停止值不等于未定义值
//   - 使用 Precision::Confusion() 作为比较的精度阈值
//   - 这是因为浮点数比较需要考虑精度误差
//
// 实现细节：
//   - UndefinedMetricValue() 返回一个特殊的"未定义"值
//   - StartValue() 和 StopValue() 返回相应的值
//   - Abs() 计算绝对值
//   - Precision::Confusion() 是一个极小的阈值（通常为 1e-7）
//
// 使用示例：
//   if (meter.IsMetricValid(Message_MetricType_WallClock))
//   {
//       // 可以安全地使用起始值和停止值
//       double elapsed = meter.StopValue(...) - meter.StartValue(...);
//   }
//=======================================================================
Standard_Boolean Message_AttributeMeter::IsMetricValid(const Message_MetricType& theMetric) const {
    // 检查起始值是否有意义（不是未定义值）
    return Abs(StartValue(theMetric) - UndefinedMetricValue()) > Precision::Confusion() &&
           // 同时检查停止值是否有意义
           Abs(StopValue(theMetric) - UndefinedMetricValue()) > Precision::Confusion();
}

//=======================================================================
// function : StartValue
// purpose  : 获取度量的起始值
//
// 参数说明：
//   - theMetric：要查询的度量类型
//
// 返回值：
//   - 返回该度量的起始值（浮点数）
//   - 如果度量不存在，返回 UndefinedMetricValue()
//
// 说明：
//   - 起始值是度量开始时的数值
//   - 例如：处理开始时的内存使用量
//   - 通过 (StopValue - StartValue) 可以计算增量
//
// 实现细节：
//   - 首先检查度量是否存在（HasMetric）
//   - 如果不存在，返回未定义值
//   - 如果存在，从映射中取出起始值（pair 的 first）
//
// 使用示例：
//   double memStart = meter.StartValue(Message_MetricType_MemPrivate);
//   double memStop = meter.StopValue(Message_MetricType_MemPrivate);
//   double memUsed = memStop - memStart;  // 内存增长量
//=======================================================================
Standard_Real Message_AttributeMeter::StartValue(const Message_MetricType& theMetric) const {
    // 如果度量不存在，返回未定义值
    if (!HasMetric(theMetric)) {
        return UndefinedMetricValue();
    }

    // 从映射中取出值对，返回 first（起始值）
    // myMetrics.Seek() 返回指向值对的指针
    return myMetrics.Seek(theMetric)->first;
}

//=======================================================================
// function : SetStartValue
// purpose  : 设置度量的起始值
//
// 参数说明：
//   - theMetric：度量类型
//   - theValue：要设置的起始值
//
// 说明：
//   - 用于记录度量的初始值
//   - 通常在操作开始时调用
//   - 如果度量已存在，更新其起始值
//   - 如果度量不存在，创建新的度量条目
//
// 实现细节：
//   - ChangeSeek() 返回指向值对的可修改指针
//   - 如果找到了现有度量，修改其 first（起始值）
//   - 如果没有找到，使用 Add() 创建新条目
//   - std::make_pair() 创建一个值对
//   - 新条目的停止值初始化为 UndefinedMetricValue()
//
// 使用示例：
//   // 在处理开始时
//   meter.SetStartValue(Message_MetricType_WallClock, OSD_Timer::GetWallClockTime());
//   meter.SetStartValue(Message_MetricType_MemPrivate, getMemoryUsage());
//
//   // ... 执行一些操作 ...
//
//   // 在处理结束时
//   meter.SetStopValue(Message_MetricType_WallClock, OSD_Timer::GetWallClockTime());
//   meter.SetStopValue(Message_MetricType_MemPrivate, getMemoryUsage());
//=======================================================================
void Message_AttributeMeter::SetStartValue(const Message_MetricType& theMetric, const Standard_Real theValue) {
    // 尝试找到现有的度量条目
    // ChangeSeek() 返回指向值对的可修改指针，如果不存在则返回 nullptr
    if (StartToStopValue* aValPtr = myMetrics.ChangeSeek(theMetric)) {
        // 如果找到了，修改其起始值
        aValPtr->first = theValue;
    } else {
        // 如果不存在，创建新条目
        // 起始值设置为 theValue，停止值初始化为未定义
        myMetrics.Add(theMetric, std::make_pair(theValue, UndefinedMetricValue()));
    }
}

//=======================================================================
// function : StopValue
// purpose  : 获取度量的停止值
//
// 参数说明：
//   - theMetric：要查询的度量类型
//
// 返回值：
//   - 返回该度量的停止值（浮点数）
//   - 如果度量不存在，返回 UndefinedMetricValue()
//
// 说明：
//   - 停止值是度量结束时的数值
//   - 例如：处理完成后的内存使用量
//   - 与 StartValue 配合使用，计算变化量
//
// 实现细节：
//   - 逻辑与 StartValue 类似
//   - 但返回的是值对的 second（停止值）
//=======================================================================
Standard_Real Message_AttributeMeter::StopValue(const Message_MetricType& theMetric) const {
    // 如果度量不存在，返回未定义值
    if (!HasMetric(theMetric)) {
        return UndefinedMetricValue();
    }
    // 返回 second（停止值）
    return myMetrics.Seek(theMetric)->second;
}

//=======================================================================
// function : SetStopValue
// purpose  : 设置度量的停止值
//
// 参数说明：
//   - theMetric：度量类型
//   - theValue：要设置的停止值
//
// 说明：
//   - 用于记录度量的最终值
//   - 通常在操作结束时调用
//   - 如果该度量不存在，此方法不做任何操作
//
// 实现细节：
//   - 与 SetStartValue 不同，此方法只修改不创建
//   - 如果度量不存在，方法直接返回
//   - 这防止了创建只有停止值而没有起始值的不完整记录
//
// 使用示例：
//   // 在处理结束时
//   meter.SetStopValue(Message_MetricType_WallClock, OSD_Timer::GetWallClockTime());
//=======================================================================
void Message_AttributeMeter::SetStopValue(const Message_MetricType& theMetric, const Standard_Real theValue) {
    // 尝试找到现有的度量条目
    if (StartToStopValue* aValPtr = myMetrics.ChangeSeek(theMetric)) {
        // 修改其停止值
        aValPtr->second = theValue;
    }
    // 如果不存在，什么都不做（不创建新条目）
}

//=======================================================================
// function : SetAlertMetrics
// purpose  : 为警报自动设置度量值（起始或停止）
//
// 参数说明：
//   - theAlert：要设置度量的警报对象
//   - theStartValue：是否设置起始值
//     * Standard_True：设置起始值（在操作开始时调用）
//     * Standard_False：设置停止值（在操作结束时调用）
//
// 说明：
//   - 这是一个静态方法，用于自动捕获度量数据
//   - 从默认报告中获取激活的度量类型列表
//   - 根据激活的度量自动收集当前的性能数据
//   - 包括：时间、CPU 时间、内存使用等
//
// 工作流程：
//   1. 验证警报和其属性的有效性
//   2. 根据激活的度量类型，收集相应的数据
//   3. 时间度量：使用 OSD_Timer 和 OSD_Chronometer
//   4. 内存度量：使用 OSD_MemInfo
//   5. 将收集到的数据设置到属性中
//
// 支持的度量类型：
//   - 时间类：WallClock（实时），ProcessCPU（进程CPU），ThreadCPU（线程CPU）
//   - 内存类：MemPrivate（私有内存），MemVirtual（虚拟内存）等
//
// 使用示例：
//   Handle(Message_AlertExtended) alert = new Message_AlertExtended();
//   // 设置警报属性...
//
//   // 在操作开始时
//   Message_AttributeMeter::SetAlertMetrics(alert, Standard_True);
//
//   // ... 执行操作 ...
//
//   // 在操作结束时
//   Message_AttributeMeter::SetAlertMetrics(alert, Standard_False);
//
// 实现细节分析：
//=======================================================================
void Message_AttributeMeter::SetAlertMetrics(const Handle(Message_AlertExtended) & theAlert,
                                             const Standard_Boolean theStartValue) {
    // 验证警报不为空
    if (theAlert.IsNull()) {
        return;
    }

    // 尝试从警报中提取度量属性
    // DownCast 类似于 C++ 的 dynamic_cast，用于安全的类型转换
    Handle(Message_AttributeMeter) aMeterAttribute = Handle(Message_AttributeMeter)::DownCast(theAlert->Attribute());
    if (aMeterAttribute.IsNull()) {
        return;
    }

    // 获取默认报告和其激活的度量列表
    // 激活的度量列表指定了要收集哪些性能数据
    Handle(Message_Report) aReport = Message::DefaultReport(Standard_True);
    const NCollection_IndexedMap<Message_MetricType>& anActiveMetrics = aReport->ActiveMetrics();

    // ========== 时间度量 ==========
    // time metrics

    // 1. 实时时钟（WallClock）
    // 实时时钟测量的是墙上时钟的流逝时间，不受 CPU 调度影响
    if (anActiveMetrics.Contains(Message_MetricType_WallClock)) {
        OSD_Timer aTimer;
        aTimer.Start(); // 启动计时器（虽然这行不做任何事，因为我们直接获取时间）
        // 获取当前的实时时间（秒数）
        Standard_Real aTime = OSD_Timer::GetWallClockTime();
        if (theStartValue)
            // 设置起始时间
            aMeterAttribute->SetStartValue(Message_MetricType_WallClock, aTime);
        else
            // 设置结束时间
            aMeterAttribute->SetStopValue(Message_MetricType_WallClock, aTime);
    }

    // 2. CPU 时间
    // CPU 时间只计算进程或线程实际运行在 CPU 上的时间
    // 不包括等待、阻塞等时间
    if (anActiveMetrics.Contains(Message_MetricType_ProcessCPUUserTime) ||
        anActiveMetrics.Contains(Message_MetricType_ProcessCPUSystemTime) ||
        anActiveMetrics.Contains(Message_MetricType_ThreadCPUUserTime) ||
        anActiveMetrics.Contains(Message_MetricType_ThreadCPUSystemTime)) {
        // 2a. 进程级 CPU 时间
        if (anActiveMetrics.Contains(Message_MetricType_ProcessCPUUserTime) ||
            anActiveMetrics.Contains(Message_MetricType_ProcessCPUSystemTime)) {
            // 获取进程的用户态和系统态 CPU 时间
            Standard_Real aProcessUserTime, aProcessSystemTime;
            OSD_Chronometer::GetProcessCPU(aProcessUserTime, aProcessSystemTime);

            // 处理用户态 CPU 时间
            if (anActiveMetrics.Contains(Message_MetricType_ProcessCPUUserTime)) {
                if (theStartValue)
                    aMeterAttribute->SetStartValue(Message_MetricType_ProcessCPUUserTime, aProcessUserTime);
                else
                    aMeterAttribute->SetStopValue(Message_MetricType_ProcessCPUUserTime, aProcessUserTime);
            }

            // 处理系统态 CPU 时间
            if (anActiveMetrics.Contains(Message_MetricType_ProcessCPUSystemTime)) {
                if (theStartValue)
                    aMeterAttribute->SetStartValue(Message_MetricType_ProcessCPUSystemTime, aProcessSystemTime);
                else
                    aMeterAttribute->SetStopValue(Message_MetricType_ProcessCPUSystemTime, aProcessSystemTime);
            }
        }

        // 2b. 线程级 CPU 时间
        if (anActiveMetrics.Contains(Message_MetricType_ThreadCPUUserTime) ||
            anActiveMetrics.Contains(Message_MetricType_ThreadCPUSystemTime)) {
            // 获取当前线程的用户态和系统态 CPU 时间
            Standard_Real aThreadUserTime, aThreadSystemTime;
            OSD_Chronometer::GetThreadCPU(aThreadUserTime, aThreadSystemTime);

            // 处理线程用户态 CPU 时间
            if (anActiveMetrics.Contains(Message_MetricType_ThreadCPUUserTime)) {
                if (theStartValue)
                    aMeterAttribute->SetStartValue(Message_MetricType_ThreadCPUUserTime, aThreadUserTime);
                else
                    aMeterAttribute->SetStopValue(Message_MetricType_ThreadCPUUserTime, aThreadUserTime);
            }

            // 处理线程系统态 CPU 时间
            if (anActiveMetrics.Contains(Message_MetricType_ThreadCPUSystemTime)) {
                if (theStartValue)
                    aMeterAttribute->SetStartValue(Message_MetricType_ThreadCPUSystemTime, aThreadSystemTime);
                else
                    aMeterAttribute->SetStopValue(Message_MetricType_ThreadCPUSystemTime, aThreadSystemTime);
            }
        }
    }

    // ========== 内存度量 ==========
    // memory metrics

    // 创建内存信息查询对象
    // OSD_MemInfo 用于查询操作系统的内存使用情况
    // 参数 Standard_False 表示不自动激活任何计数器
    OSD_MemInfo aMemInfo(Standard_False);
    aMemInfo.SetActive(Standard_False); // 明确禁用所有活动计数器

    // 创建一个临时映射来存储要查询的内存计数器
    NCollection_IndexedMap<OSD_MemInfo::Counter> aCounters;

    // 遍历所有激活的度量，找出内存相关的度量
    for (NCollection_IndexedMap<Message_MetricType>::Iterator anIterator(anActiveMetrics); anIterator.More();
         anIterator.Next()) {
        // 尝试将 Message 库的度量类型转换为 OSD 库的内存计数器类型
        OSD_MemInfo::Counter anInfoCounter;
        if (!Message::ToOSDMetric(anIterator.Value(), anInfoCounter)) {
            // 如果无法转换（例如时间度量），继续下一个
            continue;
        }

        // 添加到计数器列表，并激活此计数器
        aCounters.Add(anInfoCounter);
        aMemInfo.SetActive(anInfoCounter, Standard_True);
    }

    // 如果没有内存度量需要收集，直接返回
    if (aCounters.IsEmpty()) {
        return;
    }

    // 执行内存查询（从操作系统获取当前内存使用信息）
    aMemInfo.Update();

    // 将 OSD 度量值转换为 Message 度量值并保存
    Message_MetricType aMetricType;
    for (NCollection_IndexedMap<OSD_MemInfo::Counter>::Iterator anIterator(aCounters); anIterator.More();
         anIterator.Next()) {
        // 将 OSD 计数器类型转换回 Message 度量类型
        if (!Message::ToMessageMetric(anIterator.Value(), aMetricType)) {
            // 转换失败，继续下一个
            continue;
        }

        // 从 OSD_MemInfo 中获取值（以 MiB 为单位的精确值）
        // ValuePreciseMiB() 返回内存大小（单位：MiB = 1024*1024 字节）
        Standard_Real memValue = (Standard_Real)aMemInfo.ValuePreciseMiB(anIterator.Value());

        // 设置起始值或停止值
        if (theStartValue)
            aMeterAttribute->SetStartValue(aMetricType, memValue);
        else
            aMeterAttribute->SetStopValue(aMetricType, memValue);
    }
}

//=======================================================================
// function : DumpJson
// purpose  : 将对象内容导出为 JSON 格式（用于调试）
//
// 参数说明：
//   - theOStream：输出流，JSON 将被写入此流
//   - theDepth：递归深度（用于控制嵌套深度）
//
// 说明：
//   - 生成此对象的 JSON 表示，便于调试和日志记录
//   - 包括基类的信息和所有记录的度量
//   - 对于每个度量，输出其起始值和停止值
//
// 输出示例：
//   {
//     "Message_AttributeMeter": {
//       "Message_Attribute": {
//         "myName": "ImageProcessing"
//       },
//       "WallClock": [10.5, 15.3],
//       "MemPrivate": [128.5, 256.2]
//     }
//   }
//=======================================================================
void Message_AttributeMeter::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const {
    // 开始输出对象的 JSON
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

    // 输出基类（Message_Attribute）的信息
    OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Message_Attribute)

    // 遍历所有记录的度量，输出每个度量的信息
    for (NCollection_IndexedDataMap<Message_MetricType, StartToStopValue>::Iterator anIterator(myMetrics);
         anIterator.More(); anIterator.Next()) {
        // 输出向量形式的数据：度量名称 + [起始值, 停止值]
        OCCT_DUMP_VECTOR_CLASS(theOStream,
                               Message::MetricToString(anIterator.Key()), // 度量名称
                               2,                                         // 向量大小（2个值）
                               anIterator.Value().first,                  // 起始值
                               anIterator.Value().second)                 // 停止值
    }
}
