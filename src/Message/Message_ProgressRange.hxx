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

#ifndef _Message_ProgressRange_HeaderFile
#define _Message_ProgressRange_HeaderFile

#include <Standard_TypeDef.hxx>

class Message_ProgressScope;

//! Auxiliary class representing a part of the global progress scale allocated by
//! a step of the progress scope, see Message_ProgressScope::Next().
//!
//! A range object takes responsibility of advancing the progress by the size of
//! allocated step, which is then performed depending on how it is used:
//!
//! - If Message_ProgressScope object is created using this range as argument, then
//!   this respondibility is taken over by that scope.
//!
//! - Otherwise, a range advances progress directly upon destruction.
//!
//! A range object can be copied, the responsibility for progress advancement is 
//! then taken by the copy.
//! The same range object may be used (either copied or used to create scope) only once.
//! Any consequent attempts to use range will give no result on the progress;
//! in debug mode, an assert message will be generated.
//!
//! @sa Message_ProgressScope for more details
//! 
//! 表示由进度范围的一个步骤分配的全局进度刻度的一部分的辅助类，
//! 参见 Message_ProgressScope::Next()。
//! 
//! 范围对象负责将进度提前所分配步骤的大小，
//! 然后根据其使用方式执行：
//! 
//! - 如果使用此范围作为参数创建 Message_ProgressScope 对象，则
//!   该范围对象的责任由该范围接管。
//! 
//! - 否则，范围在销毁时直接提前进度。
//! 
//! 范围对象可以被复制，责任由副本承担。
//! 同一范围对象只能使用（复制或用于创建范围）一次。
//! 任何后续使用范围的尝试都不会对进度产生影响；
//! 在调试模式下，将生成一个断言消息。
//! 
//! @sa Message_ProgressScope 获取更多详情
class Message_ProgressRange
{
public:
    //! Constructor of the empty range
    //! 空范围的构造函数
    Message_ProgressRange()
        : myParentScope(0), myStart(0.), myDelta(0.), myWasUsed(false)
    {
    }

    //! Copy constructor disarms the source
    //! 拷贝构造函数，复制原对象并禁用原对象
    Message_ProgressRange(const Message_ProgressRange& theOther)
        : myParentScope(theOther.myParentScope),
        myStart(theOther.myStart),
        myDelta(theOther.myDelta),
        myWasUsed(theOther.myWasUsed)
    {
        // discharge theOther（禁用源对象）
        theOther.myWasUsed = true;
    }

    //! Copy assignment disarms the source
    //! 赋值运算符，复制原对象并禁用原对象
    Message_ProgressRange& operator=(const Message_ProgressRange& theOther)
    {
        myParentScope = theOther.myParentScope;
        myStart = theOther.myStart;
        myDelta = theOther.myDelta;
        myWasUsed = theOther.myWasUsed;
        theOther.myWasUsed = true;
        return *this;
    }

    //! Returns true if ProgressIndicator signals UserBreak
    //! 如果 ProgressIndicator 信号为 UserBreak，返回 true
    Standard_Boolean UserBreak() const;

    //! Returns false if ProgressIndicator signals UserBreak
    //! 如果 ProgressIndicator 信号为 UserBreak，返回 false（反向）
    Standard_Boolean More() const
    {
        return !UserBreak();
    }

    //! Returns true if this progress range is attached to some indicator.
    //! 如果该进度范围已附加到某个指示器，返回 true
    Standard_Boolean IsActive() const;

    //! Closes the current range and advances indicator
    //! 关闭当前范围并推进指示器
    void Close();

    //! Destructor
    //! 析构函数
    ~Message_ProgressRange()
    {
        Close();
    }

private:
    //! Constructor is private
    //! 构造函数是私有的（仅供 Message_ProgressScope 使用）
    Message_ProgressRange(const Message_ProgressScope& theParent,
        Standard_Real theStart, Standard_Real theDelta)
        : myParentScope(&theParent),
        myStart(theStart),
        myDelta(theDelta),
        myWasUsed(false)
    {
    }

private:
    const Message_ProgressScope* myParentScope;  //!< 指向父范围的指针
                                                 //!< Pointer to parent scope
    Standard_Real                myStart;        //!< 全局刻度上的起点
                                                 //!< Start point on the global scale
    Standard_Real                myDelta;        //!< 全局刻度上的递增步长
                                                 //!< Step of incrementation on the global scale

    mutable Standard_Boolean     myWasUsed;      //!< 指示该范围是否已被使用以创建新范围的标记
                                                 //!< Flag indicating that this range was used to create a new scope

    friend class Message_ProgressScope;
};

#include <Message_ProgressIndicator.hxx>

//=======================================================================
//function : IsActive
//purpose  : 检查此范围是否处于活跃状态（已绑定到指示器且未被使用）
//=======================================================================
inline Standard_Boolean Message_ProgressRange::IsActive() const
{
    return !myWasUsed && myParentScope && myParentScope->myProgress;
}

//=======================================================================
//function : UserBreak
//purpose  : 检查用户是否请求中断操作
//=======================================================================
inline Standard_Boolean Message_ProgressRange::UserBreak() const
{
    return myParentScope && myParentScope->myProgress && myParentScope->myProgress->UserBreak();
}

//=======================================================================
//function : Close
//purpose  : 关闭范围，向指示器报告进度
//=======================================================================
inline void Message_ProgressRange::Close()
{
    if (!IsActive())
        return;

    myParentScope->myProgress->Increment(myDelta, *myParentScope);
    myParentScope = 0;
    myWasUsed = true;
}

#endif // _Message_ProgressRange_HeaderFile
