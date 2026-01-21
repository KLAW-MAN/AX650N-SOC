/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#pragma once

#ifndef __cplusplus
#  error types.hpp header must be compiled as C++
#endif

#include <climits>
#include <cfloat>
#include <vector>
#include <limits>

namespace axclite
{
template<typename _Tp> static inline _Tp saturate_cast(unsigned char v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(signed char v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(unsigned short v)   { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(short v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(unsigned v) { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(int v)      { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(float v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(double v)   { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(int64_t v)    { return _Tp(v); }
/** @overload */
template<typename _Tp> static inline _Tp saturate_cast(uint64_t v)   { return _Tp(v); }

////////////////////////////////// Rect /////////////////////////////////
template<typename _Tp> class Rect_
{
public:
    typedef _Tp value_type;

    //! default constructor
    Rect_();
    Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height);

#if 0
    Rect_(const Rect_& r) = default;
    Rect_(Rect_&& r) CV_NOEXCEPT = default;
#endif
#if 0
    Rect_& operator = (const Rect_& r) = default;
    Rect_& operator = (Rect_&& r) CV_NOEXCEPT = default;
#endif

    //! area (width*height) of the rectangle
    _Tp area() const;

    //! true if empty
    bool empty() const;

    template<typename _Tp2> operator Rect_<_Tp2>() const;

    _Tp x; //!< x coordinate of the top-left corner
    _Tp y; //!< y coordinate of the top-left corner
    _Tp width; //!< width of the rectangle
    _Tp height; //!< height of the rectangle
};

typedef Rect_<int> Rect2i;
typedef Rect_<float> Rect2f;
typedef Rect_<double> Rect2d;
typedef Rect2i Rect;

template<typename _Tp> inline
Rect_<_Tp>::Rect_()
    : x(0), y(0), width(0), height(0) {}

template<typename _Tp> inline
Rect_<_Tp>::Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height)
    : x(_x), y(_y), width(_width), height(_height) {}

template<typename _Tp> inline
_Tp Rect_<_Tp>::area() const
{
    const _Tp result = width * height;

    return result;
}

template<typename _Tp> inline
bool Rect_<_Tp>::empty() const
{
    return width <= 0 || height <= 0;
}

template<typename _Tp> template<typename _Tp2> inline
Rect_<_Tp>::operator Rect_<_Tp2>() const
{
    return Rect_<_Tp2>(saturate_cast<_Tp2>(x), saturate_cast<_Tp2>(y), saturate_cast<_Tp2>(width), saturate_cast<_Tp2>(height));
}

template<typename _Tp> static inline
Rect_<_Tp>& operator &= ( Rect_<_Tp>& a, const Rect_<_Tp>& b )
{
    if (a.empty() || b.empty()) {
        a = Rect();
        return a;
    }
    const Rect_<_Tp>& Rx_min = (a.x < b.x) ? a : b;
    const Rect_<_Tp>& Rx_max = (a.x < b.x) ? b : a;
    const Rect_<_Tp>& Ry_min = (a.y < b.y) ? a : b;
    const Rect_<_Tp>& Ry_max = (a.y < b.y) ? b : a;
    // Looking at the formula below, we will compute Rx_min.width - (Rx_max.x - Rx_min.x)
    // but we want to avoid overflows. Rx_min.width >= 0 and (Rx_max.x - Rx_min.x) >= 0
    // by definition so the difference does not overflow. The only thing that can overflow
    // is (Rx_max.x - Rx_min.x). And it can only overflow if Rx_min.x < 0.
    // Let us first deal with the following case.
    if ((Rx_min.x < 0 && Rx_min.x + Rx_min.width < Rx_max.x) ||
        (Ry_min.y < 0 && Ry_min.y + Ry_min.height < Ry_max.y)) {
        a = Rect();
        return a;
    }
    // We now know that either Rx_min.x >= 0, or
    // Rx_min.x < 0 && Rx_min.x + Rx_min.width >= Rx_max.x and therefore
    // Rx_min.width >= (Rx_max.x - Rx_min.x) which means (Rx_max.x - Rx_min.x)
    // is inferior to a valid int and therefore does not overflow.
    a.width = std::min(Rx_min.width - (Rx_max.x - Rx_min.x), Rx_max.width);
    a.height = std::min(Ry_min.height - (Ry_max.y - Ry_min.y), Ry_max.height);
    a.x = Rx_max.x;
    a.y = Ry_max.y;
    if (a.empty())
        a = Rect();
    return a;
}

template<typename _Tp> static inline
Rect_<_Tp>& operator |= ( Rect_<_Tp>& a, const Rect_<_Tp>& b )
{
    if (a.empty()) {
        a = b;
    }
    else if (!b.empty()) {
        _Tp x1 = std::min(a.x, b.x);
        _Tp y1 = std::min(a.y, b.y);
        a.width = std::max(a.x + a.width, b.x + b.width) - x1;
        a.height = std::max(a.y + a.height, b.y + b.height) - y1;
        a.x = x1;
        a.y = y1;
    }
    return a;
}

template<typename _Tp> static inline
bool operator == (const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

template<typename _Tp> static inline
bool operator != (const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    return a.x != b.x || a.y != b.y || a.width != b.width || a.height != b.height;
}

template<typename _Tp> static inline
Rect_<_Tp> operator & (const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    Rect_<_Tp> c = a;
    return c &= b;
}

template<typename _Tp> static inline
Rect_<_Tp> operator | (const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    Rect_<_Tp> c = a;
    return c |= b;
}
} // axclite
