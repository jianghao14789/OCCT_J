// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _gp_Pnt_HeaderFile
#define _gp_Pnt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>

class gp_XYZ;
class Standard_OutOfRange;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;
class gp_Vec;

//! Defines a 3D cartesian point.
// 定义一个 3D 笛卡尔坐标点。
//
// CAD 小贴士：gp_Pnt 表示空间中的一个“位置”。
// 虽然它内部也用 X, Y, Z 表示，但它主要用于表示几何实体的顶点、中心等位置信息。
class gp_Pnt {
public:
    DEFINE_STANDARD_ALLOC;

    //! Creates a point with zero coordinates.
    // 创建一个坐标为 (0,0,0) 的点。
    gp_Pnt() {}

    //! Creates a point from a XYZ object.
    // 从 gp_XYZ 对象构造点。
    gp_Pnt(const gp_XYZ& theCoord) : coord(theCoord) {}

    //! Creates a  point with its 3 cartesian's coordinates : theXp, theYp, theZp.
    // 使用给定的 X, Y, Z 坐标构造点。
    gp_Pnt(const Standard_Real theXp, const Standard_Real theYp, const Standard_Real theZp)
        : coord(theXp, theYp, theZp) {}

    //! Changes the coordinate of range theIndex :
    //! theIndex = 1 => X is modified
    //! theIndex = 2 => Y is modified
    //! theIndex = 3 => Z is modified
    //! Raised if theIndex != {1, 2, 3}.
    // 修改指定索引的坐标。
    void SetCoord(const Standard_Integer theIndex, const Standard_Real theXi) {
        coord.SetCoord(theIndex, theXi);
    }

    //! For this point, assigns  the values theXp, theYp and theZp to its three coordinates.
    // 为点重新设置坐标值。
    void SetCoord(const Standard_Real theXp, const Standard_Real theYp, const Standard_Real theZp) {
        coord.SetCoord(theXp, theYp, theZp);
    }

    //! Assigns the given value to the X coordinate of this point.
    // 设置 X 坐标。
    void SetX(const Standard_Real theX) {
        coord.SetX(theX);
    }

    //! Assigns the given value to the Y coordinate of this point.
    // 设置 Y 坐标。
    void SetY(const Standard_Real theY) {
        coord.SetY(theY);
    }

    //! Assigns the given value to the Z coordinate of this point.
    // 设置 Z 坐标。
    void SetZ(const Standard_Real theZ) {
        coord.SetZ(theZ);
    }

    //! Assigns the three coordinates of theCoord to this point.
    // 使用 gp_XYZ 对象为点设置坐标。
    void SetXYZ(const gp_XYZ& theCoord) {
        coord = theCoord;
    }

    //! Returns the coordinate of corresponding to the value of theIndex :
    //! theIndex = 1 => X is returned
    //! theIndex = 2 => Y is returned
    //! theIndex = 3 => Z is returned
    //! Raises OutOfRange if theIndex != {1, 2, 3}.
    //! Raised if theIndex != {1, 2, 3}.
    // 获取指定索引的坐标值。
    Standard_Real Coord(const Standard_Integer theIndex) const {
        return coord.Coord(theIndex);
    }

    //! For this point gives its three coordinates theXp, theYp and theZp.
    // 同时获取点的三个坐标。
    void Coord(Standard_Real& theXp, Standard_Real& theYp, Standard_Real& theZp) const {
        coord.Coord(theXp, theYp, theZp);
    }

    //! For this point, returns its X coordinate.
    Standard_Real X() const {
        return coord.X();
    }

    //! For this point, returns its Y coordinate.
    Standard_Real Y() const {
        return coord.Y();
    }

    //! For this point, returns its Z coordinate.
    Standard_Real Z() const {
        return coord.Z();
    }

    //! For this point, returns its three coordinates as a XYZ object.
    // 返回点的底层 gp_XYZ 坐标对象。
    const gp_XYZ& XYZ() const {
        return coord;
    }

    //! For this point, returns its three coordinates as a XYZ object.
    const gp_XYZ& Coord() const {
        return coord;
    }

    //! Returns the coordinates of this point.
    //! Note: This syntax allows direct modification of the returned value.
    // 返回坐标的引用，允许直接修改内部坐标。
    gp_XYZ& ChangeCoord() {
        return coord;
    }

    //! Assigns the result of the following expression to this point
    //! (theAlpha*this + theBeta*theP) / (theAlpha + theBeta)
    // 计算当前点与点 theP 的加权重心，并将结果存入当前点。
    // 计算公式：(alpha * this + beta * theP) / (alpha + beta)
    void BaryCenter(const Standard_Real theAlpha, const gp_Pnt& theP, const Standard_Real theBeta) {
        coord.SetLinearForm(theAlpha, coord, theBeta, theP.coord);
        coord.Divide(theAlpha + theBeta);
    }

    //! Comparison
    //! Returns True if the distance between the two points is
    //! lower or equal to theLinearTolerance.
    // 检查两点是否相等（即两点间的距离是否在容差范围内）。
    Standard_Boolean IsEqual(const gp_Pnt& theOther, const Standard_Real theLinearTolerance) const {
        return Distance(theOther) <= theLinearTolerance;
    }

    //! Computes the distance between two points.
    // 计算当前点与 points theOther 之间的欧几里得距离。
    Standard_Real Distance(const gp_Pnt& theOther) const;

    //! Computes the square distance between two points.
    // 计算当前点与 theOther 之间距离的平方（计算更快，常用于比较大小）。
    Standard_Real SquareDistance(const gp_Pnt& theOther) const;

    //! Performs the symmetrical transformation of a point
    //! with respect to the point theP which is the center of
    //! the  symmetry.
    // 关于点 theP 进行中心对称变换。
    Standard_EXPORT void Mirror(const gp_Pnt& theP);

    //! Performs the symmetrical transformation of a point
    //! with respect to an axis placement which is the axis
    //! of the symmetry.
    // 返回关于点 theP 对称的新点。
    Standard_NODISCARD Standard_EXPORT gp_Pnt Mirrored(const gp_Pnt& theP) const;

    // 关于轴 theA1 进行轴对称变换。
    Standard_EXPORT void Mirror(const gp_Ax1& theA1);

    //! Performs the symmetrical transformation of a point
    //! with respect to a plane. The axis placement theA2 locates
    //! the plane of the symmetry : (Location, XDirection, YDirection).
    // 返回关于轴 theA1 对称的新点。
    Standard_NODISCARD Standard_EXPORT gp_Pnt Mirrored(const gp_Ax1& theA1) const;

    // 关于面 theA2 进行面对称变换。
    Standard_EXPORT void Mirror(const gp_Ax2& theA2);

    //! Rotates a point. theA1 is the axis of the rotation.
    //! theAng is the angular value of the rotation in radians.
    // 返回关于面 theA2 对称的新点。
    Standard_NODISCARD Standard_EXPORT gp_Pnt Mirrored(const gp_Ax2& theA2) const;

    // 绕轴 theA1 旋转 theAng 弧度。
    void Rotate(const gp_Ax1& theA1, const Standard_Real theAng);

    // 返回旋转后的新点。
    Standard_NODISCARD gp_Pnt Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const {
        gp_Pnt aP = *this;
        aP.Rotate(theA1, theAng);
        return aP;
    }

    //! Scales a point. theS is the scaling value.
    // 以 theP 为中心，进行缩放，缩放系数为 theS。
    void Scale(const gp_Pnt& theP, const Standard_Real theS);

    // 返回缩放后的新点。
    Standard_NODISCARD gp_Pnt Scaled(const gp_Pnt& theP, const Standard_Real theS) const {
        gp_Pnt aPres = *this;
        aPres.Scale(theP, theS);
        return aPres;
    }

    //! Transforms a point with the transformation T.
    // 使用一般的几何变换 T 对点进行变换（包括平移、旋转、缩放等）。
    Standard_EXPORT void Transform(const gp_Trsf& theT);

    // 返回变换后的新点。
    Standard_NODISCARD gp_Pnt Transformed(const gp_Trsf& theT) const {
        gp_Pnt aP = *this;
        aP.Transform(theT);
        return aP;
    }

    //! Translates a point in the direction of the vector theV.
    //! The magnitude of the translation is the vector's magnitude.
    // 沿向量 theV 的方向平移，平移距离为向量的模。
    void Translate(const gp_Vec& theV);

    // 返回平移后的新点。
    Standard_NODISCARD gp_Pnt Translated(const gp_Vec& theV) const;

    //! Translates a point from the point theP1 to the point theP2.
    // 平移点，使其从 theP1 移动到 theP2（相当于平移向量 P2-P1）。
    void Translate(const gp_Pnt& theP1, const gp_Pnt& theP2) {
        coord.Add(theP2.coord);
        coord.Subtract(theP1.coord);
    }

    // 返回平移后的新点。
    Standard_NODISCARD gp_Pnt Translated(const gp_Pnt& theP1, const gp_Pnt& theP2) const {
        gp_Pnt aP = *this;
        aP.Translate(theP1, theP2);
        return aP;
    }

    //! Dumps the content of me into the stream
    Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

    //! Inits the content of me from the stream
    Standard_EXPORT Standard_Boolean InitFromJson(const Standard_SStream& theSStream, Standard_Integer& theStreamPos);

private:
    gp_XYZ coord;
};

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>

//=======================================================================
// function : Distance
// purpose :
//=======================================================================
inline Standard_Real gp_Pnt::Distance(const gp_Pnt& theOther) const {
    Standard_Real aD = 0, aDD;
    const gp_XYZ& aXYZ = theOther.coord;
    aDD = coord.X();
    aDD -= aXYZ.X();
    aDD *= aDD;
    aD += aDD;
    aDD = coord.Y();
    aDD -= aXYZ.Y();
    aDD *= aDD;
    aD += aDD;
    aDD = coord.Z();
    aDD -= aXYZ.Z();
    aDD *= aDD;
    aD += aDD;
    return sqrt(aD);
}

//=======================================================================
// function : SquareDistance
// purpose :
//=======================================================================
inline Standard_Real gp_Pnt::SquareDistance(const gp_Pnt& theOther) const {
    Standard_Real aD = 0, aDD;
    const gp_XYZ& XYZ = theOther.coord;
    aDD = coord.X();
    aDD -= XYZ.X();
    aDD *= aDD;
    aD += aDD;
    aDD = coord.Y();
    aDD -= XYZ.Y();
    aDD *= aDD;
    aD += aDD;
    aDD = coord.Z();
    aDD -= XYZ.Z();
    aDD *= aDD;
    aD += aDD;
    return aD;
}

//=======================================================================
// function : Rotate
// purpose :
//=======================================================================
inline void gp_Pnt::Rotate(const gp_Ax1& theA1, const Standard_Real theAng) {
    gp_Trsf aT;
    aT.SetRotation(theA1, theAng);
    aT.Transforms(coord);
}

//=======================================================================
// function : Scale
// purpose :
//=======================================================================
inline void gp_Pnt::Scale(const gp_Pnt& theP, const Standard_Real theS) {
    gp_XYZ aXYZ = theP.coord;
    aXYZ.Multiply(1.0 - theS);
    coord.Multiply(theS);
    coord.Add(aXYZ);
}

//=======================================================================
// function : Translate
// purpose :
//=======================================================================
inline void gp_Pnt::Translate(const gp_Vec& theV) {
    coord.Add(theV.XYZ());
}

//=======================================================================
// function : Translated
// purpose :
//=======================================================================
inline gp_Pnt gp_Pnt::Translated(const gp_Vec& theV) const {
    gp_Pnt aP = *this;
    aP.coord.Add(theV.XYZ());
    return aP;
}

#endif // _gp_Pnt_HeaderFile
