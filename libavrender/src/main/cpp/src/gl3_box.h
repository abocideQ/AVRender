#ifndef AVRENDER_GL3_BOX_H
#define AVRENDER_GL3_BOX_H

#include "common.h"
#include "EGL/egl.h"
#include "GLES3/gl3.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

class gl3Box {
public:
    void update_viewport(int width, int height);

    void gl_box_draw_vao_fbo_camera(int width, int height, uint8_t *data);

protected:
    int m_r_width, m_r_height;

    GLuint m_program[2];
    GLuint m_vao[2];
    GLuint m_vbo[4];
    GLuint m_texture[2];
    GLuint m_fbo;
    GLuint m_fbo_rbo;

    float m_rotation;

    bool initial;

private:
    static GLuint gl_program_create(const char *, const char *);

    static GLuint gl_shader_create(GLenum, const char *);

    static void gl_check(int line);
};


#endif //AVRENDER_GL3_BOX_H
