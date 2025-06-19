#pragma once
#include <color.h>
#include <rsrc/vec.h>

constexpr Color mandelbrot_eval(const Vec2<long double>& point, const Vec2<long double>& offset = {0.0, 0.0}, long double breakout = 4, uint16_t iterations = 25) {
    long double x = point.x + offset.x, y = point.y + offset.y;
    long double new_x;
    uint16_t i = 1;
    while (x*x + y * y < breakout) {
        if (i == iterations) {
            return {255, 255, 255};
        }
        new_x = x*x - y*y + point.x;
        y = 2 * x * y + point.y;
        x = new_x;
        i++;
    }
    return fromColormap(i, iterations);
}