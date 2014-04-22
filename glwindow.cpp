#include "glwindow.h"
#define GLFW_INCLUDE_GLCOREARB
//#define GLFW_INCLUDE_GLU
#define GL_GLEXT_PROTOTYPES 1
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

static void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void window_resize_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double dx, double dy);
void cursor_pos_callback(GLFWwindow* window, double x, double y);
void cursor_enter_callback(GLFWwindow* window, int enter);

GlWindow::GlWindow( void ) 
    : m_numframes(0)
      , m_time(0) 
{
}

void GlWindow::init( void )
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        std::cerr << "GLFW init error!" << std::endl;
        exit(EXIT_FAILURE);
    }
    /* create opengl 4 context */
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    /**************************/
    m_win = glfwCreateWindow(640, 640, "glviewer", NULL, NULL);
    if (!m_win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowUserPointer(m_win, this);
    m_time = glfwGetTime();
    glfwMakeContextCurrent(m_win);
    //
    glfwSetKeyCallback(m_win, key_callback);
    glfwSetWindowSizeCallback(m_win, window_resize_callback);
    glfwSetFramebufferSizeCallback(m_win, framebuffer_resize_callback);
    int width, height;
    glfwGetFramebufferSize(m_win, &width, &height);
    framebuffer_resize_callback(m_win, width, height);
    //
    glfwSetMouseButtonCallback(m_win, mouse_button_callback);
    glfwSetScrollCallback(m_win, scroll_callback);
    glfwSetCursorPosCallback(m_win, cursor_pos_callback);
    glfwSetCursorEnterCallback(m_win, cursor_enter_callback);
    //
    // enable culling
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
}

void GlWindow::info( void )
{
    fprintf(stdout, "OpenGL context\n");
    int major = glfwGetWindowAttrib(m_win, GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(m_win, GLFW_CONTEXT_VERSION_MINOR);
    fprintf(stdout, "\tversion: %d.%d\n", major, minor);
    int profile = glfwGetWindowAttrib(m_win, GLFW_OPENGL_PROFILE);
    switch( profile ) {
        case GLFW_OPENGL_CORE_PROFILE:
            fprintf(stdout, "\tprofile: core\n");
            break;
        case GLFW_OPENGL_COMPAT_PROFILE:
            fprintf(stdout, "\tprofile: compat\n");
            break;
        case GLFW_OPENGL_ANY_PROFILE:
            fprintf(stdout, "\tprofile: any\n");
            break;
    }
    int fw = glfwGetWindowAttrib(m_win, GLFW_OPENGL_FORWARD_COMPAT);
    fprintf(stdout, "\tforward compatible?: %s\n", (fw==GL_TRUE?"yes":"no"));
    int debug = glfwGetWindowAttrib(m_win, GLFW_OPENGL_DEBUG_CONTEXT);
    fprintf(stdout, "\tbebug?: %s\n", (debug==GL_TRUE?"yes":"no"));
    int w, h;
    glfwGetWindowSize(m_win, &w, &h);
    fprintf(stdout, "\tm_win: %dx%d (wxh)\n", w, h);
    glfwGetFramebufferSize(m_win, &w, &h);
    fprintf(stdout, "\tframebuffer: %dx%d (wxh)\n", w, h);
}

void GlWindow::close_window(void)
{
    glfwSetWindowShouldClose(m_win, GL_TRUE);
}

void GlWindow::main_loop( bool benchmark )
{
    if ( benchmark ) {
        loop<true>();
    }
    else {
        loop<false>();
    }
}

template<bool benchmark>
void GlWindow::loop( void )
{
    if (benchmark) {
        // redraw imediately
        glfwSwapInterval(0);
    }
    else {
        //wait for refresh
        glfwSwapInterval(1);
    }
    while (!glfwWindowShouldClose(m_win)) {
        draw();
        glfwSwapBuffers(m_win);
        //
        if (benchmark) {
            ++m_numframes;
            if ( m_numframes >= 1000 ) {
                double now = glfwGetTime();
                double elapsed = now - m_time;
                m_time = now;
                char buf[512];
                sprintf(buf, "%s: %.02f fps", "title", m_numframes/elapsed);
                glfwSetWindowTitle(m_win, buf);
                m_numframes=0;
            }
            glfwPollEvents();
        }
        else {
            glfwWaitEvents();
        }
    }
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GlWindow *win = static_cast<GlWindow*>(glfwGetWindowUserPointer(window));
    win->key_event( key, scancode, action, mods ); 
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    GlWindow *win = static_cast<GlWindow*>(glfwGetWindowUserPointer(window));
    win->framebuffer_resize_event( width, height ); 
}

void window_resize_callback(GLFWwindow* window, int width, int height)
{
    GlWindow *win = static_cast<GlWindow*>(glfwGetWindowUserPointer(window));
    win->window_resize_event( width, height ); 
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    GlWindow *win = static_cast<GlWindow*>(glfwGetWindowUserPointer(window));
    win->mouse_button_event( button, action, mods ); 
}

void scroll_callback(GLFWwindow* window, double dx, double dy)
{
    GlWindow *win = static_cast<GlWindow*>(glfwGetWindowUserPointer(window));
    win->scroll_event( dx, dy ); 
}

void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
    GlWindow *win = static_cast<GlWindow*>(glfwGetWindowUserPointer(window));
    win->cursor_event( x, y ); 
}

void cursor_enter_callback(GLFWwindow* window, int enter)
{
    GlWindow *win = static_cast<GlWindow*>(glfwGetWindowUserPointer(window));
    win->cursor_enter_event( enter ); 
}

void GlWindow::query_framebuffer_size( int &w, int& h )
{
    glfwGetFramebufferSize( m_win, &w, &h );
}
