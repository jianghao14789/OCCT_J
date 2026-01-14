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

#ifndef _Message_AttributeMeter_HeaderFile
#define _Message_AttributeMeter_HeaderFile

#include <Message_Attribute.hxx>
#include <Message_MetricType.hxx>

#include <NCollection_IndexedDataMap.hxx>

class Message_Alert;
class Message_AlertExtended;

//! Alert object storing alert metrics values.
//! Start and stop values for each metric.
//! 
//! 存储警报指标值的警报对象。
//! 每个指标的开始和停止值。
class Message_AttributeMeter : public Message_Attribute
{
public:

    //! Returns default value of the metric when it is not defined
    //! @return undefined value
    //! 返回未定义指标时的默认值
    //! @return 未定义的值
    static Standard_Real UndefinedMetricValue() { return -1.0; }

public:

    //! Constructor with string argument
    //! 带字符串参数的构造函数
    Standard_EXPORT Message_AttributeMeter(const TCollection_AsciiString& theName = TCollection_AsciiString());

    //! Checks whether the attribute has values for the metric
    //! @param theMetric [in] metric type
    //! @return true if the metric values exist in the attribute
    //! 
    //! 检查属性是否具有指标的值
    //! @param theMetric [in] 指标类型
    //! @return 如果指标值存在于属性中，则返回 true
    Standard_EXPORT Standard_Boolean HasMetric(const Message_MetricType& theMetric) const;

    //! Returns true when both values of the metric are set.
    //! @param theMetric [in] metric type
    //! @return true if metric values are valid
    //! 
    //! 当指标的两个值都设置时返回 true。
    //! @param theMetric [in] 指标类型
    //! @return 如果指标值有效，则返回 true
    Standard_EXPORT Standard_Boolean IsMetricValid(const Message_MetricType& theMetric) const;

    //! Returns start value for the metric
    //! @param theMetric [in] metric type
    //! @return real value
    //! 返回指标的起始值
    //! @param theMetric [in] 指标类型
    //! @return 实数值
    Standard_EXPORT Standard_Real StartValue(const Message_MetricType& theMetric) const;

    //! Sets start values for the metric
    //! @param theMetric [in] metric type
    //! 为指标设置起始值
    //! @param theMetric [in] 指标类型
    Standard_EXPORT void SetStartValue(const Message_MetricType& theMetric, const Standard_Real theValue);

    //! Returns stop value for the metric
    //! @param theMetric [in] metric type
    //! @return real value
    //! 返回指标的停止值
    //! @param theMetric [in] 指标类型
    //! @return 实数值
    Standard_EXPORT Standard_Real StopValue(const Message_MetricType& theMetric) const;

    //! Sets stop values for the metric
    //! @param theMetric [in] metric type
    //! 为指标设置停止值
    //! @param theMetric [in] 指标类型
    Standard_EXPORT void SetStopValue(const Message_MetricType& theMetric, const Standard_Real theValue);

public:

    //! Sets start values of default report metrics into the alert
    //! @param theAlert an alert  
    //! 将默认报告指标的起始值设置到警报中
    //! @param theAlert 一个警报
    static void StartAlert(const Handle(Message_AlertExtended)& theAlert) { SetAlertMetrics(theAlert, Standard_True); }

    //! Sets stop values of default report metrics into the alert
    //! @param theAlert an alert  
    //! 将默认报告指标的停止值设置到警报中
    //! @param theAlert 一个警报
    static void StopAlert(const Handle(Message_AlertExtended)& theAlert) { SetAlertMetrics(theAlert, Standard_False); }

    //! Sets current values of default report metrics into the alert.
    //! Processed only alert with Message_AttributeMeter attribute
    //! @param theAlert an alert  
    //! @param theStartValue flag, if true, the start value is collected otherwise stop
    //! 
    //! 将默认报告指标的当前值设置到警报中。
    //! 仅处理具有 Message_AttributeMeter 属性的警报
    //! @param theAlert 一个警报
    //! @param theStartValue 标记，如果为 true，收集起始值，否则收集停止值
    static Standard_EXPORT void SetAlertMetrics(const Handle(Message_AlertExtended)& theAlert,
        const Standard_Boolean theStartValue);

    //! Dumps the content of me into the stream
    //! 将内容转储到流中
    virtual Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
        Standard_Integer theDepth = -1) const Standard_OVERRIDE;

    DEFINE_STANDARD_RTTIEXT(Message_AttributeMeter, Message_Attribute)

private:

    typedef std::pair<Standard_Real, Standard_Real> StartToStopValue;
    NCollection_IndexedDataMap<Message_MetricType, StartToStopValue> myMetrics; //!< 计算的指标
                                                                                  //!< computed metrics
};

#endif // _Message_AttributeMeter_HeaderFile
