#pragma once
#include <vec.h>

#define ZOOM_RATE 0.3
#define PAN_RATE 0.1
#define PAN_CUTOFF_THRESHOLD_PIXELS 5
#define PAN_SMOOTH_FACTOR 5

/// @brief Stores coordinate box thats maintains proportion
struct Viewport {
    // the width of the box
    long double m_width;
    // the ratio of height to width
    long double m_ratio;
    // offset in relative coordinates
    Vec2<long double> m_offset;
    
    bool* m_update_screen;

    /// @brief Converts absolute coordinates (pixels) to relative coordinates
    /// @param x x coordinate
    /// @param y y coordinate
    /// @param w Width of absolute rect
    /// @param h Height of absolute rect
    /// @return Given point in relative coordinates
    inline Vec2<long double> fromScreenCoord(int x, int y, int w, int h) const {
        long double ret_width = m_width * (long double)x / w;
        long double ret_height = -(m_width * m_ratio) * (long double)y / h;
        return m_offset + Vec2<long double>{ret_width, ret_height};
    }

    /// @brief Zooms viewport about point
    /// @param point The point to zoom about in relative coordinates.
    /// @param zoom_in Whether the to zoom in, out otherwise
    inline void zoomAboutPoint(Vec2<long double> point, bool zoom_in) {
        double factor = 1 + (zoom_in ? -ZOOM_RATE : ZOOM_RATE);

        m_width *= factor;
        m_offset = point + (m_offset - point) * factor;

        *m_update_screen = true;
    }
 };

/// @brief Provides panning functionality to `Viewport`
struct Pan {
    Vec2<float>* mouse_pos;
    Viewport* view;
    Vec2<int>* window_size;
    bool* update_screen;
    Vec2<long double> anchor;
    Vec2<long double> stored_offset;
    Vec2<long double> target_offset{view->m_offset};

    bool holding_click {false};

    /// @brief Called whenever the mouse is pressed
    void on_mouse_down() {
        holding_click = true;
        anchor = view->fromScreenCoord(mouse_pos->x, mouse_pos->y, window_size->x, window_size->y);
        stored_offset = view->m_offset + anchor;
    }

    /// @brief Called whenever the mouse is moved
    void on_mouse_motion() {
        if (!holding_click) {return;}

        Vec2<long double> offset_change = view->fromScreenCoord(mouse_pos->x, mouse_pos->y, window_size->x, window_size->y); 
        target_offset = stored_offset - offset_change;
    }

    /// @brief Called whenever the mouse is released
    void on_mouse_up() {
        holding_click = false;
    }

    /// @brief Called on each frame
    void frame() {
        Vec2 diff = view->m_offset - target_offset;
        long double diff_length_sqrd = diff*diff;
        long double px_size = (PAN_CUTOFF_THRESHOLD_PIXELS * view->m_width / window_size->x);
        if (diff_length_sqrd == 0) {
            return;
        }

        *update_screen = true;
        if (!holding_click && diff_length_sqrd < px_size * px_size) {
            view -> m_offset = target_offset;
        }
        else {
            view->m_offset = exp_interp(PAN_RATE, PAN_SMOOTH_FACTOR, view->m_offset, target_offset);
        }
    }

    /// @brief This should be called in conjunction with `view->zoomAboutPoint`
    void on_zoom() {
        holding_click = false;
        target_offset = view->m_offset;
    }
};