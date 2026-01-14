// Created on: 1996-11-29
// Created by: Christophe LEYNADIER
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _FSD_File_HeaderFile
#define _FSD_File_HeaderFile

#include <FSD_FStream.hxx>
#include <Storage_BaseDriver.hxx>

class TCollection_AsciiString;
class TCollection_ExtendedString;

DEFINE_STANDARD_HANDLE(FSD_File, Storage_BaseDriver)

//! FSD_File 是文本格式的文件存储驱动程序
//! 继承自 Storage_BaseDriver，用于以文本/ASCII格式读写对象数据
//! 
//! 与 FSD_BinaryFile 的区别：
//! - FSD_File：文本格式，可读性强，便于调试，但文件较大
//! - FSD_BinaryFile：二进制格式，效率高，文件小，但不可直接阅读
//!
//! FSD_File 主要特点：
//! - 以易读的文本格式存储对象数据
//! - 支持多行文本数据和注释
//! - 自动处理行尾和空白字符
//! - 便于版本管理和人工审查
//! - 数据结构与 FSD_BinaryFile 相同，但编码方式不同
class FSD_File : public Storage_BaseDriver
{
public:
    DEFINE_STANDARD_RTTIEXT(FSD_File, Storage_BaseDriver)

public:

    //! 构造函数
    //! 创建一个文件存储驱动程序实例，用于文本格式的文件操作。
    //! 使用 Open 函数来指定要驱动的文件名。
    Standard_EXPORT FSD_File();


    //! 打开或创建文件
    //! 将 aName 指定为此驱动程序驱动的文件名。
    //! aMode 指定文件是以读或写模式打开。
    //! 如果文件成功打开，返回 Storage_VSOk，否则返回 Storage_Error 枚举中的相应错误代码。
    Standard_EXPORT virtual Storage_Error Open(const TCollection_AsciiString& aName, const Storage_OpenMode aMode) Standard_OVERRIDE;

    //! 判断是否已到达文件末尾
    Standard_EXPORT virtual Standard_Boolean IsEnd() Standard_OVERRIDE;

    //! 返回文件中的当前位置（字节偏移）。出错时返回 -1。
    Standard_EXPORT virtual Storage_Position Tell() Standard_OVERRIDE;

    //! 静态函数：检查指定文件是否为有效的 FSD 文本格式文件
    Standard_EXPORT static Storage_Error IsGoodFileType(const TCollection_AsciiString& aName);

    // ========== 信息段（Info Section）操作函数 ==========

    //! 开始写入文件信息段
    Standard_EXPORT virtual Storage_Error BeginWriteInfoSection() Standard_OVERRIDE;

    //! 写入文件元信息（版本、日期、应用名等）
    Standard_EXPORT virtual void WriteInfo(const Standard_Integer nbObj,
        const TCollection_AsciiString& dbVersion,
        const TCollection_AsciiString& date,
        const TCollection_AsciiString& schemaName,
        const TCollection_AsciiString& schemaVersion,
        const TCollection_ExtendedString& appName,
        const TCollection_AsciiString& appVersion,
        const TCollection_ExtendedString& objectType,
        const TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;

    //! 结束写入信息段
    Standard_EXPORT virtual Storage_Error EndWriteInfoSection() Standard_OVERRIDE;

    //! 开始读取文件信息段
    Standard_EXPORT virtual Storage_Error BeginReadInfoSection() Standard_OVERRIDE;

    //! 读取文件元信息
    Standard_EXPORT virtual void ReadInfo(Standard_Integer& nbObj,
        TCollection_AsciiString& dbVersion,
        TCollection_AsciiString& date,
        TCollection_AsciiString& schemaName,
        TCollection_AsciiString& schemaVersion,
        TCollection_ExtendedString& appName,
        TCollection_AsciiString& appVersion,
        TCollection_ExtendedString& objectType,
        TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;

    //! 读取完整的信息段，包括版本兼容性检查
    Standard_EXPORT virtual void ReadCompleteInfo(Standard_IStream& theIStream, Handle(Storage_Data)& theData) Standard_OVERRIDE;

    //! 结束读取信息段
    Standard_EXPORT virtual Storage_Error EndReadInfoSection() Standard_OVERRIDE;

    // ========== 注释段（Comment Section）操作函数 ==========

    //! 开始写入注释段
    Standard_EXPORT virtual Storage_Error BeginWriteCommentSection() Standard_OVERRIDE;

    //! 写入用户注释信息
    Standard_EXPORT virtual void WriteComment(const TColStd_SequenceOfExtendedString& userComments) Standard_OVERRIDE;

    //! 结束写入注释段
    Standard_EXPORT virtual Storage_Error EndWriteCommentSection() Standard_OVERRIDE;

    //! 开始读取注释段
    Standard_EXPORT virtual Storage_Error BeginReadCommentSection() Standard_OVERRIDE;

    //! 读取注释内容
    Standard_EXPORT virtual void ReadComment(TColStd_SequenceOfExtendedString& userComments) Standard_OVERRIDE;

    //! 结束读取注释段
    Standard_EXPORT virtual Storage_Error EndReadCommentSection() Standard_OVERRIDE;

    // ========== 类型段（Type Section）操作函数 ==========

    //! 开始写入类型定义段
    Standard_EXPORT virtual Storage_Error BeginWriteTypeSection() Standard_OVERRIDE;

    //! 设置类型段的大小（对象类型总数）
    Standard_EXPORT virtual void SetTypeSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

    //! 写入单个类型的信息（类型编号和类型名称）
    Standard_EXPORT virtual void WriteTypeInformations(const Standard_Integer typeNum,
        const TCollection_AsciiString& typeName) Standard_OVERRIDE;

    //! 结束写入类型段
    Standard_EXPORT virtual Storage_Error EndWriteTypeSection() Standard_OVERRIDE;

    //! 开始读取类型段
    Standard_EXPORT virtual Storage_Error BeginReadTypeSection() Standard_OVERRIDE;

    //! 获取类型段的大小（类型总数）
    Standard_EXPORT virtual Standard_Integer TypeSectionSize() Standard_OVERRIDE;

    //! 读取单个类型信息
    Standard_EXPORT virtual void ReadTypeInformations(Standard_Integer& typeNum, TCollection_AsciiString& typeName) Standard_OVERRIDE;

    //! 结束读取类型段
    Standard_EXPORT virtual Storage_Error EndReadTypeSection() Standard_OVERRIDE;

    // ========== 根对象段（Root Section）操作函数 ==========

    //! 开始写入根对象段
    Standard_EXPORT virtual Storage_Error BeginWriteRootSection() Standard_OVERRIDE;

    //! 设置根对象段的大小（根对象总数）
    Standard_EXPORT virtual void SetRootSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

    //! 写入单个根对象的信息（名称、引用号、类型）
    Standard_EXPORT virtual void WriteRoot(const TCollection_AsciiString& rootName,
        const Standard_Integer aRef,
        const TCollection_AsciiString& aType) Standard_OVERRIDE;

    //! 结束写入根对象段
    Standard_EXPORT virtual Storage_Error EndWriteRootSection() Standard_OVERRIDE;

    //! 开始读取根对象段
    Standard_EXPORT virtual Storage_Error BeginReadRootSection() Standard_OVERRIDE;

    //! 获取根对象段的大小
    Standard_EXPORT virtual Standard_Integer RootSectionSize() Standard_OVERRIDE;

    //! 读取单个根对象信息
    Standard_EXPORT virtual void ReadRoot(TCollection_AsciiString& rootName,
        Standard_Integer& aRef,
        TCollection_AsciiString& aType) Standard_OVERRIDE;

    //! 结束读取根对象段
    Standard_EXPORT virtual Storage_Error EndReadRootSection() Standard_OVERRIDE;

    // ========== 引用段（Reference Section）操作函数 ==========

    //! 开始写入引用段
    Standard_EXPORT virtual Storage_Error BeginWriteRefSection() Standard_OVERRIDE;

    //! 设置引用段的大小（引用总数）
    Standard_EXPORT virtual void SetRefSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

    //! 写入单个引用信息（对象引用号和其类型号）
    Standard_EXPORT virtual void WriteReferenceType(const Standard_Integer reference, const Standard_Integer typeNum) Standard_OVERRIDE;

    //! 结束写入引用段
    Standard_EXPORT virtual Storage_Error EndWriteRefSection() Standard_OVERRIDE;

    //! 开始读取引用段
    Standard_EXPORT virtual Storage_Error BeginReadRefSection() Standard_OVERRIDE;

    //! 获取引用段的大小
    Standard_EXPORT virtual Standard_Integer RefSectionSize() Standard_OVERRIDE;

    //! 读取单个引用信息
    Standard_EXPORT virtual void ReadReferenceType(Standard_Integer& reference, Standard_Integer& typeNum) Standard_OVERRIDE;

    //! 结束读取引用段
    Standard_EXPORT virtual Storage_Error EndReadRefSection() Standard_OVERRIDE;

    // ========== 数据段（Data Section）操作函数 ==========

    //! 开始写入数据段
    Standard_EXPORT virtual Storage_Error BeginWriteDataSection() Standard_OVERRIDE;

    //! 写入持久化对象的头部（引用号和类型号）
    Standard_EXPORT virtual void WritePersistentObjectHeader(const Standard_Integer aRef, const Standard_Integer aType) Standard_OVERRIDE;

    //! 开始写入持久化对象的数据
    Standard_EXPORT virtual void BeginWritePersistentObjectData() Standard_OVERRIDE;

    //! 开始写入对象的具体内容数据
    Standard_EXPORT virtual void BeginWriteObjectData() Standard_OVERRIDE;

    //! 结束写入对象的具体内容数据
    Standard_EXPORT virtual void EndWriteObjectData() Standard_OVERRIDE;

    //! 结束写入持久化对象的数据
    Standard_EXPORT virtual void EndWritePersistentObjectData() Standard_OVERRIDE;

    //! 结束写入数据段
    Standard_EXPORT virtual Storage_Error EndWriteDataSection() Standard_OVERRIDE;

    //! 开始读取数据段
    Standard_EXPORT virtual Storage_Error BeginReadDataSection() Standard_OVERRIDE;

    //! 读取持久化对象的头部
    Standard_EXPORT virtual void ReadPersistentObjectHeader(Standard_Integer& aRef, Standard_Integer& aType) Standard_OVERRIDE;

    //! 开始读取持久化对象的数据
    Standard_EXPORT virtual void BeginReadPersistentObjectData() Standard_OVERRIDE;

    //! 开始读取对象的具体内容数据
    Standard_EXPORT virtual void BeginReadObjectData() Standard_OVERRIDE;

    //! 结束读取对象的具体内容数据
    Standard_EXPORT virtual void EndReadObjectData() Standard_OVERRIDE;

    //! 结束读取持久化对象的数据
    Standard_EXPORT virtual void EndReadPersistentObjectData() Standard_OVERRIDE;

    //! 结束读取数据段
    Standard_EXPORT virtual Storage_Error EndReadDataSection() Standard_OVERRIDE;

    // ========== 数据读写基本操作函数 ==========

    //! 跳过当前对象的所有数据（用于快速导航和选择性读取）
    Standard_EXPORT virtual void SkipObject() Standard_OVERRIDE;

    //! 写入对象引用（整数）
    Standard_EXPORT virtual Storage_BaseDriver& PutReference(const Standard_Integer aValue) Standard_OVERRIDE;

    //! 写入单个字符
    Standard_EXPORT virtual Storage_BaseDriver& PutCharacter(const Standard_Character aValue) Standard_OVERRIDE;

    //! 写入扩展字符（Unicode字符）
    Standard_EXPORT virtual Storage_BaseDriver& PutExtCharacter(const Standard_ExtCharacter aValue) Standard_OVERRIDE;

    //! 写入整数
    Standard_EXPORT virtual Storage_BaseDriver& PutInteger(const Standard_Integer aValue) Standard_OVERRIDE;

    //! 写入布尔值
    Standard_EXPORT virtual Storage_BaseDriver& PutBoolean(const Standard_Boolean aValue) Standard_OVERRIDE;

    //! 写入双精度实数
    Standard_EXPORT virtual Storage_BaseDriver& PutReal(const Standard_Real aValue) Standard_OVERRIDE;

    //! 写入单精度实数
    Standard_EXPORT virtual Storage_BaseDriver& PutShortReal(const Standard_ShortReal aValue) Standard_OVERRIDE;

    //! 读取对象引用
    Standard_EXPORT virtual Storage_BaseDriver& GetReference(Standard_Integer& aValue) Standard_OVERRIDE;

    //! 读取单个字符
    Standard_EXPORT virtual Storage_BaseDriver& GetCharacter(Standard_Character& aValue) Standard_OVERRIDE;

    //! 读取扩展字符
    Standard_EXPORT virtual Storage_BaseDriver& GetExtCharacter(Standard_ExtCharacter& aValue) Standard_OVERRIDE;

    //! 读取整数
    Standard_EXPORT virtual Storage_BaseDriver& GetInteger(Standard_Integer& aValue) Standard_OVERRIDE;

    //! 读取布尔值
    Standard_EXPORT virtual Storage_BaseDriver& GetBoolean(Standard_Boolean& aValue) Standard_OVERRIDE;

    //! 读取双精度实数
    Standard_EXPORT virtual Storage_BaseDriver& GetReal(Standard_Real& aValue) Standard_OVERRIDE;

    //! 读取单精度实数
    Standard_EXPORT virtual Storage_BaseDriver& GetShortReal(Standard_ShortReal& aValue) Standard_OVERRIDE;

    //! 关闭此驱动程序驱动的文件
    //! 此文件由最后一次调用 Open 函数打开。
    //! 如果关闭成功，返回 Storage_VSOk，否则返回 Storage_Error 枚举中的错误代码。
    Standard_EXPORT virtual Storage_Error Close() Standard_OVERRIDE;

    //! 销毁对象并释放资源
    Standard_EXPORT void Destroy();

    ~FSD_File()
    {
        Destroy();
    }

    //! 返回文件的魔数（标识符），用于区分文件类型
    Standard_EXPORT static Standard_CString MagicNumber();

protected:

    //! 从当前位置读取至行尾的文本内容到缓冲区
    Standard_EXPORT virtual void ReadLine(TCollection_AsciiString& buffer);

    //! 从当前位置读取至下一个空白符或行尾的单个单词
    Standard_EXPORT virtual void ReadWord(TCollection_AsciiString& buffer);

    //! 从当前位置读取扩展字符（Unicode）至行尾
    Standard_EXPORT virtual void ReadExtendedLine(TCollection_ExtendedString& buffer);

    //! 将扩展字符串写入文件并换行
    Standard_EXPORT virtual void WriteExtendedLine(const TCollection_ExtendedString& buffer);

    //! 从当前位置读取指定长度的字符到缓冲区
    Standard_EXPORT virtual void ReadChar(TCollection_AsciiString& buffer, const Standard_Size rsize);

    //! 从第一个非空白字符开始读取至行尾
    Standard_EXPORT virtual void ReadString(TCollection_AsciiString& buffer);

    //! 刷新到行尾（写入换行符）
    Standard_EXPORT virtual void FlushEndOfLine();

    //! 查找文件中特定的标签/关键字，用于定位数据段
    Standard_EXPORT virtual Storage_Error FindTag(const Standard_CString aTag);

protected:
    FSD_FStream myStream;  // 文本格式的文件流对象
};

#endif // _FSD_File_HeaderFile
