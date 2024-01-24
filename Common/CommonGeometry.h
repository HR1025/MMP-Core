//
// CommonGeometry.h
//
// Library: Common
// Package: Common
// Module:  Geometry
// 

#pragma once

#include <cstdint>

namespace Mmp 
{

class Point
{
public:
    Point();
    Point(int64_t x, int64_t y);
public:
    int64_t x;
    int64_t y;
};

class NormalizedPoint
{
public:
    NormalizedPoint();
    NormalizedPoint(float x, float y);
public:
    float x;
    float y;
};

class Rect
{
public:
    Rect();
    Rect(int64_t w, int64_t h);
public:
    int64_t w;
    int64_t h;
};

class NormalizedRect
{
public:
    NormalizedRect();
    NormalizedRect(float w, float h);
public:
    float w;
    float h;
};

} // namespace