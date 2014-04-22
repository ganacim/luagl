#ifndef GL_WINDOW_H
#define GL_WINDOW_H

struct GLFWwindow;

class GlWindow {
public:
    GlWindow( void );
    virtual ~GlWindow( void ) { }

    void info( void );

    void init( void );

    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
    friend void window_resize_callback(GLFWwindow* window, int width, int height);
    friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    friend void scroll_callback(GLFWwindow* window, double dx, double dy);
    friend void cursor_pos_callback(GLFWwindow* window, double x, double y);
    friend void cursor_enter_callback(GLFWwindow* window, int enter);

    void main_loop( bool bechmark = false );

    void query_framebuffer_size( int &w, int& h );
private:

    void key_event( int key, int scancode, int action, int mods ) {
        do_key_event( key, scancode, action, mods );
    }
    void framebuffer_resize_event( int width, int height) {
        do_framebuffer_resize( width, height );
    }
    void window_resize_event( int width, int height) {
        do_window_resize( width, height );
    }
    void mouse_button_event( int button, int action, int mods) {
        do_mouse_button( button, action, mods );
    }
    void scroll_event( double dx, double dy) {
        do_scroll( dx, dy );
    }
    void cursor_event( double x, double y) {
        do_cursor( x, y );
    }
    void cursor_enter_event( int enter) {
        do_cursor_enter( enter );
    }

    template<bool benchmark=false>
    void loop( void );

    void draw( void ) { do_draw(); };

protected:
    void close_window( void );

    virtual void do_draw( void ) = 0;
    virtual void do_key_event( int key, int scancode, int action, int mods ) = 0;
    virtual void do_framebuffer_resize( int width, int height ) = 0;
    virtual void do_window_resize( int width, int height ) = 0;
    virtual void do_mouse_button( int button, int action, int mods ) = 0;
    virtual void do_scroll( double dx, double dy ) = 0;
    virtual void do_cursor( double x, double y ) = 0;
    virtual void do_cursor_enter( int enter ) = 0;

private:
    GLFWwindow * m_win;
    int m_numframes;
    double m_time;
};

#endif
