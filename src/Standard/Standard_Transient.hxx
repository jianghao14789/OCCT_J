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

#ifndef _Standard_Transient_HeaderFile
#define _Standard_Transient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_PrimitiveTypes.hxx>

class Standard_Type;

namespace opencascade {
    template <class T> class handle;
}

//! Abstract class which forms the root of the entire 
//! Transient class hierarchy.
//!
//! 抽象类，是整个瞬时对象类层次结构的根
//! 
//! 说明：Transient 意为"短暂的"，这里表示这些对象具有生命周期管理的特性。
//! 在 OCCT 中，所有需要通过 Handle（智能指针）管理的对象都继承自 Standard_Transient。
//! 它通过引用计数机制来管理对象的生命周期。

class Standard_Transient
{
public:
    // Standard OCCT memory allocation stuff
    // OCCT 标准内存分配定义
    DEFINE_STANDARD_ALLOC;

public:

    //! Empty constructor
    //! 空构造函数 - 初始化引用计数为 0
    Standard_Transient() : myRefCount_(0) {}

    //! Copy constructor -- does nothing
    //! 拷贝构造函数 - 不执行任何操作（引用计数不复制）
    //! 这样做的原因是新对象应该有自己独立的引用计数
    Standard_Transient(const Standard_Transient&) : myRefCount_(0) {}

    //! Assignment operator, needed to avoid copying reference counter
    //! 赋值运算符 - 避免复制引用计数
    //! 类似地，赋值时不修改对象本身的引用计数
    Standard_Transient& operator= (const Standard_Transient&) { return *this; }

    //! Destructor must be virtual
    //! 虚析构函数 - 确保派生类能被正确销毁
    //! 当引用计数降到 0 时，系统会调用此函数
    virtual ~Standard_Transient() {}

    //! Memory deallocator for transient classes
    //! 为瞬时类删除内存的函数
    //! 由 Handle 智能指针在引用计数为 0 时调用
    Standard_EXPORT virtual void Delete() const;

public:
    //!@name Support of run-time type information (RTTI)
    //! 运行时类型信息（RTTI）支持 - 允许在运行时确定对象的真实类型

    typedef void base_type;

    static const char* get_type_name() { return "Standard_Transient"; }

    //! Returns type descriptor of Standard_Transient class
    //! 返回 Standard_Transient 类的类型描述符
    //! 类型描述符包含类的元信息，用于类型检查和转换
    Standard_EXPORT static const opencascade::handle<Standard_Type>& get_type_descriptor();

    //! Returns a type descriptor about this object.
    //! 返回此对象的类型描述符
    //! 对于派生类对象，返回的是派生类的类型描述符
    Standard_EXPORT virtual const opencascade::handle<Standard_Type>& DynamicType() const;

    //! Returns a true value if this is an instance of Type.
    //! 检查此对象是否是指定类型的实例（精确匹配）
    //! 不检查继承关系，只检查完全相同的类型
    Standard_EXPORT Standard_Boolean IsInstance(const opencascade::handle<Standard_Type>& theType) const;

    //! Returns a true value if this is an instance of TypeName.
    //! 通过类型名检查此对象是否是指定类型的实例（精确匹配）
    Standard_EXPORT Standard_Boolean IsInstance(const Standard_CString theTypeName) const;

    //! Returns true if this is an instance of Type or an
    //! instance of any class that inherits from Type.
    //! Note that multiple inheritance is not supported by OCCT RTTI mechanism.
    //! 检查此对象是否是指定类型或其派生类的实例
    //! 包括继承检查，这是类型安全的向下转换检查
    //! 注意：OCCT RTTI 机制不支持多重继承
    Standard_EXPORT Standard_Boolean IsKind(const opencascade::handle<Standard_Type>& theType) const;

    //! Returns true if this is an instance of TypeName or an
    //! instance of any class that inherits from TypeName.
    //! Note that multiple inheritance is not supported by OCCT RTTI mechanism.
    //! 通过类型名检查此对象是否是指定类型或其派生类的实例
    //! 注意：OCCT RTTI 机制不支持多重继承
    Standard_EXPORT Standard_Boolean IsKind(const Standard_CString theTypeName) const;

    //! Returns non-const pointer to this object (like const_cast).
    //! For protection against creating handle to objects allocated in stack
    //! or call from constructor, it will raise exception Standard_ProgramError
    //! if reference counter is zero.
    //! 返回非 const 指针，类似于 const_cast
    //! 为了防止为栈上分配的对象或在构造函数中调用创建 Handle，
    //! 如果引用计数为 0，将抛出 Standard_ProgramError 异常
    //! 这是一个安全检查，确保不会创建指向临时对象的 Handle
    Standard_EXPORT Standard_Transient* This() const;

public:
    //!@name Reference counting, for use by handle<>
    //! 引用计数 - 供 Handle 智能指针使用
    //! 这是 OCCT 内存管理的核心机制

    //! Get the reference counter of this object
    //! 获取此对象的引用计数
    //! 返回值表示有多少个 Handle 指向此对象
    Standard_Integer GetRefCount() const { return myRefCount_; }

    //! Increments the reference counter of this object
    //! 增加此对象的引用计数
    //! 当新建一个 Handle 指向此对象时调用（+1）
    Standard_EXPORT void IncrementRefCounter() const;

    //! Decrements the reference counter of this object;
    //! returns the decremented value
    //! 减少此对象的引用计数，并返回减少后的值
    //! 当一个 Handle 不再指向此对象时调用（-1）
    //! 当返回值为 0 时，对象应该被删除
    Standard_EXPORT Standard_Integer DecrementRefCounter() const;

private:

    //! Reference counter.
    //! Note use of underscore, aimed to reduce probability 
    //! of conflict with names of members of derived classes.
    //! 引用计数 - 追踪有多少个 Handle 指向此对象
    //! 使用 myRefCount_ 这样的名称（带下划线）是为了减少与派生类成员变量名冲突的概率
    //! volatile 关键字确保在多线程环境下的正确性
    mutable volatile Standard_Integer myRefCount_;
};


//! Computes a hash code for the given transient object, in the range [1, theUpperBound]
//! @param theTransientObject the transient object which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
//! 
//! 为给定的瞬时对象计算哈希码（在 [1, theUpperBound] 范围内）
//! 用于在哈希表或其他数据结构中快速查找对象
//! @param theTransientObject 要计算哈希码的瞬时对象
//! @param theUpperBound 哈希码范围的上界
//! @return 计算得到的哈希码，范围在 [1, theUpperBound]
inline Standard_Integer HashCode(const Standard_Transient* const theTransientObject,
    const Standard_Integer          theUpperBound)
{
    return ::HashCode(static_cast<const void*> (theTransientObject), theUpperBound);
}

//! Definition of Handle_Standard_Transient as typedef for compatibility
//! 定义 Handle_Standard_Transient 为兼容性别名
typedef opencascade::handle<Standard_Transient> Handle_Standard_Transient;

#endif 
