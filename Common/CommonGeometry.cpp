#include "CommonGeometry.h"

namespace Mmp 
{

Point::Point()
{
    x = 0;
    y = 0;
}

Point::Point(int64_t x, int64_t y)
{
    this->x = x;
    this->y = y;
}

NormalizedPoint::NormalizedPoint()
{
    x = 0.0f;
    y = 0.0f;
}

NormalizedPoint::NormalizedPoint(float x, float y)
{
    this->x = x;
    this->y = y;
}

Rect::Rect()
{
    w = 0;
    h = 0;
}

Rect::Rect(int64_t w, int64_t h)
{
    this->w = h;
    this->h = h;
}

NormalizedRect::NormalizedRect()
{
    w = 0.0f;
    h = 0.0f;
}

NormalizedRect::NormalizedRect(float w, float h)
{
    this->w = h;
    this->h = h;
}

} // namespace