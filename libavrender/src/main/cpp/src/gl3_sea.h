#ifndef AVRENDER_GL3_SEA_H
#define AVRENDER_GL3_SEA_H

#include "common.h"
#include "EGL/egl.h"
#include "GLES3/gl3.h"
#include <chrono>

class gl3Sea {
public:
    void update_viewport(int width, int height);

    void gl_sea();

protected:
    int m_r_width, m_r_height;

    bool initial;

    double startTime;
    double currentTime;

    GLuint m_program;

private:
    static GLuint gl_program_create(const char *, const char *);

    static GLuint gl_shader_create(GLenum, const char *);

    static void gl_check(int line);

    static double glfwGetTime();
};


#endif //AVRENDER_GL3_SEA_H
