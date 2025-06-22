#pragma once
#include <vec.h>

#define ZOOM_RATE 0.3

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

    // one tick = a 5th
    inline void zoomAboutPoint(Vec2<long double> point, bool zoom_in) {
        double factor = zoom_in ? ZOOM_RATE : 2-ZOOM_RATE;

        m_width *= factor;
        m_offset = point + (m_offset - point) * factor;;
    }
 };