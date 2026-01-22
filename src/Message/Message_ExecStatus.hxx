// Created on: 2003-03-04
// Created by: Pavel TELKOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

// The original implementation copyright (c) RINA S.p.A.

#ifndef Message_ExecStatus_HeaderFile
#define Message_ExecStatus_HeaderFile

#include <Message_StatusType.hxx>
#include <Message_Status.hxx>

/**
 * Tiny class for extended handling of error / execution
 * status of algorithm in universal way.
 *
 * It is in fact a set of integers represented as a collection of bit flags
 * for each of four types of status; each status flag has its own symbolic
 * name and can be set/tested individually.
 *
 * The flags are grouped in semantic groups:
 * - No flags means nothing done
 * - Done flags correspond to some operation succesffuly completed
 * - Warning flags correspond to warning messages on some
 *   potentially wrong situation, not harming algorithm execution
 * - Alarm flags correspond to more severe warnings about incorrect
 *   user data, while not breaking algorithm execution
 * - Fail flags correspond to cases when algorithm failed to complete
 *
 * 用于以通用方式扩展处理算法的错误/执行状态的小类。
 *
 * 它实际上是一组整数，表示为四种状态类型中每一种的位标记集合；
 * 每个状态标记有自己的符号名，可以单独设置/测试。
 *
 * 标记按语义组分组：
 * - 无标记表示未完成任何操作
 * - Done 标记对应于成功完成的某些操作
 * - Warning 标记对应于关于某些可能错误情况的警告消息，不影响算法执行
 * - Alarm 标记对应于关于不正确用户数据的更严重警告，虽然不破坏算法执行
 * - Fail 标记对应于算法无法完成的情况
 */

class Message_ExecStatus {

private:
    //! Mask to separate bits indicating status type and index within the type
    //! 用于分离表示状态类型和类型内索引的位的掩码
    enum StatusMask {
        MType = 0x0000ff00, // 状态类型掩码
        MIndex = 0x000000ff // 状态索引掩码
    };
    // 获取对应于状态的位标记
    static inline int getBitFlag(int status) {
        return 0x1 << (status & MIndex);
    }

public:
    //!@name Creation and simple operations with statuses
    //!@name 创建和对状态的简单操作
    //!@{

    //! Create empty execution status
    //! 创建空的执行状态
    Message_ExecStatus() : myDone(Message_None), myWarn(Message_None), myAlarm(Message_None), myFail(Message_None) {}

    //! Initialise the execution status
    //! 初始化执行状态
    Message_ExecStatus(Message_Status status)
        : myDone(Message_None), myWarn(Message_None), myAlarm(Message_None), myFail(Message_None) {
        Set(status);
    }

    //! Sets a status flag
    //! 设置一个状态标记
    void Set(Message_Status status) {
        switch (status & MType) {
            case Message_DONE:
                myDone |= (getBitFlag(status));
                break;
            case Message_WARN:
                myWarn |= (getBitFlag(status));
                break;
            case Message_ALARM:
                myAlarm |= (getBitFlag(status));
                break;
            case Message_FAIL:
                myFail |= (getBitFlag(status));
                break;
            default:
                break;
        }
    }

    //! Check status for being set
    //! 检查状态是否已设置
    Standard_Boolean IsSet(Message_Status status) const {
        switch (status & MType) {
            case Message_DONE:
                return (myDone & getBitFlag(status) ? Standard_True : Standard_False);
            case Message_WARN:
                return (myWarn & getBitFlag(status) ? Standard_True : Standard_False);
            case Message_ALARM:
                return (myAlarm & getBitFlag(status) ? Standard_True : Standard_False);
            case Message_FAIL:
                return (myFail & getBitFlag(status) ? Standard_True : Standard_False);
            default:
                return Standard_False;
        }
    }

    //! Clear one status
    //! 清除一个状态
    void Clear(Message_Status status) {
        switch (status & MType) {
            case Message_DONE:
                myDone &= ~(getBitFlag(status));
                return;
            case Message_WARN:
                myWarn &= ~(getBitFlag(status));
                return;
            case Message_ALARM:
                myAlarm &= ~(getBitFlag(status));
                return;
            case Message_FAIL:
                myFail &= ~(getBitFlag(status));
                return;
            default:
                return;
        }
    }

    //!@}

    //!@name Advanced: Group operations (useful for analysis)
    //!@name 高级：组操作（用于分析）
    //!@{

    //! Check if at least one status of each type is set
    //! 检查每种类型是否至少设置了一个状态
    Standard_Boolean IsDone() const {
        return myDone != Message_None;
    }
    Standard_Boolean IsFail() const {
        return myFail != Message_None;
    }
    Standard_Boolean IsWarn() const {
        return myWarn != Message_None;
    }
    Standard_Boolean IsAlarm() const {
        return myAlarm != Message_None;
    }

    //! Set all statuses of each type
    //! 设置每种类型的所有状态
    void SetAllDone() {
        myDone = ~0;
    }
    void SetAllWarn() {
        myWarn = ~0;
    }
    void SetAllAlarm() {
        myAlarm = ~0;
    }
    void SetAllFail() {
        myFail = ~0;
    }

    //! Clear all statuses of each type
    //! 清除每种类型的所有状态
    void ClearAllDone() {
        myDone = Message_None;
    }
    void ClearAllWarn() {
        myWarn = Message_None;
    }
    void ClearAllAlarm() {
        myAlarm = Message_None;
    }
    void ClearAllFail() {
        myFail = Message_None;
    }

    //! Clear all statuses
    //! 清除所有状态
    void Clear() {
        myDone = myWarn = myAlarm = myFail = Message_None;
    }

    //! Add statuses to me from theOther execution status
    //! 从 theOther 执行状态向此执行状态添加状态
    void Add(const Message_ExecStatus& theOther) {
        myDone |= theOther.myDone;
        myWarn |= theOther.myWarn;
        myAlarm |= theOther.myAlarm;
        myFail |= theOther.myFail;
    }
    const Message_ExecStatus& operator|=(const Message_ExecStatus& theOther) {
        Add(theOther);
        return *this;
    }

    //! Leave only the statuses common with theOther
    //! 仅保留与 theOther 相同的状态
    void And(const Message_ExecStatus& theOther) {
        myDone &= theOther.myDone;
        myWarn &= theOther.myWarn;
        myAlarm &= theOther.myAlarm;
        myFail &= theOther.myFail;
    }
    const Message_ExecStatus& operator&=(const Message_ExecStatus& theOther) {
        And(theOther);
        return *this;
    }

    //@}

public:
    //!@name Advanced: Iteration and analysis of status flags
    //!@name 高级：状态标记的迭代和分析
    //!@{

    //! Definitions of range of available statuses
    //! 可用状态范围的定义
    enum StatusRange {
        FirstStatus = 1,      // 第一个状态
        StatusesPerType = 32, // 每种类型的状态数
        NbStatuses = 128,     // 总状态数
        LastStatus = 129      // 最后一个状态
    };

    //! Returns index of status in whole range [FirstStatus, LastStatus]
    //! 返回整个范围 [FirstStatus, LastStatus] 中状态的索引
    static Standard_Integer StatusIndex(Message_Status status) {
        switch (status & MType) {
            case Message_DONE:
                return 0 * StatusesPerType + LocalStatusIndex(status);
            case Message_WARN:
                return 1 * StatusesPerType + LocalStatusIndex(status);
            case Message_ALARM:
                return 2 * StatusesPerType + LocalStatusIndex(status);
            case Message_FAIL:
                return 3 * StatusesPerType + LocalStatusIndex(status);
            default:
                return 0;
        }
    }

    //! Returns index of status inside type of status (Done or Warn or, etc)
    //! in range [1, StatusesPerType]
    //! 返回状态类型（Done 或 Warn 等）内的状态索引，范围为 [1, StatusesPerType]
    static Standard_Integer LocalStatusIndex(Message_Status status) {
        return (status & MIndex) + 1;
    }

    //! Returns status type (DONE, WARN, ALARM, or FAIL)
    //! 返回状态类型（DONE、WARN、ALARM 或 FAIL）
    static Message_StatusType TypeOfStatus(Message_Status status) {
        return (Message_StatusType)(status & MType);
    }

    //! Returns status with index theIndex in whole range [FirstStatus, LastStatus]
    //! 返回整个范围 [FirstStatus, LastStatus] 中索引为 theIndex 的状态
    static Message_Status StatusByIndex(const Standard_Integer theIndex) {
        Standard_Integer indx = theIndex - 1;
        if (indx < 32)
            return (Message_Status)(Message_DONE + indx);
        else if (indx < 64)
            return (Message_Status)(Message_WARN + (indx - 32));
        else if (indx < 96)
            return (Message_Status)(Message_ALARM + (indx - 64));
        else if (indx < 128)
            return (Message_Status)(Message_FAIL + (indx - 96));
        return Message_None;
    }

    //!@}

private:
    // ---------- PRIVATE FIELDS ----------
    Standard_Integer myDone;  // Done 状态标记
    Standard_Integer myWarn;  // Warning 状态标记
    Standard_Integer myAlarm; // Alarm 状态标记
    Standard_Integer myFail;  // Fail 状态标记
};

#endif
