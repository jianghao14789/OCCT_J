// Created on: 2002-02-22
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

#ifndef _Message_ProgressScope_HeaderFile
#define _Message_ProgressScope_HeaderFile

#include <Standard_Assert.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>

class Message_ProgressRange;
class Message_ProgressIndicator;

//! Message_ProgressScope class provides convenient way to advance progress
//! indicator in context of complex program organized in hierarchical way,
//! where usually it is difficult (or even not possible) to consider process
//! as linear with fixed step.
//!
//! On every level (sub-operation) in hierarchy of operations
//! the local instance of the Message_ProgressScope class is created.
//! It takes a part of the upper-level scope (via Message_ProgressRange) and provides
//! a way to consider this part as independent scale with locally defined range.
//!
//! The position on the local scale may be advanced using the method Next(),
//! which allows iteration-like advancement. This method can take argument to
//! advance by the specified value (with default step equal to 1).
//! This method returns Message_ProgressRange object that takes responsibility
//! of making the specified step, either directly at its destruction or by
//! delegating this task to another sub-scope created from that range object.
//!
//! It is important that sub-scope must have life time less than
//! the life time of its parent scope that provided the range.
//! The usage pattern is to create scope objects as local variables in the
//! functions that do the job, and pass range objects returned by Next() to
//! the functions of the lower level, to allow them creating their own scopes.
//!
//! The scope has a name that can be used in visualization of the progress.
//! It can be null. Note that when C string literal is used as a name, then its
//! value is not copied, just pointer is stored. In other variants (char pointer
//! or a string class) the string is copied, which is additional overhead.
//!
//! The same instance of the progress scope! must not be used concurrently from different threads.
//! For the algorithm running its tasks in parallel threads, a common scope is
//! created before the parallel execution, and the range objects produced by method
//! Next() are used to initialise the data pertinent to each task.
//! Then the progress is advanced within each task using its own range object.
//! See example below.
//!
//! Note that while a range of the scope is specified using Standard_Real
//! (double) parameter, it is expected to be a positive integer value.
//! If the range is not an integer, method Next() shall be called with
//! explicit step argument, and the rounded value returned by method Value()
//! may be not coherent with the step and range.
//!
//! A scope can be created with option "infinite". This is useful when
//! the number of steps is not known by the time of the scope creation.
//! In this case the progress will be advanced logarithmically, approaching
//! the end of the scope at infinite number of steps. The parameter Max
//! for infinite scope indicates number of steps corresponding to mid-range.
//!
//! A progress scope created with empty constructor is not connected to any
//! progress indicator, and passing the range created on it to any algorithm
//! allows it executing safely without actual progress indication.
//!
//! Example of preparation of progress indicator:
//!
//! @code{.cpp}
//!   Handle(Message_ProgressIndicator) aProgress = ...; // assume it can be null
//!   func (Message_ProgressIndicator::Start (aProgress));
//! @endcode
//!
//! Example of usage in sequential process:
//!
//! @code{.cpp}
//!   Message_ProgressScope aWholePS(aRange, "Whole process", 100);
//!
//!   // do one step taking 20%
//!   func1 (aWholePS.Next (20)); // func1 will take 20% of the whole scope
//!   if (aWholePS.UserBreak()) // exit prematurely if the user requested break
//!     return;
//!
//!   // ... do next step taking 50%
//!   func2 (aWholePS.Next (50));
//!   if (aWholePS.UserBreak())
//!     return;
//! @endcode
//!
//! Example of usage in nested cycle:
//!
//! @code{.cpp}
//!   // Outer cycle
//!   Message_ProgressScope anOuter (theProgress, "Outer", nbOuter);
//!   for (Standard_Integer i = 0; i < nbOuter && anOuter.More(); i++)
//!   {
//!     // Inner cycle
//!     Message_ProgressScope anInner (anOuter.Next(), "Inner", nbInner);
//!     for (Standard_Integer j = 0; j < nbInner && anInner.More(); j++)
//!     {
//!       // Cycle body
//!       func (anInner.Next());
//!     }
//!   }
//! @endcode
//!
//! Example of use in function:
//!
//! @code{.cpp}
//! //! Implementation of iterative algorithm showing its progress
//! func (const Message_ProgressRange& theProgress)
//! {
//!   // Create local scope covering the given progress range.
//!   // Set this scope to count aNbSteps steps.
//!   Message_ProgressScope aScope (theProgress, "", aNbSteps);
//!   for (Standard_Integer i = 0; i < aNbSteps && aScope.More(); i++)
//!   {
//!     // Optional: pass range returned by method Next() to the nested algorithm
//!     // to allow it to show its progress too (by creating its own scope object).
//!     // In any case the progress will advance to the next step by the end of the func2 call.
//!     func2 (aScope.Next());
//!   }
//! }
//! @endcode
//!
//! Example of usage in parallel process:
//!
//! @code{.cpp}
//! struct Task
//! {
//!   Data& Data;
//!   Message_ProgressRange Range;
//!
//!   Task (const Data& theData, const Message_ProgressRange& theRange)
//!     : Data (theData), Range (theRange) {}
//! };
//! struct Functor
//! {
//!   void operator() (Task& theTask) const
//!   {
//!     // Note: it is essential that this method is executed only once for the same Task object
//!     Message_ProgressScope aPS (theTask.Range, NULL, theTask.Data.NbItems);
//!     for (Standard_Integer i = 0; i < theTask.Data.NbSteps && aPS.More(); i++)
//!     {
//!       do_job (theTask.Data.Item[i], aPS.Next());
//!     }
//!   }
//! };
//! ...
//! {
//!   std::vector<Data> aData = ...;
//!   std::vector<Task> aTasks;
//!
//!   Message_ProgressScope aPS (aRootRange, "Data processing", aData.size());
//!   for (Standard_Integer i = 0; i < aData.size(); ++i)
//!     aTasks.push_back (Task (aData[i], aPS.Next()));
//!
//!   OSD_Parallel::ForEach (aTasks.begin(), aTasks.end(), Functor());
//! }
//! @endcode
//!
//! For lightweight algorithms that do not need advancing the progress
//! within individual tasks the code can be simplified to avoid inner scopes:
//!
//! @code
//! struct Functor
//! {
//!   void operator() (Task& theTask) const
//!   {
//!     if (theTask.Range.More())
//!     {
//!       do_job (theTask.Data);
//!       // advance the progress
//!       theTask.Range.Close();
//!     }
//!   }
//! };
//! @endcode
//!
//! Message_ProgressScope 类提供了在上下文中推进进度指示器的便利方式，
//! 该上下文是按层次方式组织的复杂程序，通常很难（甚至不可能）将流程
//! 视为具有固定步骤的线性流程。
//!
//! 在操作层次结构的每个级别（子操作）中，都会创建 Message_ProgressScope 类的
//! 本地实例。它获取上级范围的一部分（通过 Message_ProgressRange）并提供
//! 将该部分视为具有本地定义范围的独立刻度的方法。
//!
//! 在本地刻度上的位置可以使用 Next() 方法推进，该方法允许类似迭代的推进。
//! 此方法可以接受参数，通过指定的值推进（默认步骤等于 1）。
//! 此方法返回 Message_ProgressRange 对象，该对象负责执行指定的步骤，
//! 要么在其销毁时直接执行，要么通过委托此任务给从该范围对象创建的另一个子范围。
//!
//! 重要的是，子范围的生命周期必须少于其父范围的生命周期，后者提供了范围。
//! 使用模式是在执行工作的函数中将范围对象作为局部变量创建，
//! 并将 Next() 返回的范围对象传递给较低级别的函数，以允许它们创建自己的范围。
//!
//! 范围具有可用于可视化进度的名称。它可以是 null。
//! 请注意，当 C 字符串文字用作名称时，其值不会被复制，只是存储指针。
//! 在其他变体（字符指针或字符串类）中，字符串会被复制，这会带来额外的开销。
//!
//! 同一实例的进度范围！不得从不同线程并发使用。
//! 对于在并行线程中运行其任务的算法，
//! 在并行执行之前会创建一个公共范围，
//! 并且通过 Next() 方法生成的范围对象用于初始化与每个任务相关的数据。
//! 然后，通过每个任务使用其自己的范围对象推进进度。
//! 请参阅下面的示例。
//!
//! 请注意，虽然范围是使用 Standard_Real（双精度）参数指定的，但
//! 它的值应该是一个正整数值。如果范围不是整数，
//! 方法 Next() 应该带有明确的步骤参数被调用，
//! 并且方法 Value() 返回的四舍五入值可能与步骤和范围不一致。
//!
//! 可以创建具有“无限”选项的范围。当在范围创建时步骤数未知时，这很有用。
//! 在这种情况下，进度将以对数方式推进，在无限数量的步骤中接近范围的末尾。
//! 无限范围的 Max 参数指示对应于中间范围的步骤数。
//!
//! 使用空构造函数创建的进度范围没有连接到任何进度指示器，
//! 将其上创建的范围传递给任何算法允许它在没有实际进度指示的情况下安全执行。
//!
//! 进度指示器准备示例：
//!
//! @code{.cpp}
//!   Handle(Message_ProgressIndicator) aProgress = ...; // assume it can be null
//!   func (Message_ProgressIndicator::Start (aProgress));
//! @endcode
//!
//! 按顺序处理中的用法示例：
//!
//! @code{.cpp}
//!   Message_ProgressScope aWholePS(aRange, "Whole process", 100);
//!
//!   // do one step taking 20%
//!   func1 (aWholePS.Next (20)); // func1 will take 20% of the whole scope
//!   if (aWholePS.UserBreak()) // exit prematurely if the user requested break
//!     return;
//!
//!   // ... do next step taking 50%
//!   func2 (aWholePS.Next (50));
//!   if (aWholePS.UserBreak())
//!     return;
//! @endcode
//!
//! 嵌套循环中的用法示例：
//!
//! @code{.cpp}
//!   // Outer cycle
//!   Message_ProgressScope anOuter (theProgress, "Outer", nbOuter);
//!   for (Standard_Integer i = 0; i < nbOuter && anOuter.More(); i++)
//!   {
//!     // Inner cycle
//!     Message_ProgressScope anInner (anOuter.Next(), "Inner", nbInner);
//!     for (Standard_Integer j = 0; j < nbInner && anInner.More(); j++)
//!     {
//!       // Cycle body
//!       func (anInner.Next());
//!     }
//!   }
//! @endcode
//!
//! 在函数中的用法示例：
//!
//! @code{.cpp}
//! //! Implementation of iterative algorithm showing its progress
//! func (const Message_ProgressRange& theProgress)
//! {
//!   // Create local scope covering the given progress range.
//!   // Set this scope to count aNbSteps steps.
//!   Message_ProgressScope aScope (theProgress, "", aNbSteps);
//!   for (Standard_Integer i = 0; i < aNbSteps && aScope.More(); i++)
//!   {
//!     // Optional: pass range returned by method Next() to the nested algorithm
//!     // to allow it to show its progress too (by creating its own scope object).
//!     // In any case the progress will advance to the next step by the end of the func2 call.
//!     func2 (aScope.Next());
//!   }
//! }
//! @endcode
//!
//! 并行处理中的用法示例：
//!
//! @code{.cpp}
//! struct Task
//! {
//!   Data& Data;
//!   Message_ProgressRange Range;
//!
//!   Task (const Data& theData, const Message_ProgressRange& theRange)
//!     : Data (theData), Range (theRange) {}
//! };
//! struct Functor
//! {
//!   void operator() (Task& theTask) const
//!   {
//!     // Note: it is essential that this method is executed only once for the same Task object
//!     Message_ProgressScope aPS (theTask.Range, NULL, theTask.Data.NbItems);
//!     for (Standard_Integer i = 0; i < theTask.Data.NbSteps && aPS.More(); i++)
//!     {
//!       do_job (theTask.Data.Item[i], aPS.Next());
//!     }
//!   }
//! };
//! ...
//! {
//!   std::vector<Data> aData = ...;
//!   std::vector<Task> aTasks;
//!
//!   Message_ProgressScope aPS (aRootRange, "Data processing", aData.size());
//!   for (Standard_Integer i = 0; i < aData.size(); ++i)
//!     aTasks.push_back (Task (aData[i], aPS.Next()));
//!
//!   OSD_Parallel::ForEach (aTasks.begin(), aTasks.end(), Functor());
//! }
//! @endcode
//!
//! 对于不需要在各个任务内部推进进度的轻量级算法，
//! 代码可以简化以避免内部范围：
//!
//! @code
//! struct Functor
//! {
//!   void operator() (Task& theTask) const
//!   {
//!     if (theTask.Range.More())
//!     {
//!       do_job (theTask.Data);
//!       // advance the progress
//!       theTask.Range.Close();
//!     }
//!   }
//! };
//! @endcode
class Message_ProgressScope {
public:
    class NullString; //!< auxiliary type for passing NULL name to Message_ProgressScope constructor
                      //!< 用于将 NULL 名称传递给 Message_ProgressScope 构造函数的辅助类型
public:               //! @name Preparation methods
        //! @name 准备方法
    //! Creates dummy scope.
    //! It can be safely passed to algorithms; no progress indication will be done.
    //! 创建虚拟范围。可以安全地传递给算法；不会进行进度指示
    Message_ProgressScope()
        : myProgress(0), myParent(0), myName(0), myStart(0.), myPortion(1.), myMax(1.), myValue(0.), myIsActive(false),
          myIsOwnName(false), myIsInfinite(false) {}

    //! Creates a new scope taking responsibility of the part of the progress
    //! scale described by theRange. The new scope has own range from 0 to
    //! theMax, which is mapped to the given range.
    //!
    //! The topmost scope is created and owned by Message_ProgressIndicator
    //! and its pointer is contained in the Message_ProgressRange returned by the Start() method of progress indicator.
    //!
    //! @param theRange [in][out] range to fill (will be disarmed)
    //! @param theName  [in]      new scope name
    //! @param theMax   [in]      number of steps in scope
    //! @param isInfinite [in]    infinite flag
    //!
    //! 创建一个新的范围，负责由 theRange 描述的进度刻度的部分。
    //! 新范围的范围为 0 到 theMax，这被映射到给定的范围。
    //!
    //! 最高级别的范围由 Message_ProgressIndicator 创建和拥有，
    //! 其指针包含在进度指示器的 Start() 方法返回的 Message_ProgressRange 中。
    //!
    //! @param theRange [in][out] 要填充的范围（将被禁用）
    //! @param theName  [in]      新范围的名称
    //! @param theMax   [in]      范围内的步骤数
    //! @param isInfinite [in]    无限标记
    Message_ProgressScope(const Message_ProgressRange& theRange, const TCollection_AsciiString& theName,
                          Standard_Real theMax, Standard_Boolean isInfinite = false);

    //! Creates a new scope taking responsibility of the part of the progress
    //! scale described by theRange. The new scope has own range from 0 to
    //! theMax, which is mapped to the given range.
    //!
    //! The topmost scope is created and owned by Message_ProgressIndicator
    //! and its pointer is contained in the Message_ProgressRange returned by the Start() method of progress indicator.
    //!
    //! @param theRange [in][out] range to fill (will be disarmed)
    //! @param theName  [in]      new scope name constant (will be stored by pointer with no deep copy)
    //! @param theMax   [in]      number of steps in scope
    //! @param isInfinite [in]    infinite flag
    //!
    //! 创建一个新的范围（使用 C 字符串字面量作为名称，只存储指针，不进行深拷贝）
    template <size_t N>
    Message_ProgressScope(const Message_ProgressRange& theRange, const char (&theName)[N], Standard_Real theMax,
                          Standard_Boolean isInfinite = false);

    //! Creates a new scope taking responsibility of the part of the progress
    //! scale described by theRange. The new scope has own range from 0 to
    //! theMax, which is mapped to the given range.
    //!
    //! The topmost scope is created and owned by Message_ProgressIndicator
    //! and its pointer is contained in the Message_ProgressRange returned by the Start() method of progress indicator.
    //!
    //! @param theRange [in][out] range to fill (will be disarmed)
    //! @param theName  [in]      empty scope name (only NULL is accepted as argument)
    //! @param theMax   [in]      number of steps in scope
    //! @param isInfinite [in]    infinite flag
    //!
    //! 创建一个新的范围（名称为 NULL）
    Message_ProgressScope(const Message_ProgressRange& theRange, const NullString* theName, Standard_Real theMax,
                          Standard_Boolean isInfinite = false);

    //! Sets the name of the scope.
    //! 设置范围的名称
    void SetName(const TCollection_AsciiString& theName) {
        if (myIsOwnName) {
            Standard::Free(myName);
            myIsOwnName = false;
        }
        myName = NULL;
        if (!theName.IsEmpty()) {
            myIsOwnName = true;
            myName = (char*)Standard::Allocate(theName.Length() + 1);
            char* aName = (char*)myName;
            memcpy(aName, theName.ToCString(), theName.Length());
            aName[theName.Length()] = '\0';
        }
    }

    //! Sets the name of the scope; can be null.
    //! Note! Just pointer to the given string is copied,
    //! so do not pass string from a temporary variable whose
    //! lifetime is less than that of this object.
    //!
    //! 设置范围的名称；可以为 null。
    //! 注意！只复制指向给定字符串的指针，因此不要传递来自临时变量的字符串，
    //! 其生命周期小于此对象的生命周期
    template <size_t N> void SetName(const char (&theName)[N]) {
        if (myIsOwnName) {
            Standard::Free(myName);
            myIsOwnName = false;
        }
        myName = theName;
    }

public: //! @name Advance by iterations
        //! @name 按迭代推进
    //! Returns true if ProgressIndicator signals UserBreak
    //! 如果 ProgressIndicator 信号为 UserBreak，返回 true
    Standard_Boolean UserBreak() const;

    //! Returns false if ProgressIndicator signals UserBreak
    //! 如果 ProgressIndicator 信号为 UserBreak，返回 false（反向）
    Standard_Boolean More() const {
        return !UserBreak();
    }

    //! Advances position by specified step and returns the range
    //! covering this step
    //! 按指定的步骤推进位置并返回覆盖该步骤的范围
    Message_ProgressRange Next(Standard_Real theStep = 1.);

public: //! @name Auxiliary methods to use in ProgressIndicator
        //! @name 在 ProgressIndicator 中使用的辅助方法
    //! Force update of presentation of the progress indicator.
    //! Should not be called concurrently.
    //! 强制更新进度指示器的演示。不应同时调用。
    void Show();

    //! Returns true if this progress scope is attached to some indicator.
    //! 如果该进度范围已附加到某个指示器，返回 true
    Standard_Boolean IsActive() const {
        return myIsActive;
    }

    //! Returns the name of the scope (may be null).
    //! Scopes with null name (e.g. root scope) should
    //! be bypassed when reporting progress to the user.
    //! 返回范围的名称（可能为 null）。
    //! 向用户报告进度时，应跳过具有 null 名称的范围（例如根范围）
    Standard_CString Name() const {
        return myName;
    }

    //! Returns parent scope (null for top-level scope)
    //! 返回父范围（顶级范围为 null）
    const Message_ProgressScope* Parent() const {
        return myParent;
    }

    //! Returns the maximal value of progress in this scope
    //! 返回此范围内的最大进度值
    Standard_Real MaxValue() const {
        return myMax;
    }

    //! Returns the current value of progress in this scope.
    //!
    //! The value is computed by mapping current global progress into
    //! this scope range; the result is rounded up to integer.
    //! Note that if MaxValue() is not an integer, Value() can be
    //! greater than MaxValue() due to that rounding.
    //!
    //! This method should not be called concurrently while the progress
    //! is advancing, except from implementation of method Show() in
    //! descendant of Message_ProgressIndicator.
    //!
    //! 返回此范围内的当前进度值。
    //! 通过将当前全局进度映射到此范围的范围来计算值；
    //! 结果四舍五入到整数。注意，如果 MaxValue() 不是整数，
    //! 由于四舍五入，Value() 可能大于 MaxValue()。
    //!
    //! 当进度推进时，不应同时调用此方法，
    //! 除非从 Message_ProgressIndicator 的后代的 Show() 方法的实现中调用
    Standard_Real Value() const;

    //! Returns the infinite flag
    //! 返回无限标记
    Standard_Boolean IsInfinite() const {
        return myIsInfinite;
    }

    //! Get the portion of the indicator covered by this scope (from 0 to 1)
    //! 获取此范围覆盖的指示器的部分（从 0 到 1）
    Standard_Real GetPortion() const {
        return myPortion;
    }

public: //! @name Destruction, allocation
        //! @name 销毁、分配
    //! Destructor - closes the scope and adds its scale to the total progress
    //! 析构函数 - 关闭范围并将其刻度添加到总体进度
    ~Message_ProgressScope() {
        Close();
        if (myIsOwnName) {
            Standard::Free(myName);
            myIsOwnName = false;
            myName = NULL;
        }
    }

    //! Closes the scope and advances the progress to its end.
    //! Closed scope should not be used.
    //! 关闭范围并将进度推进到其末尾。不应使用已关闭的范围。
    void Close();

    DEFINE_STANDARD_ALLOC;

private: //! @name Internal methods
         //! @name 内部方法
    //! Creates a top-level scope with default range [0,1] and step 1.
    //! Called only by Message_ProgressIndicator constructor.
    //! 创建具有默认范围 [0,1] 和步骤 1 的顶级范围。仅由 Message_ProgressIndicator 构造函数调用
    Message_ProgressScope(Message_ProgressIndicator* theProgress);

    //! Convert value from this scope to global scale, but disregarding
    //! start position of the scope, in the range [0, myPortion]
    //! 将值从此范围转换为全局刻度，但不考虑范围的起始位置，范围为 [0, myPortion]
    Standard_Real localToGlobal(const Standard_Real theVal) const;

private:
    //! Copy constructor is prohibited
    //! 拷贝构造函数被禁止
    Message_ProgressScope(const Message_ProgressScope& theOther);

    //! Copy assignment is prohibited
    //! 拷贝赋值被禁止
    Message_ProgressScope& operator=(const Message_ProgressScope& theOther);

private:
    Message_ProgressIndicator* myProgress; //!< 指向进度指示器实例的指针
                                           //!< Pointer to progress indicator instance
    const Message_ProgressScope* myParent; //!< 指向父范围的指针
                                           //!< Pointer to parent scope
    Standard_CString myName;               //!< 在此范围内进行的操作的名称，或 null
                                           //!< Name of the operation being done in this scope, or null

    Standard_Real myStart;   //!< 全局刻度上的起始位置 [0, 1]
                             //!< Start position on the global scale [0, 1]
    Standard_Real myPortion; //!< 此范围覆盖的全局刻度的部分 [0, 1]
                             //!< The portion of the global scale covered by this scope [0, 1]

    Standard_Real myMax;   //!< 此范围内的最大进度值
                           //!< Maximal value of progress in this scope
    Standard_Real myValue; //!< 在此范围内推进的当前位置 [0, Max]
                           //!< Current position advanced within this scope [0, Max]

    Standard_Boolean myIsActive;   //!< 指示装备/未装备状态的标记
                                   //!< flag indicating armed/disarmed state
    Standard_Boolean myIsOwnName;  //!< 指示名称是否已分配的标记
                                   //!< flag indicating if name was allocated or not
    Standard_Boolean myIsInfinite; //!< 按双曲线规律推进的选项
                                   //!< Option to advance by hyperbolic law

private:
    friend class Message_ProgressIndicator;
    friend class Message_ProgressRange;
};

#include <Message_ProgressRange.hxx>

//=======================================================================
// function : Message_ProgressScope
// purpose  : 使用进度指示器初始化顶级范围
//=======================================================================
inline Message_ProgressScope::Message_ProgressScope(Message_ProgressIndicator* theProgress)
    : myProgress(theProgress), myParent(0), myName(0), myStart(0.), myPortion(1.), myMax(1.), myValue(0.),
      myIsActive(theProgress != NULL), myIsOwnName(false), myIsInfinite(false) {}

//=======================================================================
// function : Message_ProgressScope
// purpose  : 从 ProgressRange 和 AsciiString 名称初始化范围
//=======================================================================
inline Message_ProgressScope::Message_ProgressScope(const Message_ProgressRange& theRange,
                                                    const TCollection_AsciiString& theName, Standard_Real theMax,
                                                    Standard_Boolean isInfinite)
    : myProgress(theRange.myParentScope != NULL ? theRange.myParentScope->myProgress : NULL),
      myParent(theRange.myParentScope), myName(NULL), myStart(theRange.myStart), myPortion(theRange.myDelta),
      myMax(Max(1.e-6, theMax)), // protection against zero range（防止零范围）
      myValue(0.), myIsActive(myProgress != NULL && !theRange.myWasUsed), myIsOwnName(false), myIsInfinite(isInfinite) {
    SetName(theName);
    Standard_ASSERT_VOID(!theRange.myWasUsed, "Message_ProgressRange is used to initialize more than one scope");
    theRange.myWasUsed = true; // Disarm the range（禁用范围）
}

//=======================================================================
// function : Message_ProgressScope
// purpose  : 从 ProgressRange 和 C 字符串字面量名称初始化范围
//=======================================================================
template <size_t N>
Message_ProgressScope::Message_ProgressScope(const Message_ProgressRange& theRange, const char (&theName)[N],
                                             Standard_Real theMax, Standard_Boolean isInfinite)
    : myProgress(theRange.myParentScope != NULL ? theRange.myParentScope->myProgress : NULL),
      myParent(theRange.myParentScope), myName(theName), myStart(theRange.myStart), myPortion(theRange.myDelta),
      myMax(Max(1.e-6, theMax)), // protection against zero range（防止零范围）
      myValue(0.), myIsActive(myProgress != NULL && !theRange.myWasUsed), myIsOwnName(false), myIsInfinite(isInfinite) {
    Standard_ASSERT_VOID(!theRange.myWasUsed, "Message_ProgressRange is used to initialize more than one scope");
    theRange.myWasUsed = true; // Disarm the range（禁用范围）
}

//=======================================================================
// function : Message_ProgressScope
// purpose  : 从 ProgressRange 初始化范围，名称为 NULL
//=======================================================================
inline Message_ProgressScope::Message_ProgressScope(const Message_ProgressRange& theRange, const NullString*,
                                                    Standard_Real theMax, Standard_Boolean isInfinite)
    : myProgress(theRange.myParentScope != NULL ? theRange.myParentScope->myProgress : NULL),
      myParent(theRange.myParentScope), myName(NULL), myStart(theRange.myStart), myPortion(theRange.myDelta),
      myMax(Max(1.e-6, theMax)), // protection against zero range（防止零范围）
      myValue(0.), myIsActive(myProgress != NULL && !theRange.myWasUsed), myIsOwnName(false), myIsInfinite(isInfinite) {
    Standard_ASSERT_VOID(!theRange.myWasUsed, "Message_ProgressRange is used to initialize more than one scope");
    theRange.myWasUsed = true; // Disarm the range（禁用范围）
}

//=======================================================================
// function : Close
// purpose  : 关闭范围并推进指示器到范围的末尾
//=======================================================================
inline void Message_ProgressScope::Close() {
    if (!myIsActive) {
        return;
    }

    // Advance indicator to the end of the scope（将指示器推进到范围的末尾）
    Standard_Real aCurr = localToGlobal(myValue);
    myValue = (myIsInfinite ? Precision::Infinite() : myMax);
    Standard_Real aDelta = myPortion - aCurr;
    if (aDelta > 0.) {
        myProgress->Increment(aDelta, *this);
    }
    Standard_ASSERT_VOID(myParent == 0 || myParent->myIsActive, "Parent progress scope has been closed before child");

    myIsActive = false;
}

//=======================================================================
// function : UserBreak
// purpose  : 检查用户是否请求中断操作
//=======================================================================
inline Standard_Boolean Message_ProgressScope::UserBreak() const {
    return myProgress && myProgress->UserBreak();
}

//=======================================================================
// function : Next
// purpose  : 按指定步骤推进并返回对应的范围
//=======================================================================
inline Message_ProgressRange Message_ProgressScope::Next(Standard_Real theStep) {
    if (myIsActive && theStep > 0.) {
        Standard_Real aCurr = localToGlobal(myValue);
        Standard_Real aNext = localToGlobal(myValue += theStep);
        Standard_Real aDelta = aNext - aCurr;
        if (aDelta > 0.) {
            return Message_ProgressRange(*this, myStart + aCurr, aDelta);
        }
    }
    return Message_ProgressRange();
}

//=======================================================================
// function : Show
// purpose  : 强制更新进度指示器的显示
//=======================================================================

inline void Message_ProgressScope::Show() {
    if (myIsActive) {
        myProgress->Show(*this, Standard_True);
    }
}

//=======================================================================
// function : localToGlobal
// purpose  : 将值从本地范围转换为全局刻度
//=======================================================================
inline Standard_Real Message_ProgressScope::localToGlobal(const Standard_Real theVal) const {
    if (theVal <= 0.) return 0.;

    if (!myIsInfinite) {
        if (myMax - theVal < RealSmall()) return myPortion;
        return myPortion * theVal / myMax;
    }

    double x = theVal / myMax;
    // return myPortion * ( 1. - std::exp ( -x ) ); // exponent（指数）
    return myPortion * x / (1. + x); // hyperbola（双曲线）
}

//=======================================================================
// function : Value
// purpose  : 返回此范围内的当前进度值
//=======================================================================

inline Standard_Real Message_ProgressScope::Value() const {
    if (!myIsActive) {
        return myIsInfinite ? Precision::Infinite() : myMax;
    }

    // get current progress on the global scale counted
    // from the start of this scope
    // （从此范围的开始计数的全局刻度上的当前进度）
    Standard_Real aVal = myProgress->GetPosition() - myStart;

    // if progress has not reached yet the start of this scope, return 0
    // （如果进度尚未达到此范围的开始，返回 0）
    if (aVal <= 0.) return 0.;

    // if at end of the scope (or behind), report the maximum
    // （如果在范围的末尾（或之后），报告最大值）
    Standard_Real aDist = myPortion - aVal;
    if (aDist <= Precision::Confusion()) return myIsInfinite ? Precision::Infinite() : myMax;

    // map the value to the range of this scope [0, Max],
    // rounding up to integer, with small correction applied
    // to avoid rounding errors
    // （将值映射到此范围 [0, Max]，四舍五入到整数，应用小的修正以避免舍入错误）
    return std::ceil(myMax * aVal / (myIsInfinite ? aDist : myPortion) - Precision::Confusion());
}

#endif // _Message_ProgressScope_HeaderFile
