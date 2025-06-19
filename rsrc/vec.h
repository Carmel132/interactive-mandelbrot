#pragma once
#include <iostream>
#include <concepts>

template<typename T> requires std::floating_point<T>
struct Vec2 {
    T x, y;
    Vec2(T _x, T _y) : x{ _x }, y{ _y } {}
    Vec2() : Vec2<T>(0, 0) {}
    Vec2 operator +(const Vec2<T>& other) const {
        return Vec2<T>{ x + other.x, y + other.y };
    }
    Vec2 operator -(const Vec2<T>& other) const {
        return Vec2<T>{ x - other.x, y - other.y };
    }
    //Dot product
    auto operator *(const Vec2<T>& other) const {
        return  x * other.x + y * other.y;

    }
    Vec2 operator *(float other) const {
        return Vec2<T>(x * other, y * other);
    }
    Vec2 operator /(float other) const {
        return Vec2<T>(x / other, y / other);
    }
    // Cross product
    auto operator /(const Vec2<T>& other) const {
        return x * other.y - y * other.x;
    }
    bool operator ==(const Vec2<T>& other) const {
        return x == other.x && y == other.y;
    }
    bool operator !=(const Vec2<T>& other) const {
        return x != other.x || y != other.y;
    }

    Vec2 rotate(float angle_Rad) const {
        float c = std::cos(angle_Rad);
        float s = std::sin(angle_Rad);
        return { x * c - y * s, x * s + y * c };
    }
    Vec2 reflectX() const {
        return { -x, y };
    }
    Vec2 reflectY() const {
        return { x, -y };
    }
    Vec2 reflectOrigin() const {
        return { -y, x };
    }

    friend std::ostream& operator<<(std::ostream& os, Vec2 const& v) {
        return os << '(' << v.x <<", " << v.y << ')';
    }
};