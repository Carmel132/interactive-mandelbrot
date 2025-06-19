#pragma once
#include <vec.h>

// Stores coordinate box thats maintains proportion
struct Viewport {
    // the width of the box
    long double m_width;
    // the ration of height to width
    long double m_ratio;
    // offset in absolute coordinates
    Vec2<long double> m_offset;

    inline Vec2<long double> fromScreenCoord(int x, int y, int w, int h) const {
        long double ret_width = m_width * x / w;
        long double ret_height = -(m_width * m_ratio) * y / h;
        return m_offset + Vec2<long double>{ret_width, ret_height};
    }
 };