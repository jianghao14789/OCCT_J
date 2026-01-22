// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Standard_Handle_HeaderFile
#define _Standard_Handle_HeaderFile

#include <Standard_Address.hxx>
#include <Standard_Std.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Transient.hxx>

class Standard_Transient;

//! Namespace opencascade is intended for low-level template classes and functions
// opencascade 命名空间用于存放底层的模板类和函数
namespace opencascade {

//! Intrusive smart pointer for use with Standard_Transient class and its descendants.
//!
//! This class is similar to boost::intrusive_ptr<>. The reference counter
//! is part of the base class (Standard_Transient), thus creation of a handle
//! does not require allocation of additional memory for the counter.
//! All handles to the same object share the common counter; object is deleted
//! when the last handle pointing on it is destroyed. It is safe to create a new
//! handle from plain C pointer to the object already pointed by another handle.
//! The same object can be referenced by handles of different types (as soon as
//! they are compatible with the object type).
// 侵入式智能指针，用于 Standard_Transient 类及其派生类。
//
// 该类类似于 boost::intrusive_ptr<>。引用计数器是基类 (Standard_Transient) 的一部分，
// 因此创建 Handle 不需要为计数器分配额外的内存。
// 指向同一个对象的所有 Handle 共享同一个计数器；当最后一个指向该对象的 Handle 被销毁时，对象将被删除。
// 从已经由另一个 Handle 指向的对象的普通 C 指针创建一个新 Handle 是安全的。
// 同一个对象可以被不同类型的 Handle 引用（只要它们与对象类型兼容）。

template <class T> class handle {
public:
    //! STL-compliant typedef of contained type
    // 与 STL 兼容的包含类型定义
    typedef T element_type;

public:
    //! Empty constructor
    // 默认构造函数，初始化为空指针
    handle() : entity(0) {}

    //! Constructor from pointer to new object
    // 通过对象指针构造 Handle。会自动增加对象的引用计数。
    handle(const T* thePtr) : entity(const_cast<T*>(thePtr)) {
        BeginScope();
    }

    //! Copy constructor
    // 拷贝构造函数。新 Handle 指向相同对象，引用计数加 1。
    handle(const handle& theHandle) : entity(theHandle.entity) {
        BeginScope();
    }

#ifndef OCCT_NO_RVALUE_REFERENCE
    //! Move constructor
    // 移动构造函数。接管原 Handle 的对象，引用计数保持不变。
    handle(handle&& theHandle) : entity(theHandle.entity) {
        theHandle.entity = 0;
    }
#endif

    //! Destructor
    // 析构函数。减少引用计数，如果计数降为 0 则删除对象。
    ~handle() {
        EndScope();
    }

    //! Nullify the handle
    // 将 Handle 置为空，并减少原对象的引用计数。
    void Nullify() {
        EndScope();
    }

    //! Check for being null
    // 检查 Handle 是否为空。
    bool IsNull() const {
        return entity == 0;
    }

    //! Reset by new pointer
    // 重置 Handle 以指向新指针。
    void reset(T* thePtr) {
        Assign(thePtr);
    }

    //! Assignment operator
    // 赋值运算符。
    handle& operator=(const handle& theHandle) {
        Assign(theHandle.entity);
        return *this;
    }

    //! Assignment to pointer
    // 赋值为新指针。
    handle& operator=(const T* thePtr) {
        Assign(const_cast<T*>(thePtr));
        return *this;
    }

#ifndef OCCT_NO_RVALUE_REFERENCE
    //! Move operator
    // 移动赋值运算符。
    handle& operator=(handle&& theHandle) {
        std::swap(this->entity, theHandle.entity);
        return *this;
    }
#endif

    //! STL-like cast to pointer to referred object (note non-const).
    //! @sa std::shared_ptr::get()
    // 获取底层对象指针。
    T* get() const {
        return static_cast<T*>(this->entity);
    }

    //! Member access operator (note non-const)
    // 成员访问运算符 ->，使得 Handle 可以像指针一样使用。
    T* operator->() const {
        return static_cast<T*>(this->entity);
    }

    //! Dereferencing operator (note non-const)
    // 解引用运算符 *。
    T& operator*() const {
        return *get();
    }

    //! Check for equality
    // 相等性检查。
    template <class T2> bool operator==(const handle<T2>& theHandle) const {
        return get() == theHandle.get();
    }

    //! Check for equality
    template <class T2> bool operator==(const T2* thePtr) const {
        return get() == thePtr;
    }

    //! Check for equality
    template <class T2> friend bool operator==(const T2* left, const handle& right) {
        return left == right.get();
    }

    //! Check for inequality
    // 不等性检查。
    template <class T2> bool operator!=(const handle<T2>& theHandle) const {
        return get() != theHandle.get();
    }

    //! Check for inequality
    template <class T2> bool operator!=(const T2* thePtr) const {
        return get() != thePtr;
    }

    //! Check for inequality
    template <class T2> friend bool operator!=(const T2* left, const handle& right) {
        return left != right.get();
    }

    //! Compare operator for possible use in std::map<> etc.
    // 比较运算符，以便在 std::map 等容器中使用。
    template <class T2> bool operator<(const handle<T2>& theHandle) const {
        return get() < theHandle.get();
    }

    //! Down casting operator from handle to base type
    // 向下转换函数。用于将基类 Handle 转换为派生类 Handle。
    // 使用 C++ 的 dynamic_cast 确保转换的安全性。
    template <class T2>
    static typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value, handle>::type
    DownCast(const handle<T2>& theObject) {
        return handle(dynamic_cast<T*>(const_cast<T2*>(theObject.get())));
    }

    //! Down casting operator from pointer to base type
    template <class T2>
    static typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value, handle>::type
    DownCast(const T2* thePtr) {
        return handle(dynamic_cast<T*>(const_cast<T2*>(thePtr)));
    }

    //! For compatibility, define down casting operator from non-base type, as deprecated
    template <class T2>
    Standard_DEPRECATED("down-casting from object of the same or unrelated type is meaningless") static handle
        DownCast(const handle<T2>& theObject,
                 typename opencascade::std::enable_if<!is_base_but_not_same<T2, T>::value, void*>::type = 0) {
        return handle(dynamic_cast<T*>(const_cast<T2*>(theObject.get())));
    }

    //! For compatibility, define down casting operator from non-base type, as deprecated
    template <class T2>
    Standard_DEPRECATED("down-casting from object of the same or unrelated type is meaningless") static handle
        DownCast(const T2* thePtr,
                 typename opencascade::std::enable_if<!is_base_but_not_same<T2, T>::value, void*>::type = 0) {
        return handle(dynamic_cast<T*>(const_cast<T2*>(thePtr)));
    }

#if (defined(__clang__)) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1300) ||                                 \
    (defined(_MSC_VER) && _MSC_VER >= 1800) ||                                                                         \
    (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))

    //! Conversion to bool for use in conditional expressions
    // 显式转换为布尔值，用于条件判断（如 if (aHandle) ...）。
    explicit operator bool() const {
        return entity != nullptr;
    }

#else /* fallback version for compilers not supporting explicit conversion operators (VC10, VC11, GCC below 4.5) */

    //! Conversion to bool-compatible type for use in conditional expressions
    operator Standard_Transient* handle::*() const {
        return entity ? &handle::entity : 0;
    }

#endif

    // Support of conversions to handle of base type:
    // - copy and move constructors and assignment operators if OCCT_HANDLE_NOCAST is defined
    // - operators of upcast to const reference to base type otherwise
#if (defined(__clang__)) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1206) ||                                 \
    (defined(_MSC_VER) && _MSC_VER >= 1800) ||                                                                         \
    (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))

#ifdef OCCT_HANDLE_NOCAST

    //! Generalized copy constructor.
    //! Constructs handle holding entity of base type (T) from the one which holds entity of derived type (T2).
    // 通用拷贝构造函数。允许从派生类 Handle 构造基类 Handle。
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
    handle(const handle<T2>& theHandle) : entity(theHandle.entity) {
        BeginScope();
    }

    //! Generalized move constructor
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
    handle(handle<T2>&& theHandle) : entity(theHandle.entity) {
        theHandle.entity = 0;
    }

    //! Generalized assignment operator
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
    handle operator=(const handle<T2>& theHandle) {
        Assign(theHandle.entity);
        return *this;
    }

    //! Generalized move operator
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
    handle& operator=(handle<T2>&& theHandle) {
        std::swap(this->entity, theHandle.entity);
        return *this;
    }

#else

    //! Upcast to const reference to base type.
    // 向上转换：转换为基类 Handle 的常量引用。
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T2, T>::value>::type>
    operator const handle<T2>&() const {
        return reinterpret_cast<const handle<T2>&>(*this);
    }

    //! Upcast to non-const reference to base type.
    //! NB: this cast can be dangerous, but required for legacy code; see #26377
    // 向上转换：转换为基类 Handle 的引用。
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T2, T>::value>::type>
    operator handle<T2>&() {
        return reinterpret_cast<handle<T2>&>(*this);
    }

#endif /* OCCT_HANDLE_NOCAST */

#else /* fallback version for compilers not supporting default arguments of function templates (VC10, VC11, GCC        \
         below 4.3) */

#ifdef OCCT_HANDLE_NOCAST

    //! Generalized copy constructor.
    //! Constructs handle holding entity of base type (T) from the one which holds entity of derived type (T2).
    template <class T2>
    handle(const handle<T2>& theHandle, typename std::enable_if<is_base_but_not_same<T, T2>::value>::type* = nullptr)
        : entity(theHandle.entity) {
        BeginScope();
    }

#ifndef OCCT_NO_RVALUE_REFERENCE
    //! Generalized move constructor
    template <class T2>
    handle(handle<T2>&& theHandle, typename std::enable_if<is_base_but_not_same<T, T2>::value>::type* = nullptr)
        : entity(theHandle.entity) {
        theHandle.entity = 0;
    }
#endif

    //! Generalized assignment operator.
    template <class T2> handle operator=(const handle<T2>& theHandle) {
        std::enable_if<is_base_but_not_same<T, T2>::value, void*>::type aTypeCheckHelperVar;
        (void)aTypeCheckHelperVar;
        Assign(theHandle.entity);
        return *this;
    }

#ifndef OCCT_NO_RVALUE_REFERENCE
    //! Generalized move operator
    template <class T2> handle& operator=(handle<T2>&& theHandle) {
        std::enable_if<is_base_but_not_same<T, T2>::value, void*>::type aTypeCheckHelperVar;
        (void)aTypeCheckHelperVar;
        std::swap(this->entity, theHandle.entity);
        return *this;
    }
#endif

#else

    //! Upcast to const reference to base type.
    //! NB: this implementation will cause ambiguity errors on calls to overloaded
    //! functions accepting handles to different types, since compatibility is
    //! checked in the cast code rather than ensured by SFINAE (possible with C++11)
    template <class T2> operator const handle<T2>&() const {
        // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
        // (handle is being cast to const& to handle of non-base type)
        return reinterpret_cast<
            typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value, const handle<T2>&>::type>(*this);
    }

    //! Upcast to non-const reference to base type.
    //! NB: this cast can be dangerous, but required for legacy code; see #26377
    template <class T2>
    Standard_DEPRECATED(
        "Passing non-const reference to handle of base type in function is unsafe; use variable of exact type")
    operator handle<T2>&() {
        // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
        // (handle is being cast to const& to handle of non-base type)
        return reinterpret_cast<
            typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value, handle<T2>&>::type>(*this);
    }

#endif /* OCCT_HANDLE_NOCAST */

#endif /* compiler switch */

private:
    //! Assignment
    // 内部赋值函数。
    void Assign(Standard_Transient* thePtr) {
        if (thePtr == entity) return;
        EndScope();
        entity = thePtr;
        BeginScope();
    }

    //! Increment reference counter of referred object
    // 增加所引用对象的引用计数。
    void BeginScope() {
        if (entity != 0) entity->IncrementRefCounter();
    }

    //! Decrement reference counter and if 0, destroy referred object
    // 减少引用计数，如果为 0，则销毁所引用的对象。
    void EndScope() {
        if (entity != 0 && entity->DecrementRefCounter() == 0) entity->Delete();
        entity = 0;
    }

    template <class T2> friend class handle;

private:
    Standard_Transient* entity;
};

} // namespace opencascade

//! Define Handle() macro
// 定义 Handle 宏，方便用户声明智能指针。
// 例如 Handle(Standard_Transient) 等价于 opencascade::handle<Standard_Transient>
#define Handle(Class) opencascade::handle<Class>

//! Computes a hash code for the standard handle, in the range [1, theUpperBound]
//! @param theHandle the handle which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
template <class TheTransientType>
Standard_Integer HashCode(const Handle(TheTransientType) & theHandle, const Standard_Integer theUpperBound) {
    return ::HashCode(theHandle.get(), theUpperBound);
}

//! For compatibility with previous versions of OCCT, define Handle_Class alias for opencascade::handle<Class>.
#if (defined(_MSC_VER) && _MSC_VER >= 1800)
//! For Visual Studio 2013+, define Handle_Class as non-template class to allow exporting this type in C++/CLI.
#define DEFINE_STANDARD_HANDLECLASS(C1, C2, BC)                                                                        \
    class C1;                                                                                                          \
    class Handle_##C1 : public Handle(C1){                                                                             \
        public : Handle_##C1(){} Handle_##C1(Handle(C1) && theHandle) : Handle(C1)(                                    \
            theHandle){} template <class T2, typename = typename std::enable_if<std::is_base_of<C1, T2>::value>::type> \
        inline Handle_##C1(const opencascade::handle<T2>& theOther) : Handle(C1)(                                      \
            theOther){} template <class T2, typename = typename std::enable_if<std::is_base_of<C1, T2>::value>::type>  \
        inline Handle_##C1(const T2* theOther) : Handle(C1)(theOther){} template <typename T>                          \
        inline Handle_##C1 &                                                                                           \
        operator=(T theOther){Handle(C1)::operator=(theOther);                                                         \
    return *this;                                                                                                      \
    }                                                                                                                  \
    }                                                                                                                  \
    ;
#else
//! For other compilers, use simple typedef
#define DEFINE_STANDARD_HANDLECLASS(C1, C2, BC)                                                                        \
    class C1;                                                                                                          \
    typedef Handle(C1) Handle_##C1;
#endif

#define DEFINE_STANDARD_HANDLE(C1, C2) DEFINE_STANDARD_HANDLECLASS(C1, C2, Standard_Transient)
#define DEFINE_STANDARD_PHANDLE(C1, C2) DEFINE_STANDARD_HANDLECLASS(C1, C2, Standard_Persistent)

#endif
