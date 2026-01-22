// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _FSD_Base64_HeaderFile
#define _FSD_Base64_HeaderFile

#include <NCollection_Buffer.hxx>
#include <TCollection_AsciiString.hxx>

//! FSD_Base64 是一个Base64编码/解码的工具类
//! Base64编码用于将二进制数据转换为可打印的ASCII字符串
//! 这在文件存储和网络传输中非常有用，特别是处理不支持二进制的文本格式时
//!
//! Base64的特点：
//! - 使用64个可打印字符集：A-Z, a-z, 0-9, +, /
//! - 填充符号：= （用于填充不足3字节的末尾数据）
//! - 编码后的数据大小约为原始数据的 4/3
//! - 每4个字符代表3个字节的原始数据
class FSD_Base64 {
public:
    //! 编码函数 1：将二进制数据编码为Base64字符串（低层接口）
    //! @param[out] theEncodedStr 存储编码后的字符串的缓冲区。
    //!                           如果为NULL，则仅返回所需的字符串长度，用于缓冲区预分配。
    //! @param[in] theStrLen      theEncodedStr缓冲区的长度（字节数）。
    //!                           必须不小于当theEncodedStr为NULL时返回的值。
    //! @param[in] theData        要编码的二进制数据。
    //! @param[in] theDataLen     输入数据的长度（字节数）。
    //! @return 返回编码后的字符串长度，不包括终止空字符。
    //! 如果theStrLen不足以存储所有数据，则不写入任何数据并返回0。
    Standard_EXPORT static Standard_Size Encode(char* theEncodedStr, const Standard_Size theStrLen,
                                                const Standard_Byte* theData, const Standard_Size theDataLen);

    //! 编码函数 2：将二进制数据编码为Base64字符串（高层接口）
    //! 这是一个便利函数，自动处理内存分配。
    //! @param[in] theData     要编码的二进制数据。
    //! @param[in] theDataLen  输入数据的长度（字节数）。
    //! @return 返回包含Base64编码数据的ASCII字符串对象。
    Standard_EXPORT static TCollection_AsciiString Encode(const Standard_Byte* theData, const Standard_Size theDataLen);

    //! 解码函数 1：将Base64字符串解码为二进制数据（低层接口）
    //! @param[out] theDecodedData 存储解码后的二进制数据的缓冲区。
    //!                            如果为NULL，则仅返回所需的数据大小，用于缓冲区预分配。
    //! @param[in] theDataLen      theDecodedData缓冲区的长度（字节数）。
    //!                            必须不小于当theDecodedData为NULL时返回的值。
    //! @param[in] theEncodedStr   Base64编码的输入字符串。
    //! @param[in] theStrLen       输入字符串的长度（字符数）。
    //! @return 返回解码后的数据长度（字节数）。
    //! 如果theDataLen不足以存储所有数据，则不写入任何数据并返回0。
    Standard_EXPORT static Standard_Size Decode(Standard_Byte* theDecodedData, const Standard_Size theDataLen,
                                                Standard_CString theEncodedStr, const Standard_Size theStrLen);

    //! 解码函数 2：将Base64字符串解码为二进制数据（高层接口）
    //! 这是一个便利函数，返回包含解码数据的缓冲区对象。
    //! @param[in] theStr Base64编码的输入字符串。
    //! @param[in] theLen 输入字符串的长度。
    //! @return 返回包含解码数据的缓冲区对象句柄。
    //! 在内存不足的情况下返回NULL句柄。
    Standard_EXPORT static Handle(NCollection_Buffer) Decode(Standard_CString theStr, const Standard_Size theLen);
};

#endif // _FSD_Base64_HeaderFile
