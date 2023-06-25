#ifndef AVRENDER_GL3_PLANE_H
#define AVRENDER_GL3_PLANE_H

#include "common.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>

class gl3Plane {
public:
    void update_viewport(int width, int height);

    void gl_rgba_draw_vao_fbo_pbo_camera(bool initial, int width, int height, uint8_t *data);

protected:
    int m_r_width, m_r_height;

    GLuint m_program[2];
    GLuint m_vbo[3];
    GLuint m_vao[1];
    GLuint m_texture[2];
    GLuint m_fbo;

private:
    static GLuint gl_program_create(const char *, const char *);

    static GLuint gl_shader_create(GLenum, const char *);

    static void gl_check(int line);
};


#endif //AVRENDER_GL3_PLANE_H
