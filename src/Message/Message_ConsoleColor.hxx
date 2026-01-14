// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Message_ConsoleColor_HeaderFile
#define _Message_ConsoleColor_HeaderFile

//! Color definition for console/terminal output (limited palette).
//! 控制台/终端输出的颜色定义（有限的调色板）
enum Message_ConsoleColor
{
    Message_ConsoleColor_Default, //!< default (white) color
                                  //!< 默认（白色）颜色
    Message_ConsoleColor_Black,   //!< black   color
                                  //!< 黑色
    Message_ConsoleColor_White,   //!< white   color
                                  //!< 白色
    Message_ConsoleColor_Red,     //!< red     color
                                  //!< 红色
    Message_ConsoleColor_Blue,    //!< blue    color
                                  //!< 蓝色
    Message_ConsoleColor_Green,   //!< green   color
                                  //!< 绿色
    Message_ConsoleColor_Yellow,  //!< yellow  color
                                  //!< 黄色
    Message_ConsoleColor_Cyan,    //!< cyan    color
                                  //!< 青色
    Message_ConsoleColor_Magenta, //!< magenta color
                                  //!< 洋红色
};

#endif // _Message_ConsoleColor_HeaderFile
