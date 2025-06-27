#pragma once
#include <iostream>
#include <concepts>

template <typename T> 
concept _Number = std::floating_point<T> or std::integral<T>;

/// @brief Holds 2D numerical data
/// @tparam T Number type
template<typename T> requires _Number<T>
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


/// @brief Linearly interpolates between two vectors
/// @tparam T Number type
/// @param t Interpolation variable
/// @param v1 Start vector
/// @param v2 End vector 
/// @return Linearly interpolated vector
template <typename T>
inline Vec2<T> lerp(long double t, const Vec2<T>& v1, const Vec2<T>& v2) {
    return v1 * (1-t) + v2*t;
}

/// @brief Exponentially interpolated between two vectors
/// @tparam T Number type
/// @param t Interpolation variable
/// @param smoothing_factor How gradual the change is between the vectors
/// @param v1 Start vector
/// @param v2 End vector
/// @return Exponentially interpolated vector
template <typename T>
inline Vec2<T> exp_interp(long double t, long double smoothing_factor, const Vec2<T>& v1, const Vec2<T>& v2) {
    return v1 + (v2 - v1) * (1 - std::exp(-t * smoothing_factor));
}