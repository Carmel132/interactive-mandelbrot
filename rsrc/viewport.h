#pragma once
#include <vec.h>

#define ZOOM_RATE 0.3
#define PAN_RATE 0.1
#define PAN_CUTOFF_THRESHOLD_PIXELS 5

// Stores coordinate box thats maintains proportion
struct Viewport {
    // the width of the box
    long double m_width;
    // the ration of height to width
    long double m_ratio;
    // offset in absolute coordinates
    Vec2<long double> m_offset;
    
    bool* m_update_screen;

    inline Vec2<long double> fromScreenCoord(int x, int y, int w, int h) const {
        long double ret_width = m_width * (long double)x / w;
        long double ret_height = -(m_width * m_ratio) * (long double)y / h;
        return m_offset + Vec2<long double>{ret_width, ret_height};
    }

    inline void zoomAboutPoint(Vec2<long double> point, bool zoom_in) {
        double factor = zoom_in ? 1-ZOOM_RATE : 1+ZOOM_RATE;

        m_width *= factor;
        m_offset = point + (m_offset - point) * factor;

        *m_update_screen = true;
    }
 };

struct Pan {
    

    Vec2<float>* mouse_pos;
    Viewport* view;
    Vec2<int>* window_size;
    bool* update_screen;
    Vec2<long double> anchor;
    Vec2<long double> stored_offset;
    Vec2<long double> target_offset{view->m_offset};

    bool holding_click {false};
    void on_mouse_down() {
        holding_click = true;
        anchor = view->fromScreenCoord(mouse_pos->x, mouse_pos->y, window_size->x, window_size->y);
        stored_offset = view->m_offset + anchor;
        //target_offset = view->m_offset;
    }

    void on_mouse_motion() {
        if (!holding_click) {return;}

        Vec2<long double> offset_change = view->fromScreenCoord(mouse_pos->x, mouse_pos->y, window_size->x, window_size->y); 
        target_offset = stored_offset - offset_change;
    }

    void on_mouse_up() {
        holding_click = false;
    }

    void frame() {
        Vec2 diff = view->m_offset - target_offset;
        long double diff_length_sqrd = diff*diff;
        if (diff_length_sqrd == 0) {
            return;
        }
        *update_screen = true;
        if (diff_length_sqrd < PAN_CUTOFF_THRESHOLD_PIXELS * view->m_width / window_size->x) {
            view -> m_offset = target_offset;
        }
        else {
            view->m_offset = lerp(PAN_RATE, view->m_offset, target_offset);
        }
    }

    void on_zoom() {
        holding_click = false;
        target_offset = view->m_offset;
    }



};