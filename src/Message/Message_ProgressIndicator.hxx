// Created on: 2002-02-20
// Created by: Andrey BETENEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _Message_ProgressIndicator_HeaderFile
#define _Message_ProgressIndicator_HeaderFile

#include <Standard_TypeDef.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Handle.hxx>

DEFINE_STANDARD_HANDLE(Message_ProgressIndicator, Standard_Transient)

class Message_ProgressRange;
class Message_ProgressScope;

//! Defines abstract interface from program to the user.
//! This includes progress indication and user break mechanisms.
//!
//! The progress indicator controls the progress scale with range from 0 to 1.
//!
//! Method Start() should be called once, at the top level of the call stack,
//! to reset progress indicator and get access to the root range:
//!
//! @code{.cpp}
//! Handle(Message_ProgressIndicator) aProgress = ...;
//! anAlgorithm.Perform (aProgress->Start());
//! @endcode
//!
//! To advance the progress indicator in the algorithm,
//! use the class Message_ProgressScope that provides iterator-like
//! interface for incrementing progress; see documentation of that
//! class for details.
//! The object of class Message_ProgressRange will automatically advance
//! the indicator if it is not passed to any Message_ProgressScope.
//!
//! The progress indicator supports concurrent processing and
//! can be used in multithreaded applications.
//!
//! The derived class should be created to connect this interface to
//! actual implementation of progress indicator, to take care of visualization
//! of the progress (e.g. show total position at the graphical bar,
//! print scopes in text mode, or else), and for implementation
//! of user break mechanism (if necessary).
//!
//! See details in documentation of methods Show() and UserBreak().
//!
//! 定义从程序到用户的抽象接口。这包括进度指示和用户中断机制。
//!
//! 进度指示器控制范围为 0 到 1 的进度刻度。
//!
//! 应在调用堆栈的最顶级调用 Start() 方法一次，以重置进度指示器并获取根范围的访问权限：
//!
//! @code{.cpp}
//! Handle(Message_ProgressIndicator) aProgress = ...;
//! anAlgorithm.Perform (aProgress->Start());
//! @endcode
//!
//! 要在算法中推进进度指示器，使用提供类似迭代器的接口来增加进度的 Message_ProgressScope 类；
//! 参见该类的文档了解详情。Message_ProgressRange 对象如果不传递到任何 Message_ProgressScope，
//! 将自动推进指示器。
//!
//! 进度指示器支持并发处理，可在多线程应用程序中使用。
//!
//! 应创建派生类以将此接口连接到进度指示器的实际实现，
//! 以处理进度的可视化（例如在图形条中显示总位置、在文本模式下打印范围等），
//! 以及用户中断机制的实现（如必要）。
//!
//! 参见 Show() 和 UserBreak() 方法的文档了解详情。

class Message_ProgressIndicator : public Standard_Transient
{
    DEFINE_STANDARD_RTTIEXT(Message_ProgressIndicator, Standard_Transient)
public:
    //!@name Initialization of progress indication
    //!@name 进度指示的初始化

    //! Resets the indicator to zero, calls Reset(), and returns the range.
    //! This range refers to the scope that has no name and is initialized
    //! with max value 1 and step 1.
    //! Use this method to get the top level range for progress indication.
    //!
    //! 将指示器重置为零，调用 Reset()，并返回范围。
    //! 此范围指的是没有名称的范围，初始化时最大值为 1，步骤为 1。
    //! 使用此方法获取进度指示的顶级范围。
    Standard_EXPORT Message_ProgressRange Start();

    //! If argument is non-null handle, returns theProgress->Start().
    //! Otherwise, returns dummy range that can be safely used in the algorithms
    //! but not bound to progress indicator.
    //!
    //! 如果参数是非 null 的 handle，返回 theProgress->Start()。
    //! 否则，返回可安全用于算法但不绑定到进度指示器的虚拟范围。
    Standard_EXPORT static Message_ProgressRange Start
    (const Handle(Message_ProgressIndicator)& theProgress);

protected:
    //!@name Virtual methods to be defined by descendant.
    //!@name 应由派生类定义的虚拟方法

    //! Should return True if user has sent a break signal.
    //!
    //! This method can be called concurrently, thus implementation should
    //! be thread-safe. It should not call Show() or Position() to
    //! avoid possible data races. The method should return as soon
    //! as possible to avoid delaying the calling algorithm.
    //!
    //! Default implementation returns False.
    //!
    //! 如果用户已发送中断信号，应返回 True。
    //!
    //! 此方法可以并发调用，因此实现应该是线程安全的。
    //! 不应调用 Show() 或 Position() 以避免可能的数据竞争。
    //! 该方法应尽快返回以避免延迟调用算法。
    //!
    //! 默认实现返回 False。
    virtual Standard_Boolean UserBreak()
    {
        return Standard_False;
    }

    //! Virtual method to be defined by descendant.
    //! Should update presentation of the progress indicator.
    //!
    //! It is called whenever progress position is changed.
    //! Calls to this method from progress indicator are protected by mutex so that
    //! it is never called concurrently for the same progress indicator instance.
    //! Show() should return as soon as possible to reduce thread contention
    //! in multithreaded algorithms.
    //!
    //! It is recommended to update (redraw, output etc.) only if progress is
    //! advanced by at least 1% from previous update.
    //!
    //! Flag isForce is intended for forcing update in case if it is required
    //! at particular step of the algorithm; all calls to it from inside the core
    //! mechanism (Message_Progress... classes) are done with this flag equal to False.
    //!
    //! The parameter theScope is the current scope being advanced;
    //! it can be used to show the names and ranges of the on-going scope and
    //! its parents, providing more visibility of the current stage of the process.
    //!
    //! 应由派生类定义的虚拟方法。应更新进度指示器的演示。
    //!
    //! 每当进度位置改变时都会调用。来自进度指示器的对这个方法的调用由互斥量保护，
    //! 因此对于同一进度指示器实例，它永远不会被并发调用。Show() 应尽快返回以减少
    //! 多线程算法中的线程竞争。
    //!
    //! 建议仅当进度比上次更新至少推进 1% 时才更新（重绘、输出等）。
    //!
    //! 标记 isForce 用于在算法的特定步骤处强制更新（如果需要）；
    //! 所有来自核心机制（Message_Progress... 类）内部的调用都使用此标记等于 False。
    //!
    //! 参数 theScope 是正在推进的当前范围；它可用于显示正在进行的范围及其父范围的
    //! 名称和范围，提供更多当前过程阶段的可见性。
    virtual void Show(const Message_ProgressScope& theScope,
        const Standard_Boolean isForce) = 0;

    //! Call-back method called by Start(), can be redefined by descendants
    //! if some actions are needed when the indicator is restarted.
    //! 由 Start() 调用的回调方法，如果指示器重新启动时需要一些操作，
    //! 派生类可重新定义它
    virtual void Reset() {}

public:
    //!@name Auxiliary methods
    //!@name 辅助方法

    //! Returns total progress position ranged from 0 to 1.
    //! Should not be called concurrently while the progress is advancing,
    //! except from implementation of method Show().
    //!
    //! 返回范围为 0 到 1 的总进度位置。当进度推进时，不应并发调用，
    //! 除非来自 Show() 方法的实现。
    Standard_Real GetPosition() const
    {
        return myPosition;
    }

    //! Destructor
    //! 析构函数
    Standard_EXPORT ~Message_ProgressIndicator();

protected:

    //! Constructor
    //! 构造函数
    Standard_EXPORT Message_ProgressIndicator();

private:

    //! Increment the progress value by the specified step,
    //! then calls Show() to update presentation.
    //! The parameter theScope is reference to the caller object;
    //! it is passed to Show() where can be used to track context of the process.
    //! 按指定的步骤增加进度值，然后调用 Show() 更新演示。
    //! 参数 theScope 是调用者对象的引用；它传递给 Show()，可用于跟踪过程的上下文。
    void Increment(const Standard_Real theStep, const Message_ProgressScope& theScope);

private:

    Standard_Real myPosition;            //!< 总进度位置，范围为 0 到 1
                                         //!< Total progress position ranged from 0 to 1
    Standard_Mutex myMutex;              //!< 保护 myPosition 免受并发增量
                                         //!< Protection of myPosition from concurrent increment
    Message_ProgressScope* myRootScope;  //!< 根进度范围
                                         //!< The root progress scope

private:
    friend class Message_ProgressScope;  //!< Friend: can call Increment()
    friend class Message_ProgressRange;  //!< Friend: can call Increment()
};

#include <Message_ProgressScope.hxx>

//=======================================================================
//function : Increment
//purpose  : 按指定步骤增加进度值并更新演示
//=======================================================================
inline void Message_ProgressIndicator::Increment(const Standard_Real theStep,
    const Message_ProgressScope& theScope)
{
    // protect incrementation by mutex to avoid problems in multithreaded scenarios
    // （用互斥量保护增量以避免多线程场景中的问题）
    Standard_Mutex::Sentry aSentry(myMutex);

    myPosition = Min(myPosition + theStep, 1.);

    // show progress indicator; note that this call is protected by
    // the same mutex to avoid concurrency and ensure that this call
    // to Show() will see the position exactly as it was just set above
    // （显示进度指示器；注意这个调用受到相同的互斥量保护以避免并发并确保
    //   这个对 Show() 的调用看到的位置正好是上面刚刚设置的）
    Show(theScope, Standard_False);
}

#endif // _Message_ProgressIndicator_HeaderFile
