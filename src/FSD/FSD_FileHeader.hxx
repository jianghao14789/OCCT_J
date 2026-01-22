// Created on: 2008-06-20
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

#ifndef _FSD_FileHeader_HeaderFile
#define _FSD_FileHeader_HeaderFile

#include <Standard_PrimitiveTypes.hxx>

//! FSD_FileHeader 结构体定义了存储文件的文件头
//! 其中包含了各个数据段的起始和结束位置（字节偏移量）
//! 使用这个结构体可以快速定位文件中不同类型的数据块
struct FSD_FileHeader {
    Standard_Integer testindian; // 字节序测试标记：用于判断文件是大端还是小端格式

    // 文件信息段（Info Section）：存储数据库版本、创建日期、应用名等元信息
    Standard_Integer binfo; // Info 段的开始位置（字节偏移）
    Standard_Integer einfo; // Info 段的结束位置（字节偏移）

    // 文件注释段（Comment Section）：存储用户自定义的注释信息
    Standard_Integer bcomment; // Comment 段的开始位置
    Standard_Integer ecomment; // Comment 段的结束位置

    // 类型定义段（Type Section）：存储对象类型的名称和编号映射
    Standard_Integer btype; // Type 段的开始位置
    Standard_Integer etype; // Type 段的结束位置

    // 根对象段（Root Section）：存储根对象的名称、引用编号和类型信息
    Standard_Integer broot; // Root 段的开始位置
    Standard_Integer eroot; // Root 段的结束位置

    // 引用类型段（Reference Section）：存储对象间的引用关系和类型号
    Standard_Integer bref; // Reference 段的开始位置
    Standard_Integer eref; // Reference 段的结束位置

    // 数据段（Data Section）：存储实际的对象数据
    Standard_Integer bdata; // Data 段的开始位置
    Standard_Integer edata; // Data 段的结束位置
};

#endif
