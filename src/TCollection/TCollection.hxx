// Created on: 1992-10-13
// Created by: Ramin BARRETO
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _TCollection_HeaderFile
#define _TCollection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Integer.hxx>

//! The package <TCollection> provides the services for the
//! transient basic data structures.
//! 
//! TCollection 包为瞬时基本数据结构提供服务
//! 
//! 说明：TCollection（Transient Collection，瞬时集合）提供了：
//! - ASCII 和 Extended 字符串处理
//! - 字符串容器和数据结构
//! - 字符串的 Handle 版本（通过引用计数管理）
//! - 字符串搜索、比较、转换等操作
//! 
//! 这是 OCCT 中文本处理的基础模块，广泛用于：
//! - 文件名和路径处理
//! - 消息和日志系统
//! - 数据序列化和存储
//! - 国际化支持（Extended 字符串支持 Unicode）
class TCollection
{
public:

    DEFINE_STANDARD_ALLOC;

    //! Returns a  prime number greater than  <I> suitable
    //! to dimension a Map.  When  <I> becomes great there
    //! is  a  limit on  the  result (today  the  limit is
    //! around 1 000 000). This is not a limit of the number of
    //! items but a limit in the number  of buckets.  i.e.
    //! there will be more collisions  in  the map.
    //! 
    //! 返回大于 <I> 的素数，适合用于调整 Map 的大小
    //! 当 <I> 变大时，结果有一个上限（目前约为 100 万）。
    //! 这不是项目数的限制，而是哈希桶数的限制。
    //! 即，当超过这个值后，会有更多的碰撞在 map 中。
    //! 
    //! 用途：为哈希表选择合适的大小，避免太多哈希碰撞
    //! 示例：
    //!   int mapSize = TCollection::NextPrimeForMap(100);  // 返回 101 或更大
    //!   Map myMap(mapSize);  // 创建足够大小的 Map
    Standard_EXPORT static Standard_Integer NextPrimeForMap(const Standard_Integer I);

};

#endif // _TCollection_HeaderFile
