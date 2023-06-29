#ifndef AVRENDER_GL3_SKY_BOX_H
#define AVRENDER_GL3_SKY_BOX_H

#include "common.h"
#include "EGL/egl.h"
#include "GLES3/gl3.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

class gl3SkyBox {
public:
    void update_viewport(int width, int height);

    void gl_sky_box(int width, int height, uint8_t *data,
                    int sky_w, int sky_h,
                    uint8_t *sky_right,
                    uint8_t *sky_left,
                    uint8_t *sky_top,
                    uint8_t *sky_bottom,
                    uint8_t *sky_front,
                    uint8_t *sky_back
    );

protected:
    int m_r_width, m_r_height;

    GLuint m_program_sky;
    GLuint m_vao_sky;
    GLuint m_vbo_sky;
    GLuint m_texture_sky;

    GLuint m_program[2];
    GLuint m_vao[2];
    GLuint m_vbo[5];
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


#endif //AVRENDER_GL3_SKY_BOX_H
