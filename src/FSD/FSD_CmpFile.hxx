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

#ifndef _FSD_CmpFile_HeaderFile
#define _FSD_CmpFile_HeaderFile

#include <FSD_File.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_Error.hxx>
#include <Storage_OpenMode.hxx>

class TCollection_AsciiString;
class TCollection_ExtendedString;

DEFINE_STANDARD_HANDLE(FSD_CmpFile, FSD_File)

//! FSD_CmpFile 是压缩文本格式的文件存储驱动程序
//! 继承自 FSD_File，在文本格式的基础上增加压缩功能
//!
//! FSD_CmpFile 与 FSD_File 的区别：
//! - FSD_File：纯文本格式，便于阅读和调试，但文件较大
//! - FSD_CmpFile：压缩文本格式，在保持可读性的同时减小文件大小
//!
//! 主要特点：
//! - 基于文本格式，易于维护版本控制
//! - 对重复的数据和空白进行压缩
//! - 在可读性和文件大小之间找到了平衡
//! - 文件格式包含压缩指示符和元数据
//! - 适合中等大小的数据存储场景
//!
//! 文件结构继承自 FSD_File，但在关键的读写操作中进行了优化和压缩
class FSD_CmpFile : public FSD_File {
public:
    DEFINE_STANDARD_RTTIEXT(FSD_CmpFile, FSD_File)

public:
    //! 构造函数，创建一个压缩文本文件驱动程序实例
    Standard_EXPORT FSD_CmpFile();

    //! 打开或创建压缩文本格式的文件
    //! @param aName 文件路径名
    //! @param aMode 打开模式（读或写）
    //! @return 返回操作状态代码
    Standard_EXPORT Storage_Error Open(const TCollection_AsciiString& aName,
                                       const Storage_OpenMode aMode) Standard_OVERRIDE;

    //! 静态函数：检查指定文件是否为有效的压缩文本格式文件
    //! 通过检查文件的魔数来判断文件类型
    Standard_EXPORT static Storage_Error IsGoodFileType(const TCollection_AsciiString& aName);

    //! 开始写入信息段
    //! 压缩格式会在此处添加压缩相关的元数据
    Standard_EXPORT Storage_Error BeginWriteInfoSection() Standard_OVERRIDE;

    //! 开始读取信息段
    //! 需要先解析压缩相关的元数据
    Standard_EXPORT Storage_Error BeginReadInfoSection() Standard_OVERRIDE;

    //! 写入持久化对象的头部信息
    //! 压缩格式会对头部信息进行压缩编码
    Standard_EXPORT void WritePersistentObjectHeader(const Standard_Integer aRef,
                                                     const Standard_Integer aType) Standard_OVERRIDE;

    //! 开始写入持久化对象的数据
    //! 在压缩格式中启用压缩缓冲
    Standard_EXPORT void BeginWritePersistentObjectData() Standard_OVERRIDE;

    //! 开始写入对象的具体内容数据
    Standard_EXPORT void BeginWriteObjectData() Standard_OVERRIDE;

    //! 结束写入对象的具体内容数据
    Standard_EXPORT void EndWriteObjectData() Standard_OVERRIDE;

    //! 结束写入持久化对象的数据
    //! 在压缩格式中会刷新压缩缓冲并写入压缩数据
    Standard_EXPORT void EndWritePersistentObjectData() Standard_OVERRIDE;

    //! 读取持久化对象的头部信息
    //! 压缩格式会自动解压缩头部信息
    Standard_EXPORT void ReadPersistentObjectHeader(Standard_Integer& aRef, Standard_Integer& aType) Standard_OVERRIDE;

    //! 开始读取持久化对象的数据
    //! 在压缩格式中准备解压缩缓冲
    Standard_EXPORT void BeginReadPersistentObjectData() Standard_OVERRIDE;

    //! 开始读取对象的具体内容数据
    Standard_EXPORT void BeginReadObjectData() Standard_OVERRIDE;

    //! 结束读取对象的具体内容数据
    Standard_EXPORT void EndReadObjectData() Standard_OVERRIDE;

    //! 结束读取持久化对象的数据
    //! 关闭压缩缓冲的解压缩模式
    Standard_EXPORT void EndReadPersistentObjectData() Standard_OVERRIDE;

    //! 销毁对象并释放相关资源（包括压缩缓冲等）
    Standard_EXPORT void Destroy();

    ~FSD_CmpFile() {
        Destroy();
    }

    //! 返回压缩文本格式文件的魔数，用于识别文件类型
    Standard_EXPORT static Standard_CString MagicNumber();

protected:
    //! 读取压缩后的一行文本
    //! 自动进行解压缩处理
    Standard_EXPORT void ReadLine(TCollection_AsciiString& buffer) Standard_OVERRIDE;

    //! 读取压缩后的扩展字符串直到行尾
    //! 自动进行解压缩处理
    Standard_EXPORT void ReadExtendedLine(TCollection_ExtendedString& buffer) Standard_OVERRIDE;

    //! 写入扩展字符串并在压缩格式中进行压缩编码
    Standard_EXPORT void WriteExtendedLine(const TCollection_ExtendedString& buffer) Standard_OVERRIDE;

    //! 读取压缩后的字符串（跳过首位空白符）
    //! 自动进行解压缩处理
    Standard_EXPORT void ReadString(TCollection_AsciiString& buffer) Standard_OVERRIDE;
};

#endif // _FSD_CmpFile_HeaderFile
