#ifndef AVRENDER_GL2_PLANE_H
#define AVRENDER_GL2_PLANE_H

#include "common.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

class gl2Plane {
public:
    void update_viewport(int width, int height);

    void gl_rgba_draw_array(int width, int height, uint8_t *data);

    void gl_rgba_draw_elements_vbo_fbo(int width, int height, uint8_t *data);

protected:
    int m_r_width, m_r_height;

    GLuint m_a_program;
    GLuint m_a_texture;

    GLuint m_b_program[2];
    GLuint m_b_vbo[3];
    GLuint m_b_texture[2];
    GLuint m_b_fbo;

    bool initial;

private:
    static GLuint gl_program_create(const char *, const char *);

    static GLuint gl_shader_create(GLenum, const char *);

    static void gl_check(int line);
};

#endif //AVRENDER_GL2_PLANE_H
