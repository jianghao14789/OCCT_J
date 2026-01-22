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

//! @file
//! This file is intended to be the first file included to any
//! Open CASCADE source. It defines platform-specific pre-processor
//! macros necessary for correct compilation of Open CASCADE code.

#ifndef _Standard_Macro_HeaderFile
#define _Standard_Macro_HeaderFile

//! @def Standard_OVERRIDE
//! Should be used in declarations of virtual methods overridden in the
//! derived classes, to cause compilation error in the case if that virtual
//! function disappears or changes its signature in the base class.
//!
//! Expands to C++11 keyword "override" on compilers that are known to
//! suppot it; empty in other cases.
// Standard_OVERRIDE 宏：用于派生类中声明覆盖（重写）基类的虚函数。
// 如果基类中的虚函数被删除或签名改变，编译器会报错，从而增强代码安全性。
// 在支持 C++11 的编译器上，它展开为 "override" 关键字。
#if defined(__cplusplus) && (__cplusplus >= 201100L)
// part of C++11 standard
#define Standard_OVERRIDE override
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
// MSVC extension since VS2012
#define Standard_OVERRIDE override
#else
#define Standard_OVERRIDE
#endif

//! @def Standard_DELETE
//! Alias for C++11 keyword "=delete" marking methods to be deleted.
// Standard_DELETE 宏：用于禁用特定的成员函数（如禁用拷贝构造函数）。
// 在支持 C++11 的编译器上，展开为 "= delete"。
#if defined(__cplusplus) && (__cplusplus >= 201100L)
// part of C++11 standard
#define Standard_DELETE = delete
#elif defined(_MSC_VER) && (_MSC_VER >= 1800)
// implemented since VS2013
#define Standard_DELETE = delete
#else
#define Standard_DELETE
#endif

//! @def Standard_FALLTHROUGH
//! Should be used in a switch statement immediately before a case label,
//! if code associated with the previous case label may fall through to that
//! next label (i.e. does not end with "break" or "return" etc.).
//! This macro indicates that the fall through is intentional and should not be
//! diagnosed by a compiler that warns on fallthrough.
// Standard_FALLTHROUGH 宏：用于 switch 语句中，表示有意让代码从一个 case 穿透到下一个 case。
// 这可以防止编译器发出“意外穿透”的警告。
#if defined(__cplusplus) && (__cplusplus >= 201703L)
// part of C++17 standard
#define Standard_FALLTHROUGH [[fallthrough]];
#elif defined(__GNUC__) && (__GNUC__ >= 7)
// gcc 7+
#define Standard_FALLTHROUGH __attribute__((fallthrough));
#else
#define Standard_FALLTHROUGH
#endif

//! @def Standard_NODISCARD
//! This attribute may appear in a function declaration,
//! enumeration declaration or class declaration. It tells the compiler to
//! issue a warning, if a return value marked by that attribute is discarded.
// Standard_NODISCARD 宏：用于函数声明，告诉编译器如果调用者忽略了返回值，则发出警告。
// 对应 C++17 的 [[nodiscard]] 属性。
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(nodiscard)
#define Standard_NODISCARD [[nodiscard]]
#else
#define Standard_NODISCARD
#endif
#elif defined(__GNUC__) && !defined(INTEL_COMPILER)
// According to available documentation, GCC-style __attribute__ ((warn_unused_result))
// should be available in GCC since version 3.4, and in CLang since 3.9;
// Intel compiler does not seem to support this
#define Standard_NODISCARD __attribute__((warn_unused_result))
#else
#define Standard_NODISCARD
#endif

//! @def Standard_UNUSED
//! Macro for marking variables / functions as possibly unused
//! so that compiler will not emit redundant "unused" warnings.
// Standard_UNUSED 宏：用于标记可能未使用的变量或函数，防止编译器报“未使用”警告。
#if defined(__GNUC__) || defined(__clang__)
#define Standard_UNUSED __attribute__((unused))
#else
#define Standard_UNUSED
#endif

//! @def Standard_NOINLINE
//! Macro for disallowing function inlining.
// Standard_NOINLINE 宏：禁止编译器将函数进行内联展开。
#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)))
#define Standard_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define Standard_NOINLINE __declspec(noinline)
#else
#define Standard_NOINLINE
#endif

//! @def Standard_THREADLOCAL
//! Define Standard_THREADLOCAL modifier as C++11 thread_local keyword where it is available.
// Standard_THREADLOCAL 宏：定义线程局部变量。展开为 C++11 的 thread_local。
#if defined(__clang__)
// CLang version: standard CLang > 3.3 or XCode >= 8 (but excluding 32-bit ARM)
// Note: this has to be in separate #if to avoid failure of preprocessor on other platforms
#if __has_feature(cxx_thread_local)
#define Standard_THREADLOCAL thread_local
#endif
#elif defined(__INTEL_COMPILER)
#if (defined(_MSC_VER) && _MSC_VER >= 1900 && __INTEL_COMPILER > 1400)
// requires msvcrt vc14+ (Visual Studio 2015+)
#define Standard_THREADLOCAL thread_local
#elif (!defined(_MSC_VER) && __INTEL_COMPILER > 1500)
#define Standard_THREADLOCAL thread_local
#endif
#elif (defined(_MSC_VER) && _MSC_VER >= 1900)
// msvcrt coming with vc14+ (VS2015+)
#define Standard_THREADLOCAL thread_local
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
// GCC >= 4.8
#define Standard_THREADLOCAL thread_local
#endif

#ifndef Standard_THREADLOCAL
#define Standard_THREADLOCAL
#endif

//! @def Standard_DEPRECATED("message")
//! Can be used in declaration of a method or a class to mark it as deprecated.
//! Use of such method or class will cause compiler warning (if supported by
//! compiler and unless disabled).
//! If macro OCCT_NO_DEPRECATED is defined, Standard_DEPRECATED is defined empty.
// Standard_DEPRECATED 宏：用于标记过时的（不推荐使用的）函数或类。
// 当有人调用它们时，编译器会发出警告，提示该项已过时。
#ifdef OCCT_NO_DEPRECATED
#define Standard_DEPRECATED(theMsg)
#else
#if defined(_MSC_VER)
#define Standard_DEPRECATED(theMsg) __declspec(deprecated(theMsg))
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined(__clang__))
#define Standard_DEPRECATED(theMsg) __attribute__((deprecated(theMsg)))
#elif defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define Standard_DEPRECATED(theMsg) __attribute__((deprecated))
#else
#define Standard_DEPRECATED(theMsg)
#endif
#endif

//! @def Standard_DISABLE_DEPRECATION_WARNINGS
//! Disables warnings on use of deprecated features (see Standard_DEPRECATED),
//! from the current point till appearance of Standard_ENABLE_DEPRECATION_WARNINGS macro.
//! This is useful for sections of code kept for backward compatibility and scheduled for removal.
//!
//! @def Standard_ENABLE_DEPRECATION_WARNINGS
//! Enables warnings on use of deprecated features previously disabled by
//! Standard_DISABLE_DEPRECATION_WARNINGS.
// 下面两个宏用于在特定代码块中临时禁用或恢复“过时建议”的警告。
#if defined(__ICL) || defined(__INTEL_COMPILER)
#define Standard_DISABLE_DEPRECATION_WARNINGS __pragma(warning(push)) __pragma(warning(disable : 1478))
#define Standard_ENABLE_DEPRECATION_WARNINGS __pragma(warning(pop))
#elif (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))) || defined(__clang__)
// available since at least gcc 4.2 (maybe earlier), however only gcc 4.6+ supports this pragma inside the function body
// CLang also supports this gcc syntax (in addition to "clang diagnostic ignored")
#define Standard_DISABLE_DEPRECATION_WARNINGS _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define Standard_ENABLE_DEPRECATION_WARNINGS _Pragma("GCC diagnostic warning \"-Wdeprecated-declarations\"")
#elif defined(_MSC_VER)
#define Standard_DISABLE_DEPRECATION_WARNINGS __pragma(warning(push)) __pragma(warning(disable : 4996))
#define Standard_ENABLE_DEPRECATION_WARNINGS __pragma(warning(pop))
#else
#define Standard_DISABLE_DEPRECATION_WARNINGS
#define Standard_ENABLE_DEPRECATION_WARNINGS
#endif

//! @def OCCT_NO_RVALUE_REFERENCE
//! Disables methods and constructors that use rvalue references
//! (C++11 move semantics) not supported by obsolete compilers.
// OCCT_NO_RVALUE_REFERENCE：在旧版编译器（如 VS2008）上禁用右值引用支持。
#if (defined(_MSC_VER) && (_MSC_VER < 1600))
#define OCCT_NO_RVALUE_REFERENCE
#endif

#ifdef _WIN32

// We must be careful including windows.h: it is really poisonous stuff!
// The most annoying are #defines of many identifiers that you could use in
// normal code without knowing that Windows has its own knowledge of them...
// So lets protect ourselves by switching OFF as much as possible of this in advance.
// If someone needs more from windows.h, he is encouraged to #undef these symbols
// or include windows.h prior to any OCCT stuff.
// Note that we define each symbol to itself, so that it still can be used
// e.g. as name of variable, method etc.
// 在 Windows 上，包含 windows.h 可能会引入很多冲突的宏（如 min/max）。
// OCCT 预先定义了一些宏来限制 windows.h 的副作用。
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN /* exclude extra Windows stuff */
#endif
#ifndef NOMINMAX
#define NOMINMAX NOMINMAX /* avoid #define min() and max() */
#endif
#ifndef NOMSG
#define NOMSG NOMSG /* avoid #define SendMessage etc. */
#endif
#ifndef NODRAWTEXT
#define NODRAWTEXT NODRAWTEXT /* avoid #define DrawText etc. */
#endif
#ifndef NONLS
#define NONLS NONLS /* avoid #define CompareString etc. */
#endif
#ifndef NOGDI
#define NOGDI NOGDI /* avoid #define SetPrinter (winspool.h) etc. */
#endif
#ifndef NOSERVICE
#define NOSERVICE NOSERVICE
#endif
#ifndef NOKERNEL
#define NOKERNEL NOKERNEL
#endif
#ifndef NOUSER
#define NOUSER NOUSER
#endif
#ifndef NOMCX
#define NOMCX NOMCX
#endif
#ifndef NOIME
#define NOIME NOIME
#endif

#endif

//! @def Standard_EXPORT
//! This macro should be used in declarations of public methods
//! to ensure that they are exported from DLL on Windows and thus
//! can be called from other (dependent) libraries or applications.
//!
//! If macro OCCT_STATIC_BUILD is defined, then Standard_EXPORT
//! is set to empty.
// Standard_EXPORT 宏：非常重要！
// 在 Windows DLL 编译中，该宏展开为 __declspec(dllexport)，确保外部可以调用此函数。
// 如果是静态链接编译，它则为空。

#if defined(_WIN32) && !defined(OCCT_STATIC_BUILD) && !defined(HAVE_NO_DLL)

//======================================================
// Windows-specific definitions
// Windows 平台特定定义
//======================================================

#ifndef Standard_EXPORT
#define Standard_EXPORT __declspec(dllexport)
// For global variables :
// 用于全局变量：
#define Standard_EXPORTEXTERN __declspec(dllexport) extern
#define Standard_EXPORTEXTERNC extern "C" __declspec(dllexport)
#endif /* Standard_EXPORT */

#ifndef Standard_IMPORT
#define Standard_IMPORT __declspec(dllimport) extern
#define Standard_IMPORTC extern "C" __declspec(dllimport)
#endif /* Standard_IMPORT */

#else /* UNIX */

//======================================================
// UNIX / static library definitions
// UNIX 平台或静态库定义
//======================================================

#ifndef Standard_EXPORT
#define Standard_EXPORT
// For global variables :
#define Standard_EXPORTEXTERN extern
#define Standard_EXPORTEXTERNC extern "C"
#endif /* Standard_EXPORT */

#ifndef Standard_IMPORT
#define Standard_IMPORT extern
#define Standard_IMPORTC extern "C"
#endif /* Standard_IMPORT */

// Compatibility with old SUN compilers

// This preprocessor directive is a kludge to get around
// a bug in the Sun Workshop 5.0 compiler, it keeps the
// /usr/include/memory.h file from being #included
// with an incompatible extern "C" definition of memchr
// October 18, 2000  <rboehne@ricardo-us.com>
#if __SUNPRO_CC_COMPAT == 5
#define _MEMORY_H
#endif

#endif /* _WIN32 */

//! @def OCCT_UWP
//! This macro is defined on Windows platform in the case if the code
//! is being compiled for UWP (Universal Windows Platform).
#if defined(WINAPI_FAMILY) && WINAPI_FAMILY == WINAPI_FAMILY_APP
#define OCCT_UWP
#else
#ifdef OCCT_UWP
#undef OCCT_UWP
#endif
#endif

//! @def Standard_ATOMIC
//! Definition of Standard_ATOMIC for C++11 or visual studio that supports it.
//! Before usage there must be "atomic" included in the following way:
//! #ifdef Standard_HASATOMIC
//!   #include <atomic>
//! #endif
#if (defined(__cplusplus) && __cplusplus >= 201100L) || (defined(_MSC_VER) && _MSC_VER >= 1800) ||                     \
    (defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)))
#define Standard_HASATOMIC
#define Standard_ATOMIC(theType) std::atomic<theType>
#else
#define Standard_ATOMIC(theType) theType
#endif

#endif
