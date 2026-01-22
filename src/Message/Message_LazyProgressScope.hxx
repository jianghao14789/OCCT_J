// Copyright (c) 2017-2021 OPEN CASCADE SAS
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

#ifndef _Message_LazyProgressScope_HeaderFiler
#define _Message_LazyProgressScope_HeaderFiler

#include <Message_ProgressScope.hxx>

//! Progress scope with lazy updates and abort fetches.
//!
//! Although Message_ProgressIndicator implementation is encouraged to spare GUI updates,
//! even optimized implementation might show a noticeable overhead on a very small update step (e.g. per triangle).
//!
//! The class splits initial (displayed) number of overall steps into larger chunks specified in constructor,
//! so that displayed progress is updated at larger steps.
//!
//! 具有延迟更新和中止获取的进度范围。
//!
//! 虽然建议 Message_ProgressIndicator 实现节省 GUI 更新，
//! 但即使是优化的实现在非常小的更新步骤（例如每个三角形）上也可能会显示明显的开销。
//!
//! 该类将构造函数中指定的初始（显示）的总步骤数分成更大的块，
//! 以便显示的进度在更大的步骤处更新。
class Message_LazyProgressScope : protected Message_ProgressScope {
public:
    //! Main constructor.
    //! @param theRange [in] progress range to scope
    //! @param theName  [in] name of this scope
    //! @param theMax   [in] number of steps within this scope
    //! @param thePatchStep [in] number of steps to update progress
    //! @param theIsInf [in] infinite flag
    //!
    //! 主构造函数。
    //! @param theRange [in] 要作用的进度范围
    //! @param theName  [in] 此范围的名称
    //! @param theMax   [in] 此范围内的步骤数
    //! @param thePatchStep [in] 更新进度的步骤数
    //! @param theIsInf [in] 无限标记
    Message_LazyProgressScope(const Message_ProgressRange& theRange, const char* theName, const Standard_Real theMax,
                              const Standard_Real thePatchStep, const Standard_Boolean theIsInf = Standard_False)
        : Message_ProgressScope(theRange, theName, theMax, theIsInf), myPatchStep(thePatchStep), myPatchProgress(0.0),
          myIsLazyAborted(Standard_False) {}

    //! Increment progress with 1.
    //! 将进度增加 1
    void Next() {
        if (++myPatchProgress < myPatchStep) {
            return;
        }

        myPatchProgress = 0.0;
        Message_ProgressScope::Next(myPatchStep);
        IsAborted();
    }

    //! Return TRUE if progress has been aborted - return the cached state lazily updated.
    //! 如果进度已被中止，返回 TRUE - 返回延迟更新的缓存状态
    Standard_Boolean More() const {
        return !myIsLazyAborted;
    }

    //! Return TRUE if progress has been aborted - fetches actual value from the Progress.
    //! 如果进度已被中止，返回 TRUE - 从 Progress 获取实际值
    Standard_Boolean IsAborted() {
        myIsLazyAborted = myIsLazyAborted || !Message_ProgressScope::More();
        return myIsLazyAborted;
    }

protected:
    Standard_Real myPatchStep;        // 更新进度的步骤数
    Standard_Real myPatchProgress;    // 当前补丁进度
    Standard_Boolean myIsLazyAborted; // 延迟中止状态
};

#endif // _Message_LazyProgressScope_HeaderFiler
