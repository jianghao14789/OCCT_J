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

#ifndef _Standard_Type_HeaderFile
#define _Standard_Type_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

#include <typeinfo>

// Auxiliary tools to check at compile time that class declared as base in
// DEFINE_STANDARD_RTTI* macro is actually a base class.
// 辅助工具用于检查在 DEFINE_STANDARD_RTTI* 宏中声明的基类是否真的是基类
// 这是编译时检查，确保 RTTI 宏被正确使用
#if !defined(OCCT_CHECK_BASE_CLASS)

#if (defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || (__GNUC__ > 4)))

// For GCC 4.7+, more strict check is possible -- ensuring that base class
// is direct base -- using non-standard C++ reflection functionality.
// 对于 GCC 4.7+，可以进行更严格的检查 - 确保基类是直接基类
// 使用非标准的 C++ 反射功能

#include <tr2/type_traits>
#include <tuple>

namespace opencascade {
template <typename T> struct direct_base_class_as_tuple {};

template <typename... Ts> struct direct_base_class_as_tuple<std::tr2::__reflection_typelist<Ts...>> {
    typedef std::tuple<Ts...> type;
};

template <typename T, typename Tuple> struct has_type;

template <typename T> struct has_type<T, std::tuple<>> : std::false_type {};

template <typename T, typename U, typename... Ts>
struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {};

template <typename T, typename... Ts> struct has_type<T, std::tuple<T, Ts...>> : std::true_type {};
} // namespace opencascade

#define OCCT_CHECK_BASE_CLASS(Class, Base)                                                                             \
    using direct_base_classes = opencascade::direct_base_class_as_tuple<std::tr2::direct_bases<Class>::type>::type;    \
    static_assert(opencascade::has_type<Base, direct_base_classes>::type::value,                                       \
                  "OCCT RTTI definition is incorrect: " #Base " is not direct base class of " #Class);                 \
    static_assert(&get_type_name == &Class::get_type_name,                                                             \
                  "OCCT RTTI definition is misplaced: current class is not " #Class);

#elif (defined(_MSC_VER) && (_MSC_VER < 1600))

// VC9 does not support static_assert and decltype at all
// VC9 完全不支持 static_assert 和 decltype
#define OCCT_CHECK_BASE_CLASS(Class, Base)

#elif (defined(_MSC_VER) && (_MSC_VER >= 1900))

// VC14+ allow using address of member functions in static checks,
// that allows checking for the current type being correctly named in the macro
// VC14+ 允许在静态检查中使用成员函数地址，
// 这允许检查当前类型在宏中被正确命名
#define OCCT_CHECK_BASE_CLASS(Class, Base)                                                                             \
    static_assert(opencascade::is_base_but_not_same<Base, Class>::value,                                               \
                  "OCCT RTTI definition is incorrect: " #Base " is not base class of " #Class);                        \
    static_assert(&get_type_name == &Class::get_type_name,                                                             \
                  "OCCT RTTI definition is misplaced: current class is not " #Class);

#else

// by default, check only the base class
// 默认情况下，只检查基类
#define OCCT_CHECK_BASE_CLASS(Class, Base)                                                                             \
    static_assert(opencascade::is_base_but_not_same<Base, Class>::value,                                               \
                  "OCCT RTTI definition is incorrect: " #Base " is not base class of " #Class);

#endif

#endif /* ! defined(OCCT_CHECK_BASE_CLASS) */

//! Helper macro to get instance of a type descriptor for a class in a legacy way.
//! 辅助宏用于以传统方式获取类的类型描述符实例
#define STANDARD_TYPE(theType) theType::get_type_descriptor()

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI.
//!
//! Inline version, does not require IMPLEMENT_STANDARD_RTTIEXT, but when used
//! for big hierarchies of classes may cause considerable increase of size of binaries.
//!
//! 辅助宏，包含在继承 Standard_Transient 的类定义中以启用 OCCT RTTI
//! 内联版本，不需要 IMPLEMENT_STANDARD_RTTIEXT，但对于大型类层次结构可能导致二进制大小显著增加
#define DEFINE_STANDARD_RTTI_INLINE(Class, Base)                                                                       \
public:                                                                                                                \
    typedef Base base_type;                                                                                            \
    static const char* get_type_name() {                                                                               \
        return #Class;                                                                                                 \
        OCCT_CHECK_BASE_CLASS(Class, Base)                                                                             \
    }                                                                                                                  \
    static const Handle(Standard_Type) & get_type_descriptor() {                                                       \
        return Standard_Type::Instance<Class>();                                                                       \
    }                                                                                                                  \
    virtual const Handle(Standard_Type) & DynamicType() const Standard_OVERRIDE {                                      \
        return get_type_descriptor();                                                                                  \
    }

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI.
//!
//! Out-of-line version, requires IMPLEMENT_STANDARD_RTTIEXT.
//!
//! 辅助宏，包含在继承 Standard_Transient 的类定义中以启用 OCCT RTTI
//! 非内联版本，需要 IMPLEMENT_STANDARD_RTTIEXT 进行实现
#define DEFINE_STANDARD_RTTIEXT(Class, Base)                                                                           \
public:                                                                                                                \
    typedef Base base_type;                                                                                            \
    static const char* get_type_name() {                                                                               \
        return #Class;                                                                                                 \
        OCCT_CHECK_BASE_CLASS(Class, Base)                                                                             \
    }                                                                                                                  \
    Standard_EXPORT static const Handle(Standard_Type) & get_type_descriptor();                                        \
    Standard_EXPORT virtual const Handle(Standard_Type) & DynamicType() const Standard_OVERRIDE;

//! Defines implementation of type descriptor and DynamicType() function
//! 定义类型描述符的实现和 DynamicType() 函数
#define IMPLEMENT_STANDARD_RTTIEXT(Class, Base)                                                                        \
    const Handle(Standard_Type) & Class::get_type_descriptor() {                                                       \
        return Standard_Type::Instance<Class>();                                                                       \
    }                                                                                                                  \
    const Handle(Standard_Type) & Class::DynamicType() const {                                                         \
        return STANDARD_TYPE(Class);                                                                                   \
    }

// forward declaration of type_instance class
// 前向声明 type_instance 类
namespace opencascade {
template <typename T> class type_instance;
}

//! This class provides legacy interface (type descriptor) to run-time type
//! information (RTTI) for OCCT classes inheriting from Standard_Transient.
//!
//! In addition to features provided by standard C++ RTTI (type_info),
//! Standard_Type allows passing descriptor as an object and using it for
//! analysis of the type:
//! - get descriptor of a parent class
//! - get user-defined name of the class
//! - get size of the object
//!
//! Use static template method Instance() to get descriptor for a given type.
//! Objects supporting OCCT RTTI return their type descriptor by method DynamicType().
//!
//! To be usable with OCCT type system, the class should provide:
//! - typedef base_type to its base class in the hierarchy
//! - method get_type_name() returning programmer-defined name of the class
//!   (as a statically allocated constant C string or string literal)
//!
//! Note that user-defined name is used since typeid.name() is usually mangled in
//! compiler-dependent way.
//!
//! Only single chain of inheritance is supported, with a root base class Standard_Transient.
//!
// 该类为继承自 Standard_Transient 的 OCCT 类提供运行时类型信息（RTTI）的传统接口。
//
// 除了标准 C++ RTTI（type_info）提供的功能，Standard_Type 允许将描述符作为对象传递，
// 并用于类型分析：
// - 获取父类的描述符
// - 获取类的用户定义名称
// - 获取对象大小
//
// 使用静态模板方法 Instance() 获取给定类型的描述符。
// 支持 OCCT RTTI 的对象通过 DynamicType() 方法返回其类型描述符。
//
// 为了与 OCCT 类型系统兼容，类应提供:
// - typedef base_type 到层次结构中的基类
// - get_type_name() 方法返回类的程序员定义名称 (作为静态分配的常量 C 字符串或字符串文字)
//
// 注意使用用户定义的名称，因为 typeid.name() 通常以编译器相关的方式重整.
// 仅支持单链继承, 根基类是 Standard_Transient.

// RTTI 的唯一作用: 在程序运行时, 透过基类指针, 看清对象的真面目
class Standard_Type : public Standard_Transient {
public:
    //! Returns the system type name of the class (typeinfo.name)
    // 返回类的系统类型名称（来自 typeinfo.name）
    // 此名称是编译器生成的，通常被重整（mangled），不易读
    // READ by jh
    Standard_CString SystemName() const {
        return mySystemName;
    }

    //! Returns the given name of the class type (get_type_name)
    // 返回类型的给定名称（来自 get_type_name）
    // 此名称是程序员定义的，易读，推荐使用
    // READ by jh
    Standard_CString Name() const {
        return myName;
    }

    //! Returns the size of the class instance in bytes
    // 返回类实例的大小（字节）
    // 等同于 sizeof(ClassName)
    // READ by jh
    Standard_Size Size() const {
        return mySize;
    }

    //! Returns descriptor of the base class in the hierarchy
    // 返回层次结构中基类的描述符
    // 如果已经是根类（Standard_Transient），返回 NULL
    // READ by jh
    const Handle(Standard_Type) & Parent() const {
        return myParent;
    }

    //! Returns True if this type is the same as theOther, or inherits from theOther.
    //! Note that multiple inheritance is not supported.
    // 检查此类型是否与 theOther 相同，或继承自 theOther
    // 返回 True 表示存在继承关系
    // 注意：不支持多重继承
    Standard_EXPORT Standard_Boolean SubType(const Handle(Standard_Type) & theOther) const;

    //! Returns True if this type is the same as theOther, or inherits from theOther.
    // Note that multiple inheritance is not supported.
    // 通过名称检查此类型是否与 theOther 相同，或继承自 theOther
    Standard_EXPORT Standard_Boolean SubType(const Standard_CString theOther) const;

    //! Prints type (address of descriptor + name) to a stream
    // 将类型信息（描述符地址 + 名称）打印到流
    // 用于调试和日志记录
    Standard_EXPORT void Print(Standard_OStream& theStream) const;

    //! Template function returning instance of the type descriptor for an argument class.
    //! For optimization, each type is registered only once (due to use of the static variable).
    //! See helper macro DEFINE_STANDARD_RTTI for defining these items in the class.
    // 模板函数返回参数类的类型描述符实例
    // 为优化，每个类型只被注册一次（使用静态变量）
    // 见宏 DEFINE_STANDARD_RTTI_INLINE 或 DEFINE_STANDARD_RTTIEXT
    template <class T> static const Handle(Standard_Type) & Instance() {
        return opencascade::type_instance<T>::get();
    }

    //! Register a type; returns either new or existing descriptor.
    //!
    //! @param theSystemName name of the class as returned by typeid(class).name()
    //! @param theName name of the class to be stored in Name field
    //! @param theSize size of the class instance
    //! @param theParent base class in the Transient hierarchy
    //!
    //! Note that this function is intended for use by opencascade::type_instance only.
    //
    // 注册一个类型；返回新的或现有的描述符
    // @param theSystemName 类的系统名称（来自 typeid(class).name()）
    // @param theName 要存储在 Name 字段中的类名称
    // @param theSize 类实例的大小
    // @param theParent Transient 层次结构中的基类
    // 注意：此函数仅供 opencascade::type_instance 内部使用
    Standard_EXPORT static Standard_Type* Register(const char* theSystemName, const char* theName,
                                                   Standard_Size theSize, const Handle(Standard_Type) & theParent);

    //! Destructor removes the type from the registry
    // 析构函数从注册表中删除类型
    Standard_EXPORT ~Standard_Type();

    // Define own RTTI
    // 定义自己的 RTTI
    DEFINE_STANDARD_RTTIEXT(Standard_Type, Standard_Transient)

private:
    //! Constructor is private
    //! 构造函数是私有的，只能通过 Register() 函数创建实例
    Standard_Type(const char* theSystemName, const char* theName, Standard_Size theSize,
                  const Handle(Standard_Type) & theParent);

private:
    Standard_CString mySystemName; //!< System name of the class (typeinfo.name)
    // 类的系统名称（来自 typeinfo.name）
    Standard_CString myName; //!< Given name of the class
    // 给定的类名称（易读）
    Standard_Size mySize; //!< Size of the class instance, in bytes
    // 类实例的大小（字节）
    Handle(Standard_Type) myParent; //!< Type descriptor of parent class
    // 父类的类型描述符
};

namespace opencascade {

//! Template class providing instantiation of type descriptors as singletons.
//! The descriptors are defined as static variables in function get(), which
//! is essential to ensure that they are initialized in correct sequence.
//!
//! For compilers that do not provide thread-safe initialization of static
//! variables (C++11 feature, N2660), additional global variable is
//! defined for each type to hold its type descriptor. These globals ensure
//! that all types get initialized during the library loading and thus no
//! concurrency occurs when type system is accessed from multiple threads.
//!
//! 提供类型描述符单例实例化的模板类
//! 描述符定义为 get() 函数中的静态变量，这对确保正确的初始化顺序至关重要
//! 对于不提供线程安全静态变量初始化的编译器（C++11 特性 N2660），
//! 为每个类型定义额外的全局变量以保存其类型描述符。
//! 这些全局变量确保所有类型在库加载期间初始化，
//! 从而当从多个线程访问类型系统时不会发生并发问题。
template <typename T> class type_instance {
    static Handle(Standard_Type) myInstance;

public:
    static const Handle(Standard_Type) & get();
};

//! Specialization of type descriptor instance for void; returns null handle
//! void 类型描述符实例的特殊化；返回空 Handle
template <> class type_instance<void> {
public:
    static Handle(Standard_Type) get() {
        return 0;
    }
};

// Implementation of static function returning instance of the
// type descriptor
// 返回类型描述符实例的静态函数的实现
template <typename T> const Handle(Standard_Type) & type_instance<T>::get() {
#if (defined(_MSC_VER) && _MSC_VER < 1900) ||                                                                          \
    (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)) && !defined(__clang__) &&            \
     !defined(__INTEL_COMPILER))
    // ensure that myInstance is instantiated
    // 确保 myInstance 被实例化
    (void)myInstance;
#endif

    // static variable inside function ensures that descriptors
    // are initialized in correct sequence
    // 函数内的静态变量确保描述符以正确的顺序初始化
    static Handle(Standard_Type) anInstance = Standard_Type::Register(typeid(T).name(), T::get_type_name(), sizeof(T),
                                                                      type_instance<typename T::base_type>::get());
    return anInstance;
}

// Static class field is defined to ensure initialization of all type
// descriptors at load time of the library on compilers not supporting N2660:
// - VC++ below 14 (VS 2015)
// - GCC below 4.3
// Intel compiler reports itself as GCC on Linux and VC++ on Windows,
// and is claimed to support N2660 on Linux and on Windows "in VS2015 mode".
// CLang should support N2660 since version 2.9, but it is not clear how to
// check its version reliably (on Linux it says it is GCC 4.2).
//
// 定义静态类字段以确保在库加载时初始化所有类型描述符
// 适用于不支持 N2660 的编译器：
// - VC++ 版本低于 14（VS 2015）
// - GCC 版本低于 4.3
// Intel 编译器在 Linux 上报告自己为 GCC，在 Windows 上报告为 VC++，
// 并声称在 Linux 和 Windows "VS2015 模式"下支持 N2660。
// CLang 应该从版本 2.9 起支持 N2660，但无法可靠地检查其版本
// （在 Linux 上它声称是 GCC 4.2）。
#if (defined(_MSC_VER) && _MSC_VER < 1900) ||                                                                          \
    (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)) && !defined(__clang__) &&            \
     !defined(__INTEL_COMPILER))

template <typename T> Handle(Standard_Type) type_instance<T>::myInstance(get());

#endif
} // namespace opencascade

//! Operator printing type descriptor to stream
// 操作符将类型描述符打印到流
inline Standard_OStream& operator<<(Standard_OStream& theStream, const Handle(Standard_Type) & theType) {
    theType->Print(theStream);
    return theStream;
}

//! Definition of Handle_Standard_Type as typedef for compatibility
// 为兼容性定义 Handle_Standard_Type 为 typedef
DEFINE_STANDARD_HANDLE(Standard_Type, Standard_Transient)

#endif // _Standard_Type_HeaderFile
