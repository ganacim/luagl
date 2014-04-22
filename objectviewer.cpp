#include "objectviewer.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/vector_angle.hpp>

ObjectViewer::ObjectViewer(void)
    : m_width(0), m_height(0),
    m_rotation(1.0f),
    m_proj_view(1.0f)
{
    m_camera.changed = true;
    m_camera.fov = glm::pi<float>()/4.f;
    m_rendermode = RENDER_FILL;
    m_culling = true;
    init();
}

void ObjectViewer::init( void )
{
    GlWindow::init();
}

bool ObjectViewer::camera_changed(void)
{
    bool r = m_camera.changed;
    if ( r ) {
        m_camera.changed = false;
        return true;
    }
    return false;
}

const glm::mat4& ObjectViewer::camera(void) const
{
    return m_proj_view;
}

const glm::mat3& ObjectViewer::normal_transform(void) const
{
    return m_normal_transform;
}

void ObjectViewer::transform_model( float dx, float dy )
{
    if ( dx != 0 ) {
        m_rotation = glm::rotate( glm::mat4(1.0f), glm::pi<float>()*dx/180.f, glm::vec3(0,1,0)) * m_rotation;
    }
    else if ( dy != 0 ) {
        m_rotation = glm::rotate( glm::mat4(1.0f), glm::pi<float>()*dy/180.f, glm::vec3(1,0,0)) * m_rotation;
    }
    update_model_view_projection();
}

void ObjectViewer::toogle_culling( void )
{
    if ( m_culling ) {
        glDisable( GL_CULL_FACE );
    }
    else {
        glEnable( GL_CULL_FACE );
    }
    m_culling = !m_culling;
}

void ObjectViewer::do_key_event( int key, int scancode, int action, int mods )
{
    //std::cerr << "key :" << key << " char: " << char(key) << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        close_window();
    if ( action == GLFW_PRESS ) {
        switch( key ) {
            case 262: //arrow right
                transform_model( 1.f, 0.f );
                break;
            case 263: //arrow left
                transform_model( -1.f, 0.f );
                break;
            case 264: //arrow down
                transform_model( 0.f, 1.f );
                break;
            case 265: //arrow up
                transform_model( 0.f, -1.f );
                break;
            case 'H':
                show_help();
                break;
            case 'F':
                toogle_culling();
                break;
            case 'W':
                m_rendermode = static_cast<RenderMode>( (m_rendermode+1)%RENDER_SIZE );
                break;
            default:
                fprintf( stderr, "unkown key:%c\n", key );
        }
    }
}

void ObjectViewer::show_help( void ) const
{
    const char * h = ""\
    "-----------------------------\n"\
    "keys: \n"\
    "\th - show help\n"\
    "\tw - show wire frame\n"\
    "";
    fprintf( stdout, "%s", h );
}

void ObjectViewer::do_framebuffer_resize( int width, int height )
{
    m_width=width;
    m_height=height;
    glViewport(0, 0, m_width, m_height);
    update_model_view_projection();
}

void ObjectViewer::do_window_resize( int width, int height )
{
}

void ObjectViewer::transform_model( bool keep_rotation )
{
    float dx = m_mouse.x - m_mouse.press.x;
    float dy = m_mouse.y - m_mouse.press.y;
    if ( dx*dx + dy*dy < 0.05f ) return;
    float ratio = float(m_width)/float(m_height);
    glm::mat4 projection = glm::perspective( m_camera.fov, ratio, 0.1f, 100.0f );
    //glm::mat4 projection(1.f);
    glm::mat4 view = glm::lookAt( glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0) );
    glm::vec4 vp( 0.f, 0.f, m_width, m_height );
    //
    glm::vec3 mpos0( m_mouse.press.x, m_mouse.press.y, 0.f );
    auto pos0 = glm::unProject( mpos0, view, projection, vp );
    //std::cerr << "o: " << m_mouse.press.x << ", " << m_mouse.press.y << std::endl;
    //std::cerr << "ow: " << pos0.x << " " << pos0.y << " " << pos0.z << std::endl;
    //
    glm::vec3 mpos1( m_mouse.x, m_mouse.y, 0.f );
    auto pos1 = glm::unProject( mpos1, view, projection, vp );
    //std::cerr << "d: " << m_mouse.x << ", " << m_mouse.y << std::endl;
    //std::cerr << "dw: " << pos1.x << " " << pos1.y << " " << pos1.z << std::endl;
    //
    auto v0 = glm::normalize(pos0);
    auto v1 = glm::normalize(pos1);
    auto ref = glm::cross( v0, v1 );
    auto angle = glm::orientedAngle( v0, v1, ref );
    //auto rotation = glm::rotate( glm::mat4(1.0f), 180.f*angle/glm::pi<float>(), ref);
    auto rotation = glm::rotate( glm::mat4(1.0f), 100.f*angle, ref);
    //auto rotation = glm::rotate( glm::mat4(1.0f), glm::pi<float>()/4.f, ref);
    /*
    std::cerr << "angle: " << angle << " graus:" << 180.f*angle/glm::pi<float>()
        << " dot:" << std::acos( glm::dot(v0, v1) ) << " pi:" << glm::pi<float>()
        << " dot(v0,v1):" << glm::dot(v0, v1) << " dot(v1,v1):" << glm::dot(v1,v1)
        << " " << std::endl;
       */
    if ( !keep_rotation ) {
        update_model_view_projection(rotation);
    }
    else {
        m_rotation = rotation * m_rotation;
        update_model_view_projection();
    }
}

void ObjectViewer::do_mouse_button( int button, int action, int mods )
{
    if ( action == GLFW_PRESS ) {
        m_mouse.press.x = m_mouse.x;
        m_mouse.press.y = m_mouse.y;
        m_mouse.press.button = button;
        m_mouse.press.mods = mods;
    }
    else {
        switch ( button ) {
            case 0:
                transform_model( true );
                break;
        }
        m_mouse.press.button = -1;
    }
}

void ObjectViewer::do_scroll( double dx, double dy )
{
    m_camera.fov = std::max( std::min( m_camera.fov + 0.5f*float(dy), 0.95f*glm::pi<float>() ), 0.01f*glm::pi<float>());
    update_model_view_projection();
}

void ObjectViewer::do_cursor( double x, double y )
{
    m_mouse.x = x;
    m_mouse.y = double(m_height) - y;
    if ( m_mouse.press.button >= 0 ) {
        switch( m_mouse.press.mods ) {
            case 0:
                transform_model( false );
                break;
        }
    }
}

void ObjectViewer::update_model_view_projection( const glm::mat4& trotation )
{
    float ratio = float(m_width)/float(m_height);
    glm::mat4 projection = glm::perspective( m_camera.fov, ratio, 0.1f, 100.0f );
    glm::mat4 view = glm::lookAt( glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0) );
    auto mv = view * trotation * m_rotation;
    m_proj_view = projection * mv;
    m_normal_transform = glm::inverseTranspose(glm::mat3(mv));
    m_camera.changed = true;
}

void ObjectViewer::update_model_view_projection( void )
{
    update_model_view_projection( glm::mat4(1.f) );
}
