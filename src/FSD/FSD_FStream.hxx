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

#ifndef _FSD_FStream_HeaderFile
#define _FSD_FStream_HeaderFile

#include <stdio.h>
#include <Standard_Stream.hxx>

// FSD_FStream 是格式化文件流的别名定义
// FSD = File Storage Driver（文件存储驱动）
// 用于在 FSD_File（文本格式文件）中进行格式化数据的读写操作
// std::fstream 是 C++ 标准库提供的双向文件流类，支持读写操作
typedef std::fstream FSD_FStream;

#endif
