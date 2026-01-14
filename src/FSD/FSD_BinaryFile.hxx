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

#ifndef _FSD_BinaryFile_HeaderFile
#define _FSD_BinaryFile_HeaderFile

#include <FSD_BStream.hxx>
#include <FSD_FileHeader.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_Error.hxx>
#include <Storage_OpenMode.hxx>
#include <Storage_Data.hxx>
#include <Standard_Boolean.hxx>
#include <Storage_Position.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

class TCollection_AsciiString;
class TCollection_ExtendedString;
class Storage_HeaderData;

// 宏定义：用于判断是否需要进行字节序反转
// 在读写二进制文件时，需要考虑大小端（Endian）问题
// 大端平台（SOLARIS、IRIX）需要反转字节顺序以实现平台兼容性
#ifndef OCCT_BINARY_FILE_DO_INVERSE
#if defined ( SOLARIS ) || defined ( IRIX )
// 在大端平台上进行字节反转
#define OCCT_BINARY_FILE_DO_INVERSE 1
#else
#define OCCT_BINARY_FILE_DO_INVERSE 0
#endif
#endif

DEFINE_STANDARD_HANDLE(FSD_BinaryFile, Storage_BaseDriver)

//! FSD_BinaryFile 是二进制文件存储驱动程序
//! 继承自 Storage_BaseDriver，用于在二进制格式的文件中读写对象数据
//! 
//! 主要特点：
//! - 以二进制格式（而非文本格式）存储数据，效率更高、文件更小
//! - 支持大端/小端平台的自动字节序转换
//! - 将文件分为多个逻辑段（Section），便于管理和导航
//! - 支持对象头、数据、引用等多种数据类型的序列化
//!
//! 文件结构：
//! - FileHeader: 文件头，记录各段的位置
//! - InfoSection: 数据库信息段（版本、日期、应用名等）
//! - CommentSection: 注释段（用户自定义信息）
//! - TypeSection: 类型定义段（对象类型映射）
//! - RootSection: 根对象段
//! - RefSection: 引用关系段
//! - DataSection: 对象数据段
class FSD_BinaryFile : public Storage_BaseDriver
{
public:
    DEFINE_STANDARD_RTTIEXT(FSD_BinaryFile, Storage_BaseDriver)

public:

    //! 构造函数，创建一个二进制文件驱动程序实例
    Standard_EXPORT FSD_BinaryFile();

    //! 打开或创建文件
    //! @param aName 文件路径名
    //! @param aMode 打开模式（读或写）
    //! @return 返回操作状态代码
    Standard_EXPORT Storage_Error Open(const TCollection_AsciiString& aName, const Storage_OpenMode aMode) Standard_OVERRIDE;

    //! 判断是否已到达文件末尾
    //! @return 如果已到达文件末尾返回 True，否则返回 False
    Standard_EXPORT Standard_Boolean IsEnd() Standard_OVERRIDE;

    //! 返回文件中的当前位置（字节偏移）。出错时返回 -1。
    Standard_EXPORT Storage_Position Tell() Standard_OVERRIDE;

    //! 静态函数：检查文件是否为有效的二进制格式存储文件
    Standard_EXPORT static Storage_Error IsGoodFileType(const TCollection_AsciiString& aName);

    // ========== 信息段（Info Section）操作函数 ==========

    //! 开始写入文件信息段
    Standard_EXPORT Storage_Error BeginWriteInfoSection() Standard_OVERRIDE;

    //! 静态函数：将信息段数据写入流
    Standard_EXPORT static Standard_Integer WriteInfo(Standard_OStream& theOStream,
        const Standard_Integer nbObj,
        const TCollection_AsciiString& dbVersion,
        const TCollection_AsciiString& date,
        const TCollection_AsciiString& schemaName,
        const TCollection_AsciiString& schemaVersion,
        const TCollection_ExtendedString& appName,
        const TCollection_AsciiString& appVersion,
        const TCollection_ExtendedString& objectType,
        const TColStd_SequenceOfAsciiString& userInfo,
        const Standard_Boolean theOnlyCount = Standard_False);

    //! 写入文件信息段
    Standard_EXPORT void WriteInfo(const Standard_Integer nbObj,
        const TCollection_AsciiString& dbVersion,
        const TCollection_AsciiString& date,
        const TCollection_AsciiString& schemaName,
        const TCollection_AsciiString& schemaVersion,
        const TCollection_ExtendedString& appName,
        const TCollection_AsciiString& appVersion,
        const TCollection_ExtendedString& objectType,
        const TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;

    //! 结束写入信息段
    Standard_EXPORT Storage_Error EndWriteInfoSection() Standard_OVERRIDE;

    Standard_EXPORT Storage_Error EndWriteInfoSection(Standard_OStream& theOStream);

    //! 开始读取文件信息段
    Standard_EXPORT Storage_Error BeginReadInfoSection() Standard_OVERRIDE;

    //! 读取文件信息段内容
    Standard_EXPORT void ReadInfo(Standard_Integer& nbObj,
        TCollection_AsciiString& dbVersion,
        TCollection_AsciiString& date,
        TCollection_AsciiString& schemaName,
        TCollection_AsciiString& schemaVersion,
        TCollection_ExtendedString& appName,
        TCollection_AsciiString& appVersion,
        TCollection_ExtendedString& objectType,
        TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;

    //! 读取完整的信息段，包括版本兼容性检查
    Standard_EXPORT void ReadCompleteInfo(Standard_IStream& theIStream, Handle(Storage_Data)& theData) Standard_OVERRIDE;

    //! 结束读取信息段
    Standard_EXPORT Storage_Error EndReadInfoSection() Standard_OVERRIDE;

    // ========== 注释段（Comment Section）操作函数 ==========

    //! 开始写入注释段
    Standard_EXPORT Storage_Error BeginWriteCommentSection() Standard_OVERRIDE;

    Standard_EXPORT Storage_Error BeginWriteCommentSection(Standard_OStream& theOStream);

    //! 写入注释内容
    Standard_EXPORT void WriteComment(const TColStd_SequenceOfExtendedString& userComments) Standard_OVERRIDE;

    Standard_EXPORT static Standard_Integer WriteComment(Standard_OStream& theOStream,
        const TColStd_SequenceOfExtendedString& theComments,
        const Standard_Boolean theOnlyCount = Standard_False);

    //! 结束写入注释段
    Standard_EXPORT Storage_Error EndWriteCommentSection() Standard_OVERRIDE;

    Standard_EXPORT Storage_Error EndWriteCommentSection(Standard_OStream& theOStream);

    //! 开始读取注释段
    Standard_EXPORT Storage_Error BeginReadCommentSection() Standard_OVERRIDE;

    //! 读取注释内容
    Standard_EXPORT void ReadComment(TColStd_SequenceOfExtendedString& userComments) Standard_OVERRIDE;

    Standard_EXPORT static void ReadComment(Standard_IStream& theIStream, TColStd_SequenceOfExtendedString& userComments);

    //! 结束读取注释段
    Standard_EXPORT Storage_Error EndReadCommentSection() Standard_OVERRIDE;

    // ========== 类型段（Type Section）操作函数 ==========

    //! 开始写入类型定义段
    Standard_EXPORT Storage_Error BeginWriteTypeSection() Standard_OVERRIDE;

    //! 设置类型段的大小（对象类型数量）
    Standard_EXPORT void SetTypeSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

    //! 写入单个类型信息（类型编号和名称）
    Standard_EXPORT void WriteTypeInformations(const Standard_Integer typeNum, const TCollection_AsciiString& typeName) Standard_OVERRIDE;

    //! 结束写入类型段
    Standard_EXPORT Storage_Error EndWriteTypeSection() Standard_OVERRIDE;

    //! 开始读取类型段
    Standard_EXPORT Storage_Error BeginReadTypeSection() Standard_OVERRIDE;

    //! 获取类型段的大小（类型数量）
    Standard_EXPORT Standard_Integer TypeSectionSize() Standard_OVERRIDE;

    Standard_EXPORT static Standard_Integer TypeSectionSize(Standard_IStream& theIStream);

    //! 读取单个类型信息
    Standard_EXPORT void ReadTypeInformations(Standard_Integer& typeNum, TCollection_AsciiString& typeName) Standard_OVERRIDE;

    Standard_EXPORT static void ReadTypeInformations(Standard_IStream& theIStream,
        Standard_Integer& typeNum,
        TCollection_AsciiString& typeName);

    //! 结束读取类型段
    Standard_EXPORT Storage_Error EndReadTypeSection() Standard_OVERRIDE;

    // ========== 根对象段（Root Section）操作函数 ==========

    //! 开始写入根对象段
    Standard_EXPORT Storage_Error BeginWriteRootSection() Standard_OVERRIDE;

    //! 设置根对象段的大小（根对象数量）
    Standard_EXPORT void SetRootSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

    //! 写入单个根对象信息（名称、引用编号、类型）
    Standard_EXPORT void WriteRoot(const TCollection_AsciiString& rootName,
        const Standard_Integer aRef,
        const TCollection_AsciiString& aType) Standard_OVERRIDE;

    //! 结束写入根对象段
    Standard_EXPORT Storage_Error EndWriteRootSection() Standard_OVERRIDE;

    //! 开始读取根对象段
    Standard_EXPORT Storage_Error BeginReadRootSection() Standard_OVERRIDE;

    //! 获取根对象段的大小（根对象数量）
    Standard_EXPORT Standard_Integer RootSectionSize() Standard_OVERRIDE;

    Standard_EXPORT static Standard_Integer RootSectionSize(Standard_IStream& theIStream);

    //! 读取单个根对象信息
    Standard_EXPORT void ReadRoot(TCollection_AsciiString& rootName,
        Standard_Integer& aRef,
        TCollection_AsciiString& aType) Standard_OVERRIDE;

    Standard_EXPORT static void ReadRoot(Standard_IStream& theIStream,
        TCollection_AsciiString& rootName,
        Standard_Integer& aRef,
        TCollection_AsciiString& aType);

    //! 结束读取根对象段
    Standard_EXPORT Storage_Error EndReadRootSection() Standard_OVERRIDE;

    // ========== 引用段（Reference Section）操作函数 ==========

    //! 开始写入引用段
    Standard_EXPORT Storage_Error BeginWriteRefSection() Standard_OVERRIDE;

    //! 设置引用段的大小（引用数量）
    Standard_EXPORT void SetRefSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

    //! 写入单个引用信息（引用编号和类型编号）
    Standard_EXPORT void WriteReferenceType(const Standard_Integer reference, const Standard_Integer typeNum) Standard_OVERRIDE;

    //! 结束写入引用段
    Standard_EXPORT Storage_Error EndWriteRefSection() Standard_OVERRIDE;

    //! 开始读取引用段
    Standard_EXPORT Storage_Error BeginReadRefSection() Standard_OVERRIDE;

    //! 获取引用段的大小
    Standard_EXPORT Standard_Integer RefSectionSize() Standard_OVERRIDE;

    Standard_EXPORT static Standard_Integer RefSectionSize(Standard_IStream& theIStream);

    //! 读取单个引用信息
    Standard_EXPORT void ReadReferenceType(Standard_Integer& reference, Standard_Integer& typeNum) Standard_OVERRIDE;

    Standard_EXPORT static void ReadReferenceType(Standard_IStream& theIStream,
        Standard_Integer& reference,
        Standard_Integer& typeNum);

    //! 结束读取引用段
    Standard_EXPORT Storage_Error EndReadRefSection() Standard_OVERRIDE;

    // ========== 数据段（Data Section）操作函数 ==========

    //! 开始写入数据段
    Standard_EXPORT Storage_Error BeginWriteDataSection() Standard_OVERRIDE;

    //! 写入持久化对象头部（引用编号和类型编号）
    Standard_EXPORT void WritePersistentObjectHeader(const Standard_Integer aRef, const Standard_Integer aType) Standard_OVERRIDE;

    //! 开始写入持久化对象数据
    Standard_EXPORT void BeginWritePersistentObjectData() Standard_OVERRIDE;

    //! 开始写入对象具体数据
    Standard_EXPORT void BeginWriteObjectData() Standard_OVERRIDE;

    //! 结束写入对象具体数据
    Standard_EXPORT void EndWriteObjectData() Standard_OVERRIDE;

    //! 结束写入持久化对象数据
    Standard_EXPORT void EndWritePersistentObjectData() Standard_OVERRIDE;

    //! 结束写入数据段
    Standard_EXPORT Storage_Error EndWriteDataSection() Standard_OVERRIDE;

    //! 开始读取数据段
    Standard_EXPORT Storage_Error BeginReadDataSection() Standard_OVERRIDE;

    //! 读取持久化对象头部
    Standard_EXPORT void ReadPersistentObjectHeader(Standard_Integer& aRef, Standard_Integer& aType) Standard_OVERRIDE;

    //! 开始读取持久化对象数据
    Standard_EXPORT void BeginReadPersistentObjectData() Standard_OVERRIDE;

    //! 开始读取对象具体数据
    Standard_EXPORT void BeginReadObjectData() Standard_OVERRIDE;

    //! 结束读取对象具体数据
    Standard_EXPORT void EndReadObjectData() Standard_OVERRIDE;

    //! 结束读取持久化对象数据
    Standard_EXPORT void EndReadPersistentObjectData() Standard_OVERRIDE;

    //! 结束读取数据段
    Standard_EXPORT Storage_Error EndReadDataSection() Standard_OVERRIDE;

    // ========== 数据读写操作函数 ==========

    //! 跳过当前对象数据（用于快速导航）
    Standard_EXPORT void SkipObject() Standard_OVERRIDE;

    //! 写入对象引用（整数值）
    Standard_EXPORT Storage_BaseDriver& PutReference(const Standard_Integer aValue) Standard_OVERRIDE;

    //! 写入字符
    Standard_EXPORT Storage_BaseDriver& PutCharacter(const Standard_Character aValue) Standard_OVERRIDE;

    //! 写入扩展字符（Unicode）
    Standard_EXPORT Storage_BaseDriver& PutExtCharacter(const Standard_ExtCharacter aValue) Standard_OVERRIDE;

    //! 静态函数：写入整数到流
    Standard_EXPORT static Standard_Integer PutInteger(Standard_OStream& theOStream,
        const Standard_Integer aValue,
        const Standard_Boolean theOnlyCount = Standard_False);

    //! 写入整数
    Standard_EXPORT Storage_BaseDriver& PutInteger(const Standard_Integer aValue) Standard_OVERRIDE;

    //! 写入布尔值
    Standard_EXPORT Storage_BaseDriver& PutBoolean(const Standard_Boolean aValue) Standard_OVERRIDE;

    //! 写入双精度实数
    Standard_EXPORT Storage_BaseDriver& PutReal(const Standard_Real aValue) Standard_OVERRIDE;

    //! 写入单精度实数
    Standard_EXPORT Storage_BaseDriver& PutShortReal(const Standard_ShortReal aValue) Standard_OVERRIDE;

    //! 读取对象引用
    Standard_EXPORT Storage_BaseDriver& GetReference(Standard_Integer& aValue) Standard_OVERRIDE;

    //! 读取字符
    Standard_EXPORT Storage_BaseDriver& GetCharacter(Standard_Character& aValue) Standard_OVERRIDE;

    //! 静态函数：从流中读取引用
    Standard_EXPORT static void GetReference(Standard_IStream& theIStream, Standard_Integer& aValue);

    //! 读取扩展字符
    Standard_EXPORT Storage_BaseDriver& GetExtCharacter(Standard_ExtCharacter& aValue) Standard_OVERRIDE;

    //! 读取整数
    Standard_EXPORT Storage_BaseDriver& GetInteger(Standard_Integer& aValue) Standard_OVERRIDE;

    //! 静态函数：从流中读取整数
    Standard_EXPORT static void GetInteger(Standard_IStream& theIStream, Standard_Integer& aValue);

    //! 读取布尔值
    Standard_EXPORT Storage_BaseDriver& GetBoolean(Standard_Boolean& aValue) Standard_OVERRIDE;

    //! 读取双精度实数
    Standard_EXPORT Storage_BaseDriver& GetReal(Standard_Real& aValue) Standard_OVERRIDE;

    //! 读取单精度实数
    Standard_EXPORT Storage_BaseDriver& GetShortReal(Standard_ShortReal& aValue) Standard_OVERRIDE;

    //! 关闭文件
    Standard_EXPORT Storage_Error Close() Standard_OVERRIDE;

    //! 销毁对象，释放资源
    Standard_EXPORT void Destroy();

    ~FSD_BinaryFile()
    {
        Destroy();
    }

public:
    //!@name 字节序转换函数（用于大小端兼容性）

    //! 反转整数的字节顺序（4字节值）
    //! 用于在大小端平台间转换数据
    static Standard_Integer InverseInt(const Standard_Integer theValue)
    {
        return (0 | ((theValue & 0x000000ff) << 24)
            | ((theValue & 0x0000ff00) << 8)
            | ((theValue & 0x00ff0000) >> 8)
            | ((theValue >> 24) & 0x000000ff));
    }

    //! 反转扩展字符的字节顺序（2字节值）
    static Standard_ExtCharacter InverseExtChar(const Standard_ExtCharacter theValue)
    {
        return (0 | ((theValue & 0x00ff) << 8)
            | ((theValue & 0xff00) >> 8));
    }

    //! 反转双精度实数的字节顺序
    Standard_EXPORT static Standard_Real InverseReal(const Standard_Real theValue);

    //! 反转单精度实数的字节顺序
    Standard_EXPORT static Standard_ShortReal InverseShortReal(const Standard_ShortReal theValue);

    //! 反转 Size 类型的字节顺序
    Standard_EXPORT static Standard_Size InverseSize(const Standard_Size theValue);

    //! 反转 64位无符号整数的字节顺序
    Standard_EXPORT static uint64_t InverseUint64(const uint64_t theValue);

    //! 静态函数：从流中读取文件头
    Standard_EXPORT static void ReadHeader(Standard_IStream& theIStream, FSD_FileHeader& theFileHeader);

    //! 静态函数：读取文件头数据（版本、兼容性等）
    Standard_EXPORT static void ReadHeaderData(Standard_IStream& theIStream, const Handle(Storage_HeaderData)& theHeaderData);

    //! 静态函数：从流中读取ASCII字符串
    Standard_EXPORT static void ReadString(Standard_IStream& theIStream, TCollection_AsciiString& buffer);

    //! 静态函数：从流中读取扩展字符串（Unicode）
    Standard_EXPORT static void ReadExtendedString(Standard_IStream& theIStream, TCollection_ExtendedString& buffer);

    //! 静态函数：向流中写入文件头
    Standard_EXPORT static Standard_Integer WriteHeader(Standard_OStream& theOStream,
        const FSD_FileHeader& theHeader,
        const Standard_Boolean theOnlyCount = Standard_False);

    //! 返回文件的魔数（标识字符串）用于检验文件类型
    Standard_EXPORT static Standard_CString MagicNumber();

protected:

    //! 读取指定长度的字符到缓冲区
    Standard_EXPORT void ReadChar(TCollection_AsciiString& buffer, const Standard_Size rsize);

    //! 读取ASCII字符串（直到遇到特定分隔符）
    Standard_EXPORT void ReadString(TCollection_AsciiString& buffer);

    //! 写入ASCII字符串
    Standard_EXPORT void WriteString(const TCollection_AsciiString& buffer);

    //! 静态函数：向流写入字符串
    Standard_EXPORT static Standard_Integer WriteString(Standard_OStream& theOStream,
        const TCollection_AsciiString& theString,
        const Standard_Boolean theOnlyCount = Standard_False);

    //! 读取扩展字符串（Unicode字符串）
    Standard_EXPORT void ReadExtendedString(TCollection_ExtendedString& buffer);

    //! 写入扩展字符串
    Standard_EXPORT void WriteExtendedString(const TCollection_ExtendedString& buffer);

    //! 静态函数：向流写入扩展字符串
    Standard_EXPORT static Standard_Integer WriteExtendedString(Standard_OStream& theOStream,
        const TCollection_ExtendedString& theString,
        const Standard_Boolean theOnlyCount = Standard_False);

private:

    //! 私有函数：向文件写入文件头
    void WriteHeader();

    //! 私有函数：从文件读取文件头
    void ReadHeader();


private:

    FSD_BStream myStream;        // 二进制文件流句柄
    FSD_FileHeader myHeader;     // 文件头结构体，记录各段位置信息
};

#endif // _FSD_BinaryFile_HeaderFile
