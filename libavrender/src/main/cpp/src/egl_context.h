#ifndef AVRENDER_EGL_CONTEXT_H
#define AVRENDER_EGL_CONTEXT_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "stb_image_write.h"
#include "common.h"

class eglContext {

public:
    void onscreenWindowCreate(int glVersion, ANativeWindow *native_window);

    void onscreenWindowSwap();

    void onscreenDraw();

    void offscreenWindowCreate(int glVersion, int w, int h);

    void offscreenDraw(std::string &save_path_name);

    void release();

protected:
    EGLDisplay m_eglDisplay;
    EGLSurface m_eglSurface;
    EGLContext m_eglContext;

    int m_r_width, m_r_height;
    GLuint m_onscreen_program;
    GLuint m_offscreen_program;

    bool initial;

private:
    static GLuint gl_program_create(const char *, const char *);

    static GLuint gl_shader_create(GLenum, const char *);

    static void gl_check(int line);
};


#endif //AVRENDER_EGL_CONTEXT_H
