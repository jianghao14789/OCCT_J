// Created on: 1991-05-14
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _math_BracketMinimum_HeaderFile
#define _math_BracketMinimum_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class math_Function;

//! Given two distinct initial points, BracketMinimum
//! implements the computation of three points (a, b, c) which
//! bracket the minimum of the function and verify A less than
//! B, B less than C and F(B) less than F(A), F(B) less than F(C).
//!
//! The algorithm supports conditional optimization. By default no limits are
//! applied to the parameter change. The method SetLimits defines the allowed range.
//! If no minimum is found in limits then IsDone() will return false. The user
//! is in charge of providing A and B to be in limits.
//!
//! 给定两个不同的初始点，BracketMinimum 实现三个点 (a, b, c) 的计算，
//! 这三个点将函数的最小值括起来，并满足 A < B, B < C，以及 F(B) < F(A), F(B) < F(C)。
//!
//! 该算法支持条件优化。默认情况下，参数的变化不受限制。SetLimits 方法定义了
//! 允许的范围。如果在限制范围内未找到最小值，则 IsDone() 将返回 false。用户
//! 负责提供在限制范围内的 A 和 B。
class math_BracketMinimum {
public:

    DEFINE_STANDARD_ALLOC;

    //! Constructor preparing A and B parameters only. It does not perform the job.
    //! 仅准备 A 和 B 参数的构造函数。它不执行计算任务。
    math_BracketMinimum(const Standard_Real A, const Standard_Real B);

    //! Given two initial values this class computes a
    //! bracketing triplet of abscissae Ax, Bx, Cx
    //! (such that Bx is between Ax and Cx, F(Bx) is
    //! less than both F(Bx) and F(Cx)) the Brent minimization is done
    //! on the function F.
    //!
    //! 给定两个初始值，此类计算一个横坐标的三元组 Ax, Bx, Cx，
    //! 该三元组将最小值括住（使得 Bx 在 Ax 和 Cx 之间，F(Bx) 小于 F(Ax) 和 F(Cx)），
    //! 并对函数 F 执行 Brent 最小化。
    Standard_EXPORT math_BracketMinimum(math_Function& F, const Standard_Real A, const Standard_Real B);


    //! Given two initial values this class computes a
    //! bracketing triplet of abscissae Ax, Bx, Cx
    //! (such that Bx is between Ax and Cx, F(Bx) is
    //! less than both F(Bx) and F(Cx)) the Brent minimization is done
    //! on the function F.
    //! This constructor has to be used if F(A) is known.
    //!
    //! 给定两个初始值，此类计算一个横坐标的三元组 Ax, Bx, Cx，
    //! 该三元组将最小值括住（使得 Bx 在 Ax 和 Cx 之间，F(Bx) 小于 F(Ax) 和 F(Cx)），
    //! 并对函数 F 执行 Brent 最小化。
    //! 如果 F(A) 已知，则必须使用此构造函数。
    Standard_EXPORT math_BracketMinimum(math_Function& F, const Standard_Real A, const Standard_Real B, const Standard_Real FA);


    //! Given two initial values this class computes a
    //! bracketing triplet of abscissae Ax, Bx, Cx
    //! (such that Bx is between Ax and Cx, F(Bx) is
    //! less than both F(Bx) and F(Cx)) the Brent minimization is done
    //! on the function F.
    //! This constructor has to be used if F(A) and F(B) are known.
    //!
    //! 给定两个初始值，此类计算一个横坐标的三元组 Ax, Bx, Cx，
    //! 该三元组将最小值括住（使得 Bx 在 Ax 和 Cx 之间，F(Bx) 小于 F(Ax) 和 F(Cx)），
    //! 并对函数 F 执行 Brent 最小化。
    //! 如果 F(A) 和 F(B) 已知，则必须使用此构造函数。
    Standard_EXPORT math_BracketMinimum(math_Function& F, const Standard_Real A, const Standard_Real B, const Standard_Real FA, const Standard_Real FB);

    //! Set limits of the parameter. By default no limits are applied to the parameter change.
    //! If no minimum is found in limits then IsDone() will return false. The user
    //! is in charge of providing A and B to be in limits.
    //!
    //! 设置参数的限制。默认情况下，参数的变化不受限制。
    //! 如果在限制范围内未找到最小值，则 IsDone() 将返回 false。用户
    //! 负责提供在限制范围内的 A 和 B。
    void SetLimits(const Standard_Real theLeft, const Standard_Real theRight);

    //! Set function value at A
    //! 设置 A 点的函数值
    void SetFA(const Standard_Real theValue);

    //! Set function value at B
    //! 设置 B 点的函数值
    void SetFB(const Standard_Real theValue);

    //! The method performing the job. It is called automatically by constructors with the function.
    //! 执行计算任务的方法。带函数的构造函数会自动调用此方法。
    Standard_EXPORT void Perform(math_Function& F);

    //! Returns true if the computations are successful, otherwise returns false.
    //! 如果计算成功，则返回 true，否则返回 false。
    Standard_Boolean IsDone() const;

    //! Returns the bracketed triplet of abscissae.
    //! Exceptions
    //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
    //!
    //! 返回包含最小值的横坐标三元组。
    //! 异常
    //! 如果算法失败 (IsDone 返回 false)，则引发 StdFail_NotDone。
    Standard_EXPORT void Values(Standard_Real& A, Standard_Real& B, Standard_Real& C) const;

    //! returns the bracketed triplet function values.
    //! Exceptions
    //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
    //!
    //! 返回包含最小值的函数值三元组。
    //! 异常
    //! 如果算法失败 (IsDone 返回 false)，则引发 StdFail_NotDone。
    Standard_EXPORT void FunctionValues(Standard_Real& FA, Standard_Real& FB, Standard_Real& FC) const;

    //! Prints on the stream o information on the current state
    //! of the object.
    //! Is used to redefine the operator <<.
    //!
    //! 在流 o 上打印有关对象当前状态的信息。
    //! 用于重定义操作符 <<。
    Standard_EXPORT void Dump(Standard_OStream& o) const;

private:

    //! Limit the given value to become within the range [myLeft, myRight].
    //! 将给定值限制在 [myLeft, myRight] 范围内。
    Standard_Real Limited(const Standard_Real theValue) const;

    //! Limit the value of C (see Limited) and compute the function in it.
    //! If C occurs to be between A and B then swap parameters and function
    //! values of B and C.
    //! Return false in the case of C becomes equal to B or function calculation
    //! failure.
    //!
    //! 限制 C 的值 (参见 Limited) 并在该点计算函数值。
    //! 如果 C 恰好在 A 和 B 之间，则交换 B 和 C 的参数及函数值。
    //! 如果 C 变得等于 B 或函数计算失败，则返回 false。
    Standard_Boolean LimitAndMayBeSwap(math_Function& F, const Standard_Real theA,
        Standard_Real& theB, Standard_Real& theFB,
        Standard_Real& theC, Standard_Real& theFC) const;

private:

    Standard_Boolean Done;
    Standard_Real Ax;
    Standard_Real Bx;
    Standard_Real Cx;
    Standard_Real FAx;
    Standard_Real FBx;
    Standard_Real FCx;
    Standard_Real myLeft;
    Standard_Real myRight;
    Standard_Boolean myIsLimited;
    Standard_Boolean myFA;
    Standard_Boolean myFB;


};

#include <math_BracketMinimum.lxx>

#endif // _math_BracketMinimum_HeaderFile
