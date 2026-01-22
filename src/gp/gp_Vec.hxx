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

#ifndef _gp_Vec_HeaderFile
#define _gp_Vec_HeaderFile

#include <gp_VectorWithNullMagnitude.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

class gp_Dir;
class gp_Pnt;
class gp_Ax1;
class gp_Ax2;
class gp_Trsf;

//! Defines a non-persistent vector in 3D space.
// 定义 3D 空间中的向量（矢量）。
//
// CAD 小贴士：gp_Vec 表示空间中的“方向”和“长度”。
// 它不代表一个固定位置，而是代表从一个点到另一个点的“位移”。
// 例如：点 P1 移动 向量 V 之后到达点 P2 (P2 = P1 + V)。
class gp_Vec {
public:
    DEFINE_STANDARD_ALLOC;

    //! Creates a zero vector.
    // 创建一个零向量 (0,0,0)。
    gp_Vec() {}

    //! Creates a unitary vector from a direction theV.
    // 从方向 (gp_Dir) 创建一个单位向量。
    gp_Vec(const gp_Dir& theV);

    //! Creates a vector with a triplet of coordinates.
    // 从 gp_XYZ 坐标组创建向量。
    gp_Vec(const gp_XYZ& theCoord) : coord(theCoord) {}

    //! Creates a point with its three cartesian coordinates.
    // 使用给定的 X, Y, Z 分量创建向量。
    gp_Vec(const Standard_Real theXv, const Standard_Real theYv, const Standard_Real theZv)
        : coord(theXv, theYv, theZv) {}

    //! Creates a vector from two points. The length of the vector
    //! is the distance between theP1 and theP2
    // 创建从点 theP1 指向点 theP2 的向量。向量的模等于两点间的距离。
    gp_Vec(const gp_Pnt& theP1, const gp_Pnt& theP2);

    //! Changes the coordinate of range theIndex
    //! theIndex = 1 => X is modified
    //! theIndex = 2 => Y is modified
    //! theIndex = 3 => Z is modified
    //! Raised if theIndex != {1, 2, 3}.
    // 修改指定索引的分量。
    void SetCoord(const Standard_Integer theIndex, const Standard_Real theXi) {
        coord.SetCoord(theIndex, theXi);
    }

    //! For this vector, assigns
    //! -   the values theXv, theYv and theZv to its three coordinates.
    // 为向量的分量重新赋值。
    void SetCoord(const Standard_Real theXv, const Standard_Real theYv, const Standard_Real theZv) {
        coord.SetX(theXv);
        coord.SetY(theYv);
        coord.SetZ(theZv);
    }

    //! Assigns the given value to the X coordinate of this vector.
    // 设置 X 分量。
    void SetX(const Standard_Real theX) {
        coord.SetX(theX);
    }

    //! Assigns the given value to the X coordinate of this vector.
    // 设置 Y 分量。
    void SetY(const Standard_Real theY) {
        coord.SetY(theY);
    }

    //! Assigns the given value to the X coordinate of this vector.
    // 设置 Z 分量。
    void SetZ(const Standard_Real theZ) {
        coord.SetZ(theZ);
    }

    //! Assigns the three coordinates of theCoord to this vector.
    // 使用 gp_XYZ 对象设置向量分量。
    void SetXYZ(const gp_XYZ& theCoord) {
        coord = theCoord;
    }

    //! Returns the coordinate of range theIndex :
    //! theIndex = 1 => X is returned
    //! theIndex = 2 => Y is returned
    //! theIndex = 3 => Z is returned
    //! Raised if theIndex != {1, 2, 3}.
    // 获取指定索引的分量值。
    Standard_Real Coord(const Standard_Integer theIndex) const {
        return coord.Coord(theIndex);
    }

    //! For this vector returns its three coordinates theXv, theYv, and theZv inline
    // 同时获取向量的三个分量。
    void Coord(Standard_Real& theXv, Standard_Real& theYv, Standard_Real& theZv) const {
        theXv = coord.X();
        theYv = coord.Y();
        theZv = coord.Z();
    }

    //! For this vector, returns its X coordinate.
    Standard_Real X() const {
        return coord.X();
    }

    //! For this vector, returns its Y coordinate.
    Standard_Real Y() const {
        return coord.Y();
    }

    //! For this vector, returns its Z  coordinate.
    Standard_Real Z() const {
        return coord.Z();
    }

    //! For this vector, returns
    //! -   its three coordinates as a number triple
    // 返回向量的底层 gp_XYZ 坐标对象。
    const gp_XYZ& XYZ() const {
        return coord;
    }

    //! Returns True if the two vectors have the same magnitude value
    //! and the same direction. The precision values are theLinearTolerance
    //! for the magnitude and theAngularTolerance for the direction.
    // 检查两个向量是否相等（模和方向都在容差范围内）。
    Standard_EXPORT Standard_Boolean IsEqual(const gp_Vec& theOther, const Standard_Real theLinearTolerance,
                                             const Standard_Real theAngularTolerance) const;

    //! Returns True if abs(<me>.Angle(theOther) - PI/2.) <= theAngularTolerance
    //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
    //! theOther.Magnitude() <= Resolution from gp
    // 检查两个向量是否正交（垂直）。
    Standard_Boolean IsNormal(const gp_Vec& theOther, const Standard_Real theAngularTolerance) const;

    //! Returns True if PI - <me>.Angle(theOther) <= theAngularTolerance
    //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
    //! Other.Magnitude() <= Resolution from gp
    // 检查两个向量是否反向。
    Standard_Boolean IsOpposite(const gp_Vec& theOther, const Standard_Real theAngularTolerance) const {
        Standard_Real anAng = M_PI - Angle(theOther);
        return anAng <= theAngularTolerance;
    }

    //! Returns True if Angle(<me>, theOther) <= theAngularTolerance or
    //! PI - Angle(<me>, theOther) <= theAngularTolerance
    //! This definition means that two parallel vectors cannot define
    //! a plane but two vectors with opposite directions are considered
    //! as parallel. Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution or
    //! Other.Magnitude() <= Resolution from gp
    // 检查两个向量是否平行（同向或反向）。
    Standard_Boolean IsParallel(const gp_Vec& theOther, const Standard_Real theAngularTolerance) const {
        Standard_Real anAng = Angle(theOther);
        return anAng <= theAngularTolerance || M_PI - anAng <= theAngularTolerance;
    }

    //! Computes the angular value between <me> and <theOther>
    //! Returns the angle value between 0 and PI in radian.
    //! Raises VectorWithNullMagnitude if <me>.Magnitude() <= Resolution from gp or
    //! theOther.Magnitude() <= Resolution because the angular value is
    //! indefinite if one of the vectors has a null magnitude.
    // 计算两个向量之间的夹角（弧度，范围 [0, PI]）。
    Standard_Real Angle(const gp_Vec& theOther) const;

    //! Computes the angle, in radians, between this vector and
    //! vector theOther. The result is a value between -Pi and Pi.
    //! For this, theVRef defines the positive sense of rotation: the
    //! angular value is positive, if the cross product this ^ theOther
    //! has the same orientation as theVRef relative to the plane
    //! defined by the vectors this and theOther. Otherwise, the
    //! angular value is negative.
    //! Exceptions
    //! gp_VectorWithNullMagnitude if the magnitude of this
    //! vector, the vector theOther, or the vector theVRef is less than or
    //! equal to gp::Resolution().
    //! Standard_DomainError if this vector, the vector theOther,
    //! and the vector theVRef are coplanar, unless this vector and
    //! the vector theOther are parallel.
    // 计算带参考方向的夹角（结果在 [-PI, PI] 之间）。
    Standard_Real AngleWithRef(const gp_Vec& theOther, const gp_Vec& theVRef) const;

    //! Computes the magnitude of this vector.
    // 计算向量的模（长度）。
    Standard_Real Magnitude() const {
        return coord.Modulus();
    }

    //! Computes the square magnitude of this vector.
    // 计算向量模的平方。
    Standard_Real SquareMagnitude() const {
        return coord.SquareModulus();
    }

    //! Adds two vectors
    // 向量加法。
    void Add(const gp_Vec& theOther) {
        coord.Add(theOther.coord);
    }

    void operator+=(const gp_Vec& theOther) {
        Add(theOther);
    }

    //! Adds two vectors
    // 返回两个向量相加后的新向量。
    Standard_NODISCARD gp_Vec Added(const gp_Vec& theOther) const {
        gp_Vec aV = *this;
        aV.coord.Add(theOther.coord);
        return aV;
    }

    Standard_NODISCARD gp_Vec operator+(const gp_Vec& theOther) const {
        return Added(theOther);
    }

    //! Subtracts two vectors
    // 向量减法。
    void Subtract(const gp_Vec& theRight) {
        coord.Subtract(theRight.coord);
    }

    void operator-=(const gp_Vec& theRight) {
        Subtract(theRight);
    }

    //! Subtracts two vectors
    // 返回两个向量相减后的新向量。
    Standard_NODISCARD gp_Vec Subtracted(const gp_Vec& theRight) const {
        gp_Vec aV = *this;
        aV.coord.Subtract(theRight.coord);
        return aV;
    }

    Standard_NODISCARD gp_Vec operator-(const gp_Vec& theRight) const {
        return Subtracted(theRight);
    }

    //! Multiplies a vector by a scalar
    // 向量乘以标量。
    void Multiply(const Standard_Real theScalar) {
        coord.Multiply(theScalar);
    }

    void operator*=(const Standard_Real theScalar) {
        Multiply(theScalar);
    }

    //! Multiplies a vector by a scalar
    // 返回向量乘以标量后的新向量。
    Standard_NODISCARD gp_Vec Multiplied(const Standard_Real theScalar) const {
        gp_Vec aV = *this;
        aV.coord.Multiply(theScalar);
        return aV;
    }

    Standard_NODISCARD gp_Vec operator*(const Standard_Real theScalar) const {
        return Multiplied(theScalar);
    }

    //! Divides a vector by a scalar
    // 向量除以标量。
    void Divide(const Standard_Real theScalar) {
        coord.Divide(theScalar);
    }

    void operator/=(const Standard_Real theScalar) {
        Divide(theScalar);
    }

    //! Divides a vector by a scalar
    // 返回向量除以标量后的新向量。
    Standard_NODISCARD gp_Vec Divided(const Standard_Real theScalar) const {
        gp_Vec aV = *this;
        aV.coord.Divide(theScalar);
        return aV;
    }

    Standard_NODISCARD gp_Vec operator/(const Standard_Real theScalar) const {
        return Divided(theScalar);
    }

    //! computes the cross product between two vectors
    // 计算当前向量与 theRight 的叉积（矢量积），并更新当前向量。
    void Cross(const gp_Vec& theRight) {
        coord.Cross(theRight.coord);
    }

    void operator^=(const gp_Vec& theRight) {
        Cross(theRight);
    }

    //! computes the cross product between two vectors
    // 返回叉积后的新向量。
    Standard_NODISCARD gp_Vec Crossed(const gp_Vec& theRight) const {
        gp_Vec aV = *this;
        aV.coord.Cross(theRight.coord);
        return aV;
    }

    Standard_NODISCARD gp_Vec operator^(const gp_Vec& theRight) const {
        return Crossed(theRight);
    }

    //! Computes the magnitude of the cross
    //! product between <me> and theRight.
    //! Returns || <me> ^ theRight ||
    // 计算叉积后的模。
    Standard_Real CrossMagnitude(const gp_Vec& theRight) const {
        return coord.CrossMagnitude(theRight.coord);
    }

    //! Computes the square magnitude of
    //! the cross product between <me> and theRight.
    //! Returns || <me> ^ theRight ||**2
    // 计算叉积后的模的平方。
    Standard_Real CrossSquareMagnitude(const gp_Vec& theRight) const {
        return coord.CrossSquareMagnitude(theRight.coord);
    }

    //! Computes the triple vector product.
    //! <me> ^= (theV1 ^ theV2)
    // 三重矢量积。
    void CrossCross(const gp_Vec& theV1, const gp_Vec& theV2) {
        coord.CrossCross(theV1.coord, theV2.coord);
    }

    //! Computes the triple vector product.
    //! <me> ^ (theV1 ^ theV2)
    // 返回三重矢量积后的新向量。
    Standard_NODISCARD gp_Vec CrossCrossed(const gp_Vec& theV1, const gp_Vec& theV2) const {
        gp_Vec aV = *this;
        aV.coord.CrossCross(theV1.coord, theV2.coord);
        return aV;
    }

    //! computes the scalar product
    // 计算两个向量的点积（数量积）。
    Standard_Real Dot(const gp_Vec& theOther) const {
        return coord.Dot(theOther.coord);
    }

    Standard_Real operator*(const gp_Vec& theOther) const {
        return Dot(theOther);
    }

    //! Computes the triple scalar product <me> * (theV1 ^ theV2).
    // 计算混合积（三重标量积）。
    Standard_Real DotCross(const gp_Vec& theV1, const gp_Vec& theV2) const {
        return coord.DotCross(theV1.coord, theV2.coord);
    }

    //! normalizes a vector
    //! Raises an exception if the magnitude of the vector is
    //! lower or equal to Resolution from gp.
    // 归一化：将向量的长度变为 1。
    void Normalize() {
        Standard_Real aD = coord.Modulus();
        Standard_ConstructionError_Raise_if(aD <= gp::Resolution(), "gp_Vec::Normalize() - vector has zero norm");
        coord.Divide(aD);
    }

    //! normalizes a vector
    //! Raises an exception if the magnitude of the vector is
    //! lower or equal to Resolution from gp.
    // 返回归一化后的新向量。
    Standard_NODISCARD gp_Vec Normalized() const;

    //! Reverses the direction of a vector
    // 反向：方向变为相反方向。
    void Reverse() {
        coord.Reverse();
    }

    //! Reverses the direction of a vector
    // 返回反向后的新向量。
    Standard_NODISCARD gp_Vec Reversed() const {
        gp_Vec aV = *this;
        aV.coord.Reverse();
        return aV;
    }

    Standard_NODISCARD gp_Vec operator-() const {
        return Reversed();
    }

    //! <me> is set to the following linear form :
    //! theA1 * theV1 + theA2 * theV2 + theA3 * theV3 + theV4
    // 线性组合形式设置向量。
    void SetLinearForm(const Standard_Real theA1, const gp_Vec& theV1, const Standard_Real theA2, const gp_Vec& theV2,
                       const Standard_Real theA3, const gp_Vec& theV3, const gp_Vec& theV4) {
        coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord, theA3, theV3.coord, theV4.coord);
    }

    //! <me> is set to the following linear form :
    //! theA1 * theV1 + theA2 * theV2 + theA3 * theV3
    void SetLinearForm(const Standard_Real theA1, const gp_Vec& theV1, const Standard_Real theA2, const gp_Vec& theV2,
                       const Standard_Real theA3, const gp_Vec& theV3) {
        coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord, theA3, theV3.coord);
    }

    //! <me> is set to the following linear form :
    //! theA1 * theV1 + theA2 * theV2 + theV3
    void SetLinearForm(const Standard_Real theA1, const gp_Vec& theV1, const Standard_Real theA2, const gp_Vec& theV2,
                       const gp_Vec& theV3) {
        coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord, theV3.coord);
    }

    //! <me> is set to the following linear form :
    //! theA1 * theV1 + theA2 * theV2
    void SetLinearForm(const Standard_Real theA1, const gp_Vec& theV1, const Standard_Real theA2, const gp_Vec& theV2) {
        coord.SetLinearForm(theA1, theV1.coord, theA2, theV2.coord);
    }

    //! <me> is set to the following linear form : theA1 * theV1 + theV2
    void SetLinearForm(const Standard_Real theA1, const gp_Vec& theV1, const gp_Vec& theV2) {
        coord.SetLinearForm(theA1, theV1.coord, theV2.coord);
    }

    //! <me> is set to the following linear form : theV1 + theV2
    void SetLinearForm(const gp_Vec& theV1, const gp_Vec& theV2) {
        coord.SetLinearForm(theV1.coord, theV2.coord);
    }

    // 关于向量 theV 进行镜像对称。
    Standard_EXPORT void Mirror(const gp_Vec& theV);

    //! Performs the symmetrical transformation of a vector
    //! with respect to the vector theV which is the center of
    //! the  symmetry.
    // 返回镜像后的新向量。
    Standard_NODISCARD Standard_EXPORT gp_Vec Mirrored(const gp_Vec& theV) const;

    // 关于轴 theA1 进行对称。
    Standard_EXPORT void Mirror(const gp_Ax1& theA1);

    //! Performs the symmetrical transformation of a vector
    //! with respect to an axis placement which is the axis
    //! of the symmetry.
    Standard_NODISCARD Standard_EXPORT gp_Vec Mirrored(const gp_Ax1& theA1) const;

    // 关于面 theA2 进行对称。
    Standard_EXPORT void Mirror(const gp_Ax2& theA2);

    //! Performs the symmetrical transformation of a vector
    //! with respect to a plane. The axis placement theA2 locates
    //! the plane of the symmetry : (Location, XDirection, YDirection).
    Standard_NODISCARD Standard_EXPORT gp_Vec Mirrored(const gp_Ax2& theA2) const;

    // 绕轴 theA1 旋转。
    void Rotate(const gp_Ax1& theA1, const Standard_Real theAng);

    //! Rotates a vector. theA1 is the axis of the rotation.
    //! theAng is the angular value of the rotation in radians.
    // 返回旋转后的新向量。
    Standard_NODISCARD gp_Vec Rotated(const gp_Ax1& theA1, const Standard_Real theAng) const {
        gp_Vec aVres = *this;
        aVres.Rotate(theA1, theAng);
        return aVres;
    }

    // 标量缩放。
    void Scale(const Standard_Real theS) {
        coord.Multiply(theS);
    }

    //! Scales a vector. theS is the scaling value.
    // 返回缩放后的新向量。
    Standard_NODISCARD gp_Vec Scaled(const Standard_Real theS) const {
        gp_Vec aV = *this;
        aV.coord.Multiply(theS);
        return aV;
    }

    //! Transforms a vector with the transformation theT.
    // 变换向量（注意：向量变换不包含平移分量，因为向量是相对的）。
    Standard_EXPORT void Transform(const gp_Trsf& theT);

    //! Transforms a vector with the transformation theT.
    Standard_NODISCARD gp_Vec Transformed(const gp_Trsf& theT) const {
        gp_Vec aV = *this;
        aV.Transform(theT);
        return aV;
    }

    //! Dumps the content of me into the stream
    Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
    gp_XYZ coord;
};

#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

//=======================================================================
// function :  gp_Vec
// purpose :
//=======================================================================
inline gp_Vec::gp_Vec(const gp_Dir& theV) {
    coord = theV.XYZ();
}

//=======================================================================
// function :  gp_Vec
// purpose :
//=======================================================================
inline gp_Vec::gp_Vec(const gp_Pnt& theP1, const gp_Pnt& theP2) {
    coord = theP2.XYZ().Subtracted(theP1.XYZ());
}

//=======================================================================
// function :  IsNormal
// purpose :
//=======================================================================
inline Standard_Boolean gp_Vec::IsNormal(const gp_Vec& theOther, const Standard_Real theAngularTolerance) const {
    Standard_Real anAng = M_PI / 2.0 - Angle(theOther);
    if (anAng < 0) {
        anAng = -anAng;
    }
    return anAng <= theAngularTolerance;
}

//=======================================================================
// function :  Angle
// purpose :
//=======================================================================
inline Standard_Real gp_Vec::Angle(const gp_Vec& theOther) const {
    gp_VectorWithNullMagnitude_Raise_if(
        coord.Modulus() <= gp::Resolution() || theOther.coord.Modulus() <= gp::Resolution(), " ");
    return (gp_Dir(coord)).Angle(theOther);
}

//=======================================================================
// function :  AngleWithRef
// purpose :
//=======================================================================
inline Standard_Real gp_Vec::AngleWithRef(const gp_Vec& theOther, const gp_Vec& theVRef) const {
    gp_VectorWithNullMagnitude_Raise_if(coord.Modulus() <= gp::Resolution() ||
                                            theVRef.coord.Modulus() <= gp::Resolution() ||
                                            theOther.coord.Modulus() <= gp::Resolution(),
                                        " ");
    return (gp_Dir(coord)).AngleWithRef(theOther, theVRef);
}

//=======================================================================
// function :  Normalized
// purpose :
//=======================================================================
inline gp_Vec gp_Vec::Normalized() const {
    Standard_Real aD = coord.Modulus();
    Standard_ConstructionError_Raise_if(aD <= gp::Resolution(), "gp_Vec::Normalized() - vector has zero norm");
    gp_Vec aV = *this;
    aV.coord.Divide(aD);
    return aV;
}

//=======================================================================
// function :  Rotate
// purpose :
//=======================================================================
inline void gp_Vec::Rotate(const gp_Ax1& theA1, const Standard_Real theAng) {
    gp_Trsf aT;
    aT.SetRotation(theA1, theAng);
    coord.Multiply(aT.VectorialPart());
}

//=======================================================================
// function :  operator*
// purpose :
//=======================================================================
inline gp_Vec operator*(const Standard_Real theScalar, const gp_Vec& theV) {
    return theV.Multiplied(theScalar);
}

#endif // _gp_Vec_HeaderFile
