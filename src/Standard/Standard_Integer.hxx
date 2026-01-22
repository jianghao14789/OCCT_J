// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _Standard_Integer_HeaderFile
#define _Standard_Integer_HeaderFile

#include <Standard_Std.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_values.h>

// ===============
// Inline methods
// 内联方法 - 整数相关操作
// ===============

// ------------------------------------------------------------------
// Abs : Returns the absolute value of an Integer
// 返回整数的绝对值
// ------------------------------------------------------------------
inline Standard_Integer Abs(const Standard_Integer Value) {
    return Value >= 0 ? Value : -Value;
}

// ------------------------------------------------------------------
// IsEven : Returns Standard_True if an integer is even
// 如果整数是偶数返回 Standard_True
// ------------------------------------------------------------------
inline Standard_Boolean IsEven(const Standard_Integer Value) {
    return Value % 2 == 0;
}

// ------------------------------------------------------------------
// IsOdd : Returns Standard_True if an integer is odd
// 如果整数是奇数返回 Standard_True
// ------------------------------------------------------------------
inline Standard_Boolean IsOdd(const Standard_Integer Value) {
    return Value % 2 == 1;
}

// ------------------------------------------------------------------
// Max : Returns the maximum integer between two integers
// 返回两个整数中的最大值
// ------------------------------------------------------------------
inline Standard_Integer Max(const Standard_Integer Val1, const Standard_Integer Val2) {
    return Val1 >= Val2 ? Val1 : Val2;
}

// ------------------------------------------------------------------
// Min : Returns the minimum integer between two integers
// 返回两个整数中的最小值
// ------------------------------------------------------------------
inline Standard_Integer Min(const Standard_Integer Val1, const Standard_Integer Val2) {
    return Val1 <= Val2 ? Val1 : Val2;
}

// ------------------------------------------------------------------
// Modulus : Returns the remainder of division between two integers
// 返回两个整数相除的余数（取模）
// ------------------------------------------------------------------
inline Standard_Integer Modulus(const Standard_Integer Value, const Standard_Integer Divisor) {
    return Value % Divisor;
}

// ------------------------------------------------------------------
// Square : Returns the square of an integer
// 返回整数的平方
// ------------------------------------------------------------------
inline Standard_Integer Square(const Standard_Integer Value) {
    return Value * Value;
}

// ------------------------------------------------------------------
// IntegerFirst : Returns the minimum value of an integer
// 返回整数的最小值（通常为 -2147483648）
// ------------------------------------------------------------------
inline Standard_Integer IntegerFirst() {
    return INT_MIN;
}

// ------------------------------------------------------------------
// IntegerLast : Returns the maximum value of an integer
// 返回整数的最大值（通常为 2147483647）
// ------------------------------------------------------------------
inline Standard_Integer IntegerLast() {
    return INT_MAX;
}

// ------------------------------------------------------------------
// IntegerSize : Returns the size in digits of an integer
// 返回整数的大小（位数），通常为 32
// ------------------------------------------------------------------
inline Standard_Integer IntegerSize() {
    return BITS(Standard_Integer);
}

//! Computes a hash code for the given value of some integer type, in range [1, theUpperBound]
//! @tparam TheInteger the type of the integer which hash code is to be computed
//! @param theValue the value of the TheInteger type which hash code is to be computed
//! @param theMask the mask for the last bits of the value that are used in the computation of a hash code
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in range [1, theUpperBound]
//!
//! 为给定的某种整数类型计算哈希码，范围 [1, theUpperBound]
//! @tparam TheInteger 要计算哈希码的整数类型
//! @param theValue 要计算哈希码的 TheInteger 类型的值
//! @param theMask 用于计算哈希码的值的最后位的掩码
//! @param theUpperBound 哈希码范围的上界
//! @return 计算得到的哈希码，范围 [1, theUpperBound]
template <typename TheInteger>
typename opencascade::std::enable_if<opencascade::is_integer<TheInteger>::value, Standard_Integer>::type
IntegerHashCode(const TheInteger theValue, const typename opencascade::disable_deduction<TheInteger>::type theMask,
                const Standard_Integer theUpperBound) {
    return static_cast<Standard_Integer>((theValue & theMask) % theUpperBound + 1);
}

//! Computes a hash code for the given value of the Standard_Integer type, in range [1, theUpperBound]
//! @param theValue the value of the Standard_Integer type which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in range [1, theUpperBound]
//!
//! 为 Standard_Integer 类型的值计算哈希码，范围 [1, theUpperBound]
//! 用于在哈希表或集合中快速定位整数
//! @param theValue 要计算哈希码的 Standard_Integer 值
//! @param theUpperBound 哈希码范围的上界
//! @return 计算得到的哈希码，范围 [1, theUpperBound]
inline Standard_Integer HashCode(const Standard_Integer theValue, const Standard_Integer theUpperBound) {
    // return (Abs (theMe) % theUpper) + 1;
    return IntegerHashCode(theValue, IntegerLast(), theUpperBound);
}

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two integers are equal
// 如果两个整数相等返回 Standard_True
// ------------------------------------------------------------------
inline Standard_Boolean IsEqual(const Standard_Integer theOne, const Standard_Integer theTwo) {
    return theOne == theTwo;
}

//! Computes a hash value for the given unsigned integer, in range [1, theUpperBound]
//! @param theValue the unsigned integer which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a hash value computed for the given unsigned integer, in range [1, theUpperBound]
//!
//! 为无符号整数计算哈希值
//! @param theValue 要计算哈希码的无符号整数
//! @param theUpperBound 哈希码范围的上界
//! @return 计算得到的哈希值
inline Standard_Integer HashCode(const unsigned int theValue, const Standard_Integer theUpperBound) {
    return ::HashCode(static_cast<Standard_Integer>(theValue), theUpperBound);
}

//! Computes a hash code for the given value of the "long long int" type, in range [1, theUpperBound]
//! @param theValue the value of the "long long int" type which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in range [1, theUpperBound]
//!
//! 为"long long int"类型的值计算哈希码
//! @param theValue 要计算哈希码的 long long int 值
//! @param theUpperBound 哈希码范围的上界
//! @return 计算得到的哈希码
inline Standard_Integer HashCode(const long long int theValue, const Standard_Integer theUpperBound) {
    return IntegerHashCode(theValue, 0x7fffffffffffffff, theUpperBound);
}

#if (defined(_LP64) || defined(__LP64__) || defined(_WIN64)) || defined(__APPLE__)

//! Computes a hash code for the given value of the Standard_Utf32Char type, in the range [1, theUpperBound]
//! @tparam TheUtf32Char the type of the given value (it is Standard_Utf32Char,
//! and must not be the same as "unsigned int", because the overload of the HashCode function
//! for "unsigned int" type is already presented in Standard_Integer.hxx)
//! @param theValue the value of the Standard_Utf32Char type which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
//!
//! 为 Standard_Utf32Char 类型的值计算哈希码
//! （仅在 64 位系统上定义，避免与 unsigned int 的重载冲突）
inline Standard_Integer HashCode(const Standard_Utf32Char theValue, const Standard_Integer theUpperBound) {
    return IntegerHashCode(theValue, IntegerLast(), theUpperBound);
}

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two integers are equal
// 如果两个 UTF32 字符相等返回 Standard_True
// ------------------------------------------------------------------
inline Standard_Boolean IsEqual(const Standard_Utf32Char theOne, const Standard_Utf32Char theTwo) {
    return theOne == theTwo;
}

#endif

#endif
