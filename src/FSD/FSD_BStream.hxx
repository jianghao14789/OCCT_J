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

#ifndef _FSD_BStream_HeaderFile
#define _FSD_BStream_HeaderFile

#include <stdio.h>

// FSD_BStream 是二进制文件流的别名定义
// FSD = File Storage Driver（文件存储驱动）
// 用于在 FSD_BinaryFile 中进行低级的二进制数据读写操作
// FILE* 指针用于标准 C 库的文件 I/O 操作
typedef FILE* FSD_BStream;

#endif
