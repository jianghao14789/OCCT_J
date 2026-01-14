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

#include <FSD_Base64.hxx>

// =======================================================================
// function : Encode (编码函数)
// =======================================================================
// 编码函数：将二进制数据转换为Base64字符串
// Base64编码原理：每3个字节（24位）转换为4个Base64字符（每个6位）
Standard_Size FSD_Base64::Encode(char* theEncodedStr,
    const Standard_Size theStrLen,
    const Standard_Byte* theData,
    const Standard_Size theDataLen)
{
    // Base64字符集：包含大小写字母、数字和+/两个特殊字符
    // 共64个字符（0-63对应到字符集中的位置）
    static const char aBase64Chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    // 如果输入数据为空，返回0
    if (theDataLen == 0)
    {
        return 0;
    }

    // 计算填充字节数：输入数据长度模3的余数
    // aPad 的含义：
    //   0：输入数据长度是3的倍数，不需要填充
    //   1：输入数据最后有1个字节，需要填充2个'='
    //   2：输入数据最后有2个字节，需要填充1个'='
    Standard_Size aPad = theDataLen % 3;
    
    // 计算编码后的字符串长度
    // 公式：4 * ceil(dataLen / 3) = 4 * ((dataLen + 2) / 3)
    // 示例：
    //   dataLen=1: 4 * ((1+2)/3) = 4 * 1 = 4 字符（1字节+3个填充）
    //   dataLen=2: 4 * ((2+2)/3) = 4 * 1 = 4 字符（2字节+2个填充）
    //   dataLen=3: 4 * ((3+2)/3) = 4 * 1 = 4 字符（3字节+0个填充）
    //   dataLen=4: 4 * ((4+2)/3) = 4 * 2 = 8 字符（3字节+1个4字节组+1个填充）
    const Standard_Size aSize64 = 4 * ((theDataLen + 2) / 3);
    
    // 如果目标缓冲区为NULL，仅返回所需的字符串长度
    // 这允许调用者先获取所需的缓冲区大小，然后分配足够的空间
    if (theEncodedStr == NULL)
    {
        return aSize64;
    }
    
    // 如果目标缓冲区空间不足，返回0表示失败
    if (aSize64 > theStrLen)
    {
        return 0;
    }

    // 编码主循环：每次处理3个输入字节，产生4个Base64字符
    // Base64编码步骤：
    // 1. 将3个字节（24位）组合成一个整数
    // 2. 从高位到低位依次提取6位，作为字符集的索引
    // 3. 从字符集中查找并输出对应的Base64字符
    Standard_Size iStr = 0;
    for (Standard_Size i = 0; i < theDataLen - aPad; i += 3)
    {
        // 将3个字节组合成一个32位整数（仅使用低24位）
        // 位布局：[第1字节(8位)][第2字节(8位)][第3字节(8位)]
        // 在32位整数中的位置：
        //   第1字节: 位23-16
        //   第2字节: 位15-8
        //   第3字节: 位7-0
        uint32_t aWord = (uint32_t(theData[i]) << 16) +
            (uint32_t(theData[i + 1]) << 8) +
            theData[i + 2];
        
        // 从高到低提取6位作为索引，转换为Base64字符
        // 6位能表示的范围是 0-63，对应Base64字符集的所有字符
        
        // 右移18位：获得位23-18（第一组6位）
        // Base64字符1来自最高的6位
        theEncodedStr[iStr++] = aBase64Chars[aWord >> 18];
        
        // 右移12位并掩码0x3F（6位）：获得位17-12（第二组6位）
        // 掩码0x3F = 0b111111，确保只取低6位
        theEncodedStr[iStr++] = aBase64Chars[aWord >> 12 & 0x3F];
        
        // 右移6位并掩码0x3F：获得位11-6（第三组6位）
        theEncodedStr[iStr++] = aBase64Chars[aWord >> 6 & 0x3F];
        
        // 掩码0x3F：获得位5-0（第四组6位）
        theEncodedStr[iStr++] = aBase64Chars[aWord & 0x3F];
    }
    
    // 处理填充字节（不足3字节的剩余数据）
    // 这部分需要特殊处理，因为最后的1或2个字节不足以填满6位×4的空间
    if (aPad-- != 0)  // aPad 从 1 或 2 递减到 0 或 1
    {
        // 如果还有2个剩余字节（aPad 原始值为2）
        if (aPad != 0)
        {
            // 将最后2个字节组合成一个16位整数
            // 位布局：[倒数第2字节(8位)][倒数第1字节(8位)]
            uint32_t aWord = uint32_t(theData[theDataLen - 2]) << 8 | theData[theDataLen - 1];
            
            // 处理2个字节的编码：16位 -> 3个Base64字符 + 1个'='
            
            // 第1个Base64字符：16位右移10位 = 位15-10（最高6位）
            theEncodedStr[iStr++] = aBase64Chars[aWord >> 10];
            
            // 第2个Base64字符：右移4位并掩码0x03F = 位9-4
            // 注意：0x03F 与 0x3F 相同（两种写法）
            theEncodedStr[iStr++] = aBase64Chars[aWord >> 4 & 0x03F];
            
            // 第3个Base64字符：最低4位左移2位 = 位3-0加2个零位
            // 2个字节只能提供16位，所以最后6位中有2位是零填充
            // (aWord & 0xF) 获取最低4位
            // 左移2位让这4位变成位3-6，剩余低2位为0
            theEncodedStr[iStr++] = aBase64Chars[(aWord & 0xF) << 2];
            
            // 第4个位置填充'='，表示有1个字节的有效数据被填充
        }
        // 如果只有1个剩余字节（aPad 原始值为1）
        else
        {
            // 处理1个字节的编码：8位 -> 2个Base64字符 + 2个'='
            
            // 处理最后1个字节
            uint32_t aWord = theData[theDataLen - 1];
            
            // 第1个Base64字符：右移2位 = 位7-2（最高6位）
            theEncodedStr[iStr++] = aBase64Chars[aWord >> 2];
            
            // 第2个Base64字符：最低2位左移4位 = 位1-0加4个零位
            // 1个字节只能提供8位，所以后续6位中有4位是零填充
            theEncodedStr[iStr++] = aBase64Chars[(aWord & 3) << 4];
            
            // 第3个位置填充'='
            theEncodedStr[iStr++] = '=';
        }
    }
    
    // 用'='填充剩余位置到预期长度
    // 根据RFC 4648，Base64编码后的长度必须是4的倍数
    // 用'='字符作为填充符号
    while (iStr < aSize64)
    {
        theEncodedStr[iStr++] = '=';
    }
    
    return aSize64;
}

// =======================================================================
// function : Encode (重载版本)
// =======================================================================
// 便利函数：直接返回编码后的字符串对象
// 这个版本自动处理内存分配，调用者无需关心缓冲区管理
TCollection_AsciiString FSD_Base64::Encode(const Standard_Byte* theData,
    const Standard_Size theDataLen)
{
    // 第一步：调用编码函数，参数为NULL以获取所需缓冲区长度
    // 这个调用只计算大小，不进行实际的编码
    Standard_Size aStrLen = Encode(NULL, 0, theData, theDataLen);
    
    // 第二步：创建指定长度的字符串对象
    // TCollection_AsciiString 构造函数的第二个参数 0 表示初始内容为空
    TCollection_AsciiString aStr((Standard_Integer)aStrLen, 0);
    
    // 第三步：调用编码函数，实际将数据写入字符串缓冲区
    // const_cast 用于移除 const 限定，因为 ToCString() 返回 const 指针
    Encode(const_cast<char*>(aStr.ToCString()), aStrLen, theData, theDataLen);
    
    return aStr;
}

// =======================================================================
// function : Decode (解码函数)
// =======================================================================
// 解码函数：将Base64字符串转换回二进制数据
// Base64解码是编码的逆过程
Standard_Size FSD_Base64::Decode(Standard_Byte* theDecodedData,
    const Standard_Size theDataLen,
    Standard_CString theEncodedStr,
    const Standard_Size theStrLen)
{
    // Base64解码查找表：将ASCII字符映射到6位值（0-63）
    // 查找表的构建规则：
    //   位置 65-90 ('A'-'Z')：值 0-25
    //   位置 97-122 ('a'-'z')：值 26-51
    //   位置 48-57 ('0'-'9')：值 52-61
    //   位置 43 ('+'）：值 62
    //   位置 47 ('/')：值 63
    //   其他位置：值 0（无效字符）
    static const Standard_Byte aBase64Codes[256] =
    {
       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
      56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
       7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
       0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
      41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };
    
    // 如果输入字符串为空，返回0
    if (theStrLen == 0)
    {
        return 0;
    }

    // 计算填充字节数和有效数据长度
    // aPad 的含义：
    //   0：字符串长度是4的倍数，且没有'='填充字符
    //   1：字符串长度不是4的倍数，或最后一个字符是'='
    // 条件分析：
    //   (theStrLen % 4)：如果字符串长度不是4的倍数，值为非0，逻辑与运算结果为 true（转为1）
    //   theEncodedStr[theStrLen - 1] == '='：如果最后一个字符是'='，值为 true（转为1）
    //   两个条件的逻辑或：只要满足其一就返回 1，都不满足返回 0
    uint32_t aPad(theStrLen % 4 || theEncodedStr[theStrLen - 1] == '=');
    
    // aNbIter：计算完整的4字符组的字节数（不包含填充部分）
    // 公式：((len + 3) / 4 - aPad) * 4
    // 示例：
    //   theStrLen=4, aPad=0: ((4+3)/4 - 0)*4 = (1-0)*4 = 4
    //   theStrLen=4, aPad=1: ((4+3)/4 - 1)*4 = (1-1)*4 = 0
    //   theStrLen=8, aPad=0: ((8+3)/4 - 0)*4 = (2-0)*4 = 8
    // 这相当于计算去掉最后一个不完整4字符组后的长度
    const Standard_Size aNbIter = ((theStrLen + 3) / 4 - aPad) * 4;
    
    // 如果倒数第三个字符不是'='，说明还有额外的填充字节需要处理
    // 这处理了编码器在填充时的特殊情况
    if (theStrLen > aNbIter + 2 && theEncodedStr[aNbIter + 2] != '=')
    {
        ++aPad;
    }

    // 计算解码后的数据大小
    // 完整组解码：每4个字符 -> 3个字节
    //   aNbIter / 4 个完整组，每组产生3字节 = aNbIter / 4 * 3 字节
    // 填充部分：aPad个字符 -> (aPad-1)个字节
    //   1个字符：0字节（只有'='，没有有效数据）
    //   2个字符：1字节（1个Base64字符 + 1个'='）
    //   3个字符：2字节（2个Base64字符 + 1个'='）
    // 由于 aPad 表示的是有'='的字符数，实际有效字节 = aPad - 1
    // 但为了简化，公式中直接用 aPad（已包含边界情况的处理）
    const Standard_Size aDecodedSize = aNbIter / 4 * 3 + aPad;
    
    // 如果目标缓冲区为NULL，仅返回所需的数据大小
    if (theDecodedData == NULL)
    {
        return aDecodedSize;
    }
    
    // 如果目标缓冲区空间不足，返回0表示失败
    if (aDecodedSize > theDataLen)
    {
        return 0;
    }

    // 解码主循环：每次处理4个Base64字符，产生3个字节
    // Base64解码步骤：
    // 1. 获取4个Base64字符的值（通过查找表）
    // 2. 将4个6位值组合成一个24位整数
    // 3. 从高位到低位依次提取3个字节
    for (Standard_Size i = 0; i < aNbIter; i += 4)
    {
        // 将4个Base64字符转换为4个6位值，组合成一个32位整数
        // 位布局：[第1字符(6位)<<18][第2字符(6位)<<12][第3字符(6位)<<6][第4字符(6位)]
        // 结果在32位整数中的位置：
        //   第1字符：位23-18
        //   第2字符：位17-12
        //   第3字符：位11-6
        //   第4字符：位5-0
        unsigned aWord = (aBase64Codes[unsigned(theEncodedStr[i])] << 18) +
            (aBase64Codes[unsigned(theEncodedStr[i + 1])] << 12) +
            (aBase64Codes[unsigned(theEncodedStr[i + 2])] << 6) +
            aBase64Codes[unsigned(theEncodedStr[i + 3])];
        
        // 从高到低提取3个字节
        // 第1字节：位23-16（最高字节）
        *theDecodedData++ = static_cast<Standard_Byte>(aWord >> 16);
        
        // 第2字节：位15-8，掩码0xFF确保只取低8位
        *theDecodedData++ = static_cast<Standard_Byte>(aWord >> 8 & 0xFF);
        
        // 第3字节：位7-0，掩码0xFF
        *theDecodedData++ = static_cast<Standard_Byte>(aWord & 0xFF);
    }

    // 处理填充部分（少于4个字符的剩余数据）
    if (aPad > 0)
    {
        // 组合第1和第2个Base64字符
        // 这部分处理最后不足4个字符的情况
        unsigned aWord = (aBase64Codes[unsigned(theEncodedStr[aNbIter])] << 18) +
            (aBase64Codes[unsigned(theEncodedStr[aNbIter + 1])] << 12);
        
        // 输出第1个字节
        // 从第1和第2个字符的组合中，总能提取出至少1字节的有效数据
        *theDecodedData++ = static_cast<Standard_Byte> (aWord >> 16);

        // 如果还有额外的字符（第3个），输出第2个字节
        if (aPad > 1)
        {
            // 加入第3个Base64字符
            // 第3个字符提供额外的4位数据
            aWord += (aBase64Codes[unsigned(theEncodedStr[aNbIter + 2])] << 6);
            
            // 输出第2字节：位15-8
            *theDecodedData++ = static_cast<Standard_Byte> (aWord >> 8 & 0xFF);
        }
    }
    
    return aDecodedSize;
}

// =======================================================================
// function : Decode (重载版本)
// =======================================================================
// 便利函数：直接返回解码后的缓冲区对象
// 这个版本自动处理内存分配和数据解码
Handle(NCollection_Buffer) FSD_Base64::Decode(Standard_CString theEncodedStr,
    const Standard_Size theStrLen)
{
    // 第一步：调用解码函数，参数为NULL以获取所需缓冲区大小
    // 这个调用只计算大小，不进行实际的解码
    const Standard_Size aDataSize = Decode(NULL, 0, theEncodedStr, theStrLen);
    
    // 第二步：创建新的缓冲区对象
    // NCollection_Buffer 是 OCCT 中的动态字节缓冲区
    // CommonBaseAllocator 是通用的内存分配器
    Handle(NCollection_Buffer) aBuf = new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator());
    
    // 如果解码大小为0（输入为空或解析失败），返回空缓冲区
    if (aDataSize == 0)
    {
        return aBuf;
    }
    
    // 第三步：为缓冲区分配所需的内存大小
    // 如果内存分配失败，返回NULL句柄
    if (!aBuf->Allocate(aDataSize))
    {
        return Handle(NCollection_Buffer)();
    }
    
    // 第四步：调用解码函数，实际将数据写入缓冲区
    // ChangeData() 获取缓冲区的可写指针
    Decode(aBuf->ChangeData(), aDataSize, theEncodedStr, theStrLen);
    
    return aBuf;
}
