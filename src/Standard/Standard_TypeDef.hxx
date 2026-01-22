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

#ifndef _Standard_TypeDef_HeaderFile
#define _Standard_TypeDef_HeaderFile

#include <cstddef>
#include <ctime>

// VC9 does not have stdint.h
#if (defined(_MSC_VER) && (_MSC_VER < 1600))
// old MSVC - hasn't stdint header
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
#else
#include <stdint.h>
#endif

#if (defined(_MSC_VER) && (_MSC_VER < 1800))
// only Visual Studio 2013 (vc12) provides <cinttypes> header
// we do not defined all macros here - only used by OCCT framework
#define PRIx64 "I64x"
#define PRIX64 "I64X"
#define PRId64 "I64d"
#define PRIu64 "I64u"
#define SCNd64 "I64d"
#define SCNu64 "I64u"
#ifdef _WIN64
#define PRIxPTR "I64x"
#define PRIXPTR "I64X"
#define PRIdPTR "I64d"
#define PRIuPTR "I64u"
#define SCNdPTR "I64d"
#define SCNuPTR "I64u"
#else
#define PRIxPTR "Ix"
#define PRIXPTR "IX"
#define PRIdPTR "d"
#define PRIuPTR "u"
#define SCNdPTR "d"
#define SCNuPTR "u"
#endif
#else
// should be just <cinttypes> since C++11
// however we use this code for compatibility with old C99 compilers
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#endif

#define Standard_False false
#define Standard_True true

#include <Standard_Macro.hxx>

typedef int Standard_Integer;
// Standard_Integer 是 OCCT 中定义的标准整数类型，对应 C++ 的 int 类型。
// 在 CAD 编程中，它通常用于循环计数、数组索引等。

typedef double Standard_Real;
// Standard_Real 是 OCCT 中的标准浮点数类型，对应 C++ 的 double（双精度浮点数）。
// 在 CAD 系统中，由于对几何精度要求很高，绝大多数坐标和几何计算都使用双精度浮点数。

typedef bool Standard_Boolean;
// Standard_Boolean 是标准布尔类型，对应 C++ 的 bool。
// 用于表示真（True）或假（False）的逻辑值。

typedef float Standard_ShortReal;
// Standard_ShortReal 是短浮点数类型，对应 C++ 的 float（单精度浮点数）。
// 通常用于对精度要求不高但需要节省内存的场合，如某些可视化数据。

typedef char Standard_Character;
// Standard_Character 是标准字符类型，对应 C++ 的 char。

typedef unsigned char Standard_Byte;
// Standard_Byte 是标准字节类型，对应 unsigned char。
// 常用于处理二进制数据流。

typedef void* Standard_Address;
// Standard_Address 是通用地址类型，对应 void* 指针。

typedef size_t Standard_Size;
// Standard_Size 是用于表示对象大小或内存偏移的类型，对应 C++ 的 size_t。

typedef std::time_t Standard_Time;
// Standard_Time 是用于处理时间的类型，对应标准库的 time_t。

// Unicode primitives, char16_t, char32_t
typedef char Standard_Utf8Char;           //!< signed   UTF-8 char
// 有符号 UTF-8 字符类型。

typedef unsigned char Standard_Utf8UChar; //!< unsigned UTF-8 char
// 无符号 UTF-8 字符类型。

#if ((defined(__GNUC__) && !defined(__clang__) && ((__GNUC__ == 4 && __GNUC_MINOR__ <= 3) || __GNUC__ < 4)) ||         \
     (defined(_MSC_VER) && (_MSC_VER < 1600)))
// compatibility with old GCC and MSVC compilers
typedef uint16_t Standard_ExtCharacter;
typedef uint16_t Standard_Utf16Char;
typedef uint32_t Standard_Utf32Char;
#else
typedef char16_t Standard_ExtCharacter;
// 扩展字符类型，通常用于 Unicode 支持。

typedef char16_t Standard_Utf16Char; //!< UTF-16 char (always unsigned)
// UTF-16 编码的字符。

typedef char32_t Standard_Utf32Char; //!< UTF-32 char (always unsigned)
// UTF-32 编码的字符。
#endif
typedef wchar_t Standard_WideChar; //!< wide char (unsigned UTF-16 on Windows platform and signed UTF-32 on Linux)
// 宽字符类型，其大小和编码取决于操作系统（Windows 上通常是 16 位，Linux 上通常是 32 位）。

//
typedef const Standard_Character* Standard_CString;
// Standard_CString 是指向常量字符的指针，即 C 风格的字符串（如 "Hello"）。

typedef const Standard_ExtCharacter* Standard_ExtString;
// Standard_ExtString 是指向常量扩展字符的指针，即宽字符串。

#endif // _Standard_TypeDef_HeaderFile
