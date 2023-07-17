#ifndef AVRENDER_GL3_MODE_OBJ_H
#define AVRENDER_GL3_MODE_OBJ_H

#include "common.h"
#include "EGL/egl.h"
#include "GLES3/gl3.h"
#include "stb_image.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

class gl3ModeObj {
public:
    void update_viewport(int width, int height);

    void gl_mode_obj(int width, int height, std::string &, std::string &);

protected:
    std::vector<float> m_vec_vertex;
    std::vector<float> m_vec_texture;
    std::vector<std::vector<int>> m_vec_indices;

    int m_r_width, m_r_height;

    GLuint m_program[1];
    GLuint m_vbo[2];
    GLuint m_vao[1];
    GLuint m_tex[1];

    GLuint m_fbo[1];
    GLuint m_fbo_program[1];
    GLuint m_fbo_vbo[2];
    GLuint m_fbo_vao[1];
    GLuint m_fbo_tex[1];
    GLuint m_fbo_rbo;
    std::vector<GLuint> m_fbo_ebo = std::vector<GLuint>();

    float m_rotation;

    bool initial;

private:
    GLuint gl_program_create(const char *, const char *);

    GLuint gl_shader_create(GLenum, const char *);

    static void gl_check(int line);

    static int
    obj_parse(const std::string &,
              std::vector<float> *,
              std::vector<float> *,
              std::vector<std::vector<int>> *);

    static std::vector<std::string> string_split(const std::string &, const char *);
};


#endif //AVRENDER_GL3_MODE_OBJ_H
