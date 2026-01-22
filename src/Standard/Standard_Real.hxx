// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Standard_Real_HeaderFile
#define _Standard_Real_HeaderFile

#include <cmath>
#include <float.h>
#include <Standard_values.h>
#include <Standard_math.hxx>
#include <Standard_TypeDef.hxx>

// ===============================================
// Methods from Standard_Entity class which are redefined:
//    - Hascode - 哈希码计算
//    - IsEqual - 相等性比较
// ===============================================

// ==================================
// Methods implemented in Standard_Real.cxx
// 在 Standard_Real.cxx 中实现的方法
// ==================================

//! Computes a hash code for the given real, in the range [1, theUpperBound]
//! @param theReal the real value which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
//! 为给定的实数计算哈希码，范围在 [1, theUpperBound] 内
//! 用于在哈希表或集合中快速查找浮点数
Standard_EXPORT Standard_Integer HashCode(Standard_Real theReal, Standard_Integer theUpperBound);

//! 反余弦函数
Standard_EXPORT Standard_Real ACos(const Standard_Real);
//! 反余弦近似函数（性能优化）
Standard_EXPORT Standard_Real ACosApprox(const Standard_Real);
//! 反正弦函数
Standard_EXPORT Standard_Real ASin(const Standard_Real);
//! 两个参数的反正切函数（考虑象限）
Standard_EXPORT Standard_Real ATan2(const Standard_Real, const Standard_Real);
//! 返回更接近 y 的下一个浮点数（用于数值计算精度）
Standard_EXPORT Standard_Real NextAfter(const Standard_Real, const Standard_Real);

//! Returns |a| if b >= 0; -|a| if b < 0.
//! 如果 b >= 0，返回 |a|；如果 b < 0，返回 -|a|
//! 用于将 a 的绝对值与 b 的符号组合
Standard_EXPORT Standard_Real Sign(const Standard_Real a, const Standard_Real b);

//! 反双曲正切函数
Standard_EXPORT Standard_Real ATanh(const Standard_Real);
//! 反双曲余弦函数
Standard_EXPORT Standard_Real ACosh(const Standard_Real);
//! 双曲正弦函数
Standard_EXPORT Standard_Real Sinh(const Standard_Real);
//! 双曲余弦函数
Standard_EXPORT Standard_Real Cosh(const Standard_Real);
//! 自然对数函数
Standard_EXPORT Standard_Real Log(const Standard_Real);
//! 平方根函数
Standard_EXPORT Standard_Real Sqrt(const Standard_Real);

//-------------------------------------------------------------------
// RealSmall : Returns the smallest positive real
// 返回最小正实数（浮点数的最小正值）
//-------------------------------------------------------------------
inline Standard_Real RealSmall() {
    return DBL_MIN;
}

//-------------------------------------------------------------------
// Abs : Returns the absolute value of a real
// 返回实数的绝对值
//-------------------------------------------------------------------
inline Standard_Real Abs(const Standard_Real Value) {
    return fabs(Value);
}

//-------------------------------------------------------------------
// IsEqual : Returns Standard_True if two reals are equal
// 如果两个实数相等则返回 Standard_True
// 考虑浮点精度，使用 RealSmall() 作为误差界限
//-------------------------------------------------------------------
inline Standard_Boolean IsEqual(const Standard_Real Value1, const Standard_Real Value2) {
    return Abs((Value1 - Value2)) < RealSmall();
}

//  *********************************** //
//       Class methods                  //
//       类方法 - 机器相关值           //
//                                      //
//  Machine-dependent values            //
//  Should be taken from include file   //
//  应从 float.h 中获取的包含文件      //
//  *********************************** //

//-------------------------------------------------------------------
// RealDigit : Returns the number of digits of precision in a real
// 返回实数精度的位数（通常为 15 位十进制数字）
//-------------------------------------------------------------------
inline Standard_Integer RealDigits() {
    return DBL_DIG;
}

//-------------------------------------------------------------------
// RealEpsilon : Returns the minimum positive real such that
//               1.0 + x is not equal to 1.0
// 返回最小正实数使得 1.0 + x != 1.0
// 这是机器精度的度量，通常约为 2.22e-16
//-------------------------------------------------------------------
inline Standard_Real RealEpsilon() {
    return DBL_EPSILON;
}

//-------------------------------------------------------------------
// RealFirst : Returns the minimum negative value of a real
// 返回实数的最小负值（例如 -1.79e+308）
//-------------------------------------------------------------------
inline Standard_Real RealFirst() {
    return -DBL_MAX;
}

//-------------------------------------------------------------------
// RealFirst10Exp : Returns the minimum value of exponent(base 10) of
//                  a real.
// 返回实数 10 进制指数的最小值（通常为 -307）
//-------------------------------------------------------------------
inline Standard_Integer RealFirst10Exp() {
    return DBL_MIN_10_EXP;
}

//-------------------------------------------------------------------
// RealLast : Returns the maximum value of a real
// 返回实数的最大值（例如 1.79e+308）
//-------------------------------------------------------------------
inline Standard_Real RealLast() {
    return DBL_MAX;
}

//-------------------------------------------------------------------
// RealLast10Exp : Returns the maximum value of exponent(base 10) of
//                 a real.
// 返回实数 10 进制指数的最大值（通常为 308）
//-------------------------------------------------------------------
inline Standard_Integer RealLast10Exp() {
    return DBL_MAX_10_EXP;
}

//-------------------------------------------------------------------
// RealMantissa : Returns the size in bits of the matissa part of a
//                real.
// 返回实数尾数部分的位数（通常为 53 位用于 double）
//-------------------------------------------------------------------
inline Standard_Integer RealMantissa() {
    return DBL_MANT_DIG;
}

//-------------------------------------------------------------------
// RealRadix : Returns the radix of exponent representation
// 返回指数表示的基数（通常为 2，表示二进制）
//-------------------------------------------------------------------
inline Standard_Integer RealRadix() {
    return FLT_RADIX;
}

//-------------------------------------------------------------------
// RealSize : Returns the size in bits of an integer
// 返回 double 的大小（位数），通常为 64
//-------------------------------------------------------------------
inline Standard_Integer RealSize() {
    return BITS(Standard_Real);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//   End of machine-dependent values   //
//   机器相关值结束                   //
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

//-------------------------------------------------------------------
// IntToReal : Converts an integer in a real
// 将整数转换为实数
//-------------------------------------------------------------------
inline Standard_Real IntToReal(const Standard_Integer Value) {
    return Value;
}

//-------------------------------------------------------------------
// ATan : Returns the value of the arc tangent of a real
// 返回实数的反正切值
//-------------------------------------------------------------------
inline Standard_Real ATan(const Standard_Real Value) {
    return atan(Value);
}

//-------------------------------------------------------------------
// Ceiling : Returns the smallest integer not less than a real
// 返回不小于实数的最小整数（向上取整）
//-------------------------------------------------------------------
inline Standard_Real Ceiling(const Standard_Real Value) {
    return ceil(Value);
}

//-------------------------------------------------------------------
// Cos : Returns the cosine of a real
// 返回实数的余弦值
//-------------------------------------------------------------------
inline Standard_Real Cos(const Standard_Real Value) {
    return cos(Value);
}

//-------------------------------------------------------------------
// Epsilon : The function returns absolute value of difference
//           between 'Value' and other nearest value of
//           Standard_Real type.
//           Nearest value is chosen in direction of infinity
//           the same sign as 'Value'.
//           If 'Value' is 0 then returns minimal positive value
//           of Standard_Real type.
// Epsilon：返回 'Value' 与最近的另一个 Standard_Real 值之间的差的绝对值
// 最近值在与 'Value' 同号的无穷方向上选择
// 如果 'Value' 是 0，则返回 Standard_Real 的最小正值
// 用于确定特定值的机器精度
//-------------------------------------------------------------------
inline Standard_Real Epsilon(const Standard_Real Value) {
    Standard_Real aEpsilon;

    if (Value >= 0.0) {
        aEpsilon = NextAfter(Value, RealLast()) - Value;
    } else {
        aEpsilon = Value - NextAfter(Value, RealFirst());
    }
    return aEpsilon;
}

//-------------------------------------------------------------------
// Exp : Returns the exponential function of a real
// 返回实数的指数函数（e^Value）
//-------------------------------------------------------------------
inline Standard_Real Exp(const Standard_Real Value) {
    return exp(Value);
}

//-------------------------------------------------------------------
// Floor : Return the largest integer not greater than a real
// 返回不大于实数的最大整数（向下取整）
//-------------------------------------------------------------------
inline Standard_Real Floor(const Standard_Real Value) {
    return floor(Value);
}

//-------------------------------------------------------------------
// IntegerPart : Returns the integer part of a real
// 返回实数的整数部分
// 正数使用 floor，负数使用 ceil（保留符号）
//-------------------------------------------------------------------
inline Standard_Real IntegerPart(const Standard_Real Value) {
    return ((Value > 0) ? floor(Value) : ceil(Value));
}

//-------------------------------------------------------------------
// Log10 : Returns the base-10 logarithm of a real
// 返回实数的 10 进制对数
//-------------------------------------------------------------------
inline Standard_Real Log10(const Standard_Real Value) {
    return log10(Value);
}

//-------------------------------------------------------------------
// Max : Returns the maximum value of two reals
// 返回两个实数中的最大值
//-------------------------------------------------------------------
inline Standard_Real Max(const Standard_Real Val1, const Standard_Real Val2) {
    return Val1 >= Val2 ? Val1 : Val2;
}

//-------------------------------------------------------------------
// Min : Returns the minimum value of two reals
// 返回两个实数中的最小值
//-------------------------------------------------------------------
inline Standard_Real Min(const Standard_Real Val1, const Standard_Real Val2) {
    return Val1 <= Val2 ? Val1 : Val2;
}

//-------------------------------------------------------------------
// Pow : Returns a real to a given power
// 返回实数的幂（Value^P）
//-------------------------------------------------------------------
inline Standard_Real Pow(const Standard_Real Value, const Standard_Real P) {
    return pow(Value, P);
}

//-------------------------------------------------------------------
// RealPart : Returns the fractional part of a real.
// 返回实数的小数部分（例如 3.14 的小数部分是 0.14）
//-------------------------------------------------------------------
inline Standard_Real RealPart(const Standard_Real Value) {
    return fabs(IntegerPart(Value) - Value);
}

//-------------------------------------------------------------------
// RealToInt : Returns the real converted to nearest valid integer.
//             If input value is out of valid range for integers,
//             minimal or maximal possible integer is returned.
// RealToInt：将实数转换为最接近的有效整数
// 如果输入值超出整数的有效范围，返回最小或最大可能的整数
// 这个函数避免了浮点数到整数转换时的溢出异常
//-------------------------------------------------------------------
inline Standard_Integer RealToInt(const Standard_Real Value) {
    // Note that on WNT under MS VC++ 8.0 conversion of double value less
    // than INT_MIN or greater than INT_MAX to integer will cause signal
    // "Floating point multiple trap" (OCC17861)
    // 注意：在 WNT 上使用 MS VC++ 8.0，将小于 INT_MIN 或大于 INT_MAX 的 double
    // 值转换为整数会导致"浮点异常"信号，所以需要边界检查
    return Value < INT_MIN ? INT_MIN : Value > INT_MAX ? INT_MAX : (Standard_Integer)Value;
}

// =======================================================================
// function : RealToShortReal
// purpose  : Converts Standard_Real value to the nearest valid
//            Standard_ShortReal. If input value is out of valid range
//            for Standard_ShortReal, minimal or maximal
//            Standard_ShortReal is returned.
// 将 Standard_Real 值转换为最接近的有效 Standard_ShortReal
// 如果输入值超出 Standard_ShortReal 的有效范围，返回最小或最大 Standard_ShortReal
// 用于从双精度浮点数安全地转换为单精度浮点数
// =======================================================================
inline Standard_ShortReal RealToShortReal(const Standard_Real theVal) {
    return theVal < -FLT_MAX ? -FLT_MAX : theVal > FLT_MAX ? FLT_MAX : (Standard_ShortReal)theVal;
}

//-------------------------------------------------------------------
// Round : Returns the nearest integer of a real
// 返回实数最接近的整数（四舍五入）
// 对于 x.5，向远离零的方向舍入
//-------------------------------------------------------------------
inline Standard_Real Round(const Standard_Real Value) {
    return IntegerPart(Value + (Value > 0 ? 0.5 : -0.5));
}

//-------------------------------------------------------------------
// Sin : Returns the sine of a real
// 返回实数的正弦值
//-------------------------------------------------------------------
inline Standard_Real Sin(const Standard_Real Value) {
    return sin(Value);
}

//-------------------------------------------------------------------
// ASinh : Returns the hyperbolic arc sine of a real
// 返回实数的反双曲正弦值
//-------------------------------------------------------------------
inline Standard_Real ASinh(const Standard_Real Value)
#if defined(__QNX__)
{
    return std::asinh(Value);
}
#else
{
    return asinh(Value);
}
#endif

//-------------------------------------------------------------------
// Square : Returns a real to the power 2
// 返回实数的平方（Value * Value）
// 比使用 Pow(Value, 2) 更高效
//-------------------------------------------------------------------
inline Standard_Real Square(const Standard_Real Value) {
    return Value * Value;
}

//-------------------------------------------------------------------
// Tan : Returns the tangent of a real
// 返回实数的正切值
//-------------------------------------------------------------------
inline Standard_Real Tan(const Standard_Real Value) {
    return tan(Value);
}

//-------------------------------------------------------------------
// Tanh : Returns the hyperbolic tangent of a real
// 返回实数的双曲正切值
//-------------------------------------------------------------------
inline Standard_Real Tanh(const Standard_Real Value) {
    return tanh(Value);
}

#endif
