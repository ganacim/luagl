#ifndef OBJECT_VIEWER_H
#define OBJECT_VIEWER_H

#include "glwindow.h"

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

struct Camera
{
    float fov;
    bool changed;
};

struct Mouse {
    double x, y;
    struct press_ {
        int button = -1;
        int mods;
        double x, y;
    } press;
};

enum RenderMode {
    RENDER_FILL,
    RENDER_WIRE,
    RENDER_FILL_WIRE,
    RENDER_FILL_WIRE_HIDE,
    RENDER_SIZE
};

class ObjectViewer : public GlWindow {
public:
    ObjectViewer(void);
    bool camera_changed(void);
    const glm::mat4& camera(void) const;
    const glm::mat3& normal_transform(void) const;
    void toogle_culling( void );

protected:
    RenderMode get_render_mode( void ) const { return m_rendermode; }

private:
    void init( void );
    void do_key_event( int key, int scancode, int action, int mods );
    void do_framebuffer_resize( int width, int height );
    void do_window_resize( int width, int height );
    void do_mouse_button( int button, int action, int mods );
    void do_scroll( double dx, double dy );
    void do_cursor( double x, double y );
    void do_cursor_enter( int enter ) { };

    void update_model_view_projection( const glm::mat4& trotation );
    void update_model_view_projection( void );

    void transform_model( float dx, float dy );
    void transform_model( bool keep_rotation );

    void show_help( void ) const;

    int m_width, m_height;
    glm::mat4 m_rotation, m_proj_view;
    glm::mat3 m_normal_transform;
    Camera m_camera;
    Mouse m_mouse;
    RenderMode m_rendermode;
    bool m_culling;

};

#endif
