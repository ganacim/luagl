#include <lua.hpp>
#define GLFW_INCLUDE_GLCOREARB
//#define GLFW_INCLUDE_GLU
#define GL_GLEXT_PROTOTYPES 1
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "objectviewer.h"

class LuaObjectViewer : public ObjectViewer {
public:
    LuaObjectViewer(void) { ; }
    ~LuaObjectViewer(void) { ; }

    void set_camera( GLuint program )
    {
        //if ( camera_changed() ) {
            GLint transform_location = glGetUniformLocation( program, "Transform" );
            if ( transform_location >= 0 ) {
                glUniformMatrix4fv( transform_location, 1, GL_FALSE, glm::value_ptr(camera()) );
            }
            GLint normal_transform_location = glGetUniformLocation( program, "NormalTransform" );
            if ( normal_transform_location >= 0 ) {
                glUniformMatrix3fv( normal_transform_location, 1, GL_FALSE, glm::value_ptr(normal_transform()) );
            }
        //}
    }

    void do_draw( void )
    {/*{{{*/
        auto e = glGetError();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        e = glGetError();
        if ( e != GL_NO_ERROR )
            std::cerr << "error glClear" << std::endl;
        // Vao unbinds after a resize event... wtf
        glBindVertexArray(VaoId);
        switch ( get_render_mode() ) {
            case RENDER_FILL:
                glUseProgram(ProgramId);
                    set_camera( ProgramId );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                    glDrawArrays(GL_TRIANGLES, 0, m_vertex.size());
                break;
            case RENDER_WIRE:
                glUseProgram(ProgramWireId);
                    set_camera( ProgramWireId );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                    glDrawArrays(GL_TRIANGLES, 0, m_vertex.size());
                break;
            case RENDER_FILL_WIRE:
                glUseProgram(ProgramId);
                    set_camera( ProgramId );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                    glDrawArrays(GL_TRIANGLES, 0, m_vertex.size());
                glUseProgram(ProgramWireId);
                    set_camera( ProgramWireId );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                    glDrawArrays(GL_TRIANGLES, 0, m_vertex.size());
                break;
            case RENDER_FILL_WIRE_HIDE:
                glUseProgram(ProgramWhiteId);
                    set_camera( ProgramWhiteId );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                    glDrawArrays(GL_TRIANGLES, 0, m_vertex.size());
                glUseProgram(ProgramWireId);
                    set_camera( ProgramWireId );
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                    glDrawArrays(GL_TRIANGLES, 0, m_vertex.size());
                break;
            default:
                assert( false );
        }
        //glBindVertexArray(0);
        e = glGetError();
        if ( e != GL_NO_ERROR )
            std::cerr << "error glDrawArrays" << std::endl;
    }/*}}}*/

    void show( void )
    {/*{{{*/
        //init();
        info();
        //
        glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );
        glEnable(GL_DEPTH_TEST);
        //
        create_shaders();
        create_vbo();
        //
        main_loop(false);
        //main_loop(true);
        //
        destroy_vbo();
        destroy_shaders();
    }/*}}}*/

    void create_shaders( void )
    {/*{{{*/
        // reset error status
        GLenum ErrorCheckValue = glGetError();
        //
        VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(VertexShaderId, 1, get_vertex_shader(), NULL);
        glCompileShader(VertexShaderId);
        FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentShaderId, 1, get_fragment_shader(), NULL);
        glCompileShader(FragmentShaderId);
        ProgramId = glCreateProgram();
            glAttachShader(ProgramId, VertexShaderId);
            glAttachShader(ProgramId, FragmentShaderId);
        glLinkProgram(ProgramId);
        //
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR) {
            fprintf( stderr,
                "ERROR: Could not create the shaders: %d \n",
                //gluErrorString(ErrorCheckValue)
                glGetError()
            );
            exit(-1);
        }
        //
        FragmentShaderWireId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentShaderWireId, 1, get_fragment_shader_wire(), NULL);
        glCompileShader(FragmentShaderWireId);
        ProgramWireId = glCreateProgram();
            glAttachShader(ProgramWireId, VertexShaderId);
            glAttachShader(ProgramWireId, FragmentShaderWireId);
        glLinkProgram(ProgramWireId);
        GLint link_status = GL_FALSE;
        glGetProgramiv(ProgramWireId, GL_LINK_STATUS, &link_status);
        if ( link_status == GL_FALSE ) {
            std::cerr << "link error!" << std::endl;
        }
        fprintf(stdout, "\tdone!\n");
        //
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR) {
            fprintf( stderr,
                "ERROR: Could not create the shaders: %d \n",
                //gluErrorString(ErrorCheckValue)
                glGetError()
            );
            exit(-1);
        }
        //
        FragmentShaderWhiteId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentShaderWhiteId, 1, get_fragment_shader_white(), NULL);
        glCompileShader(FragmentShaderWhiteId);
        ProgramWhiteId = glCreateProgram();
            glAttachShader(ProgramWhiteId, VertexShaderId);
            glAttachShader(ProgramWhiteId, FragmentShaderWhiteId);
        glLinkProgram(ProgramWhiteId);
        link_status = GL_FALSE;
        glGetProgramiv(ProgramWhiteId, GL_LINK_STATUS, &link_status);
        if ( link_status == GL_FALSE ) {
            std::cerr << "link error!" << std::endl;
        }
        fprintf(stdout, "\tdone!\n");
        //
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR) {
            fprintf( stderr,
                "ERROR: Could not create the shaders: %d \n",
                //gluErrorString(ErrorCheckValue)
                glGetError()
            );
            exit(-1);
        }
        //
        glUseProgram(ProgramId);
    }/*}}}*/

    void destroy_shaders(void)
    {/*{{{*/
        GLenum ErrorCheckValue = glGetError();
        glUseProgram(0);
        glDetachShader(ProgramId, VertexShaderId);
        glDetachShader(ProgramId, FragmentShaderId);
        glDeleteShader(FragmentShaderId);
        glDeleteShader(VertexShaderId);
        glDeleteProgram(ProgramId);
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR) {
            fprintf( stderr,
                    "ERROR: Could not destroy the shaders: %d \n",
                    //gluErrorString(ErrorCheckValue)
                    ErrorCheckValue
            );
            exit(-1);
        }
    }/*}}}*/

    void create_vbo(void)
    {/*{{{*/
        // reset error
        GLenum ErrorCheckValue = glGetError();
        // create vertex array
        // http://www.opengl.org/sdk/docs/man4/xhtml/glGenVertexArrays.xml
        glGenVertexArrays(1, &VaoId);
        glBindVertexArray(VaoId);
        // geometry buffer
        glGenBuffers(1, &VboId); //generates the buffer
        glBindBuffer(GL_ARRAY_BUFFER, VboId); //activate the buffer
        //copies data to the active buffer of type GL_ARRAY_BUFFER
        glBufferData(GL_ARRAY_BUFFER, m_vertex.size()*sizeof(float), &m_vertex.front(), GL_STATIC_DRAW);
        //sets the index (0, used by the shader), of 4 elements of type float, not
        //normalized, packed (0 stride), at the begining of the active buffer.
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        // element index
        //glGenBuffers(1, &VeoId);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VeoId);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // color buffer
        glGenBuffers(1, &NormalBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, NormalBufferId);
        glBufferData(GL_ARRAY_BUFFER, m_normal.size()*sizeof(float), &m_normal.front(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        //
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR) {
            fprintf( stderr,
                "ERROR: Could not create a VBO: %d \n",
                //gluErrorString(ErrorCheckValue)
                ErrorCheckValue
            );
            exit(-1);
        }
        //
        glEnable( GL_POLYGON_OFFSET_LINE );
        glPolygonOffset( -1.f, 1.f );
        //
    }/*}}}*/

    void destroy_vbo(void)
    {/*{{{*/
        GLenum ErrorCheckValue = glGetError();
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &ColorBufferId);
        glDeleteBuffers(1, &VboId);
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &VaoId);
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR) {
            fprintf( stderr,
                "ERROR: Could not destroy the VBO: %d \n",
                //gluErrorString(ErrorCheckValue)
                ErrorCheckValue
            );
            exit(-1);
        }
    }/*}}}*/

    void add_triangle( float x, float y, float z, float nx, float ny, float nz )
    {/*{{{*/
        m_vertex.push_back( x );
        m_vertex.push_back( y );
        m_vertex.push_back( z );
        m_vertex.push_back( 1.f );
        m_normal.push_back( nx );
        m_normal.push_back( ny );
        m_normal.push_back( nz );
    }/*}}}*/

private:
    GLuint VertexShaderId, FragmentShaderId, FragmentShaderWireId, FragmentShaderWhiteId, 
           ProgramId, ProgramWireId, ProgramWhiteId,
       VaoId, VboId, VeoId, ColorBufferId, NormalBufferId;

    const GLchar *const * get_vertex_shader(void) const
    {/*{{{*/
        static const GLchar* vertex_shader_string[] = {
                "#version 400\n"\
                "layout(location=0) in vec4 in_Position;\n"\
                "layout(location=1) in vec3 in_Normal;\n"\
                "uniform mat4 Transform;\n"\
                "uniform mat3 NormalTransform;\n"\
                "out vec4 ex_Color;\n"\
                "void main(void)\n"\
                "{\n"\
                "	gl_Position = Transform*in_Position;\n"\
                "	float d = max( 0.f, dot( NormalTransform*normalize(in_Normal), vec3(0.f, 0.f, 1.f) ));\n"\
                "	ex_Color = d*vec4(0.f, 0.f, 1.f, 1.f);\n"\
                "}\n"
        };
        return vertex_shader_string;
    }/*}}}*/

    const GLchar *const * get_fragment_shader(void) const
    {/*{{{*/
        static const GLchar* fragment_shader_string[] = {
                "#version 400\n"\
                "in vec4 ex_Color;\n"\
                "out vec4 out_Color;\n"\
                "void main(void)\n"\
                "{\n"\
                "	out_Color = ex_Color;\n"\
                "}\n"
        };
        return fragment_shader_string;
    }/*}}}*/
    
    const GLchar *const * get_fragment_shader_wire(void) const
    {/*{{{*/
        static const GLchar* fragment_shader_string[] = {
                "#version 400\n"\
                "in vec4 ex_Color;\n"\
                "out vec4 out_Color;\n"\
                "void main(void)\n"\
                "{\n"\
                "	out_Color = vec4(1.f, 0.f, 0.f, 1.f);\n"\
                "}\n"
        };
        return fragment_shader_string;
    }/*}}}*/

    const GLchar *const * get_fragment_shader_white(void) const
    {/*{{{*/
        static const GLchar* fragment_shader_string[] = {
                "#version 400\n"\
                "in vec4 ex_Color;\n"\
                "out vec4 out_Color;\n"\
                "void main(void)\n"\
                "{\n"\
                "	out_Color = vec4(.9f, .9f, .9f, 1.f);\n"\
                "}\n"
        };
        return fragment_shader_string;
    }/*}}}*/

    std::vector<float> m_vertex;
    std::vector<float> m_normal;
};

static int object_show(lua_State *L) {
    auto *o = static_cast<LuaObjectViewer*>( lua_touserdata(L, lua_upvalueindex(1)) );
    //double n = luaL_checknumber(L, 1);
    //lua_pushnumber(L, o->method(n) );
    o->show();
    return 0;
}

static int object_add_point(lua_State *L) {
    auto *o = static_cast<LuaObjectViewer*>( lua_touserdata(L, lua_upvalueindex(1)) );
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);
    double z = luaL_checknumber(L, 3);
    double nx = luaL_checknumber(L, 4);
    double ny = luaL_checknumber(L, 5);
    double nz = luaL_checknumber(L, 6);
    o->add_triangle(x, y, z, nx, ny, nz);
    return 0;
}

static int object_delete(lua_State *L) {
    auto *o = static_cast<LuaObjectViewer*>( lua_touserdata(L,1) );
    o->~LuaObjectViewer();
    return 0;
}

static const struct luaL_Reg cmethods[] = {
    {"show", object_show},
    {"add_point", object_add_point},
    {NULL, NULL}
};

extern "C"
int luaopen_glviewer(lua_State *L) {
    luaL_newlibtable(L, cmethods); //modtable
    LuaObjectViewer *o = static_cast<LuaObjectViewer*>( lua_newuserdata(L, sizeof(LuaObjectViewer)) );
    new (o) LuaObjectViewer;
    //
    lua_newtable(L); // modtable, ptr, table
    lua_pushliteral(L, "__gc"); // modtable, ptr, table, "__gc"
    lua_pushcfunction(L, object_delete); // modtable, ptr, table cpu, "__gc", delete
    lua_rawset(L, -3); // modtable, ptr, table
    lua_setmetatable(L, -2); // modtable, ptr
    //
    luaL_setfuncs(L, cmethods, 1);
    return 1;
}
