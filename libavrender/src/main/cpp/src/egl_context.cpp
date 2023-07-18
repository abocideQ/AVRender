#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "egl_context.h"

void eglContext::onscreenWindowCreate(int glVersion, ANativeWindow *native_window) {
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLDisplay eglDisplay = m_eglDisplay;
    if (eglDisplay == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay fail");
        return;
    }
    if (EGL_TRUE != eglInitialize(eglDisplay, nullptr, nullptr)) {
        LOGE("eglInitialize fail");
        return;
    }
    EGLint eglConfigNum;
    EGLConfig eglConfig;
    EGLint eglConfigAttr[] = {
            EGL_RED_SIZE, 8, // color bit
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, // EGL_WINDOW_BIT or EGL_PBUFFER_BIT
            EGL_NONE
    };
    if (EGL_TRUE != eglChooseConfig(eglDisplay, eglConfigAttr, &eglConfig, 1, &eglConfigNum)) {
        LOGE("eglChooseConfig fail");
        return;
    }
    m_eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, native_window, nullptr);
    EGLSurface eglSurface = m_eglSurface;
    if (eglSurface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return;
    }
    const EGLint elgContextAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, glVersion, EGL_NONE
    };
    m_eglContext = eglCreateContext(eglDisplay,
                                    eglConfig,
                                    EGL_NO_CONTEXT, // EGL_NO_CONTEXT no share
                                    elgContextAttr);
    EGLContext eglContext = m_eglContext;
    if (eglContext == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return;
    }
    if (EGL_TRUE != eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        LOGE("eglMakeCurrent failed");
        return;
    }
}

void eglContext::onscreenWindowSwap() {
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void eglContext::onscreenDraw() {
    const char *gl_shader_vertex_source_plane = GET_CHAR(
            precision highp float;
            attribute vec4 aPosition;
            void main() {
                gl_Position = vec4(aPosition.xyz, 1.0);
            }
    );
    const char *gl_shader_fragment_source_plane = GET_CHAR(
            precision highp float;
            void main() {
                gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
            }
    );
    float loc_vertex[] = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
    };
    if (!initial) {
        initial = true;
        // gl_program
        m_onscreen_program = gl_program_create(
                gl_shader_vertex_source_plane,
                gl_shader_fragment_source_plane
        );
    }
    glUseProgram(m_onscreen_program);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) loc_vertex);
    glEnableVertexAttribArray(0);
    gl_check(__LINE__);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    gl_check(__LINE__);
}

void eglContext::offscreenWindowCreate(int glVersion, int w, int h) {
    m_r_width = w;
    m_r_height = h;
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLDisplay eglDisplay = m_eglDisplay;
    if (eglDisplay == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay fail");
        return;
    }
    if (EGL_TRUE != eglInitialize(eglDisplay, nullptr, nullptr)) {
        LOGE("eglInitialize fail");
        return;
    }
    EGLint eglConfigNum;
    EGLConfig eglConfig;
    EGLint eglConfigAttr[] = {
            EGL_RED_SIZE, 8, // color bit
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, // EGL_WINDOW_BIT or EGL_PBUFFER_BIT
            EGL_NONE
    };
    if (EGL_TRUE != eglChooseConfig(eglDisplay, eglConfigAttr, &eglConfig, 1, &eglConfigNum)) {
        LOGE("eglChooseConfig fail");
        return;
    }
    const EGLint eglSurfaceAttr[] = {
            EGL_WIDTH, w,
            EGL_HEIGHT, h,
            EGL_NONE
    };
    m_eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, eglSurfaceAttr);
    EGLSurface eglSurface = m_eglSurface;
    if (eglSurface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return;
    }
    const EGLint elgContextAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, glVersion, EGL_NONE
    };
    m_eglContext = eglCreateContext(eglDisplay,
                                    eglConfig,
                                    EGL_NO_CONTEXT, // EGL_NO_CONTEXT no share
                                    elgContextAttr);
    EGLContext eglContext = m_eglContext;
    if (eglContext == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return;
    }
    if (EGL_TRUE != eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        LOGE("eglMakeCurrent failed");
        return;
    }
}

void eglContext::offscreenDraw(std::string &save_path_name) {
    const char *gl_shader_vertex_source_plane = GET_CHAR(
            precision highp float;
            attribute vec4 aPosition;
            void main() {
                gl_Position = vec4(aPosition.xyz, 1.0);
            }
    );
    const char *gl_shader_fragment_source_plane = GET_CHAR(
            precision highp float;
            void main() {
                gl_FragColor = vec4(0.2, 0.3, 0.4, 1.0);
            }
    );
    float loc_vertex[] = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
    };
    if (!initial) {
        initial = true;
        // gl_program
        m_offscreen_program = gl_program_create(
                gl_shader_vertex_source_plane,
                gl_shader_fragment_source_plane
        );
    }
    glUseProgram(m_offscreen_program);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) loc_vertex);
    glEnableVertexAttribArray(0);
    gl_check(__LINE__);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    gl_check(__LINE__);
    auto *data = new uint8_t[m_r_width * m_r_height * 4];
    glReadPixels(0, 0, m_r_width, m_r_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_write_jpg(save_path_name.c_str(),
                   m_r_width, m_r_height,
                   4, data, m_r_width * 4);
    gl_check(__LINE__);
    delete[] data;
}

void eglContext::release() {
    if (m_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglDestroyContext(m_eglDisplay, m_eglContext);
        eglTerminate(m_eglDisplay);
        eglReleaseThread();
    }
    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglSurface = EGL_NO_SURFACE;
    m_eglContext = EGL_NO_CONTEXT;
}

GLuint eglContext::gl_program_create(const char *vertex, const char *fragment) {
    GLuint gl_program = glCreateProgram();
    { // program attach & link & delete shader
        GLuint gl_shader_vertex = gl_shader_create(GL_VERTEX_SHADER, vertex);
        GLuint gl_shader_fragment = gl_shader_create(GL_FRAGMENT_SHADER, fragment);
        glAttachShader(gl_program, gl_shader_vertex);
        glAttachShader(gl_program, gl_shader_fragment);
        glLinkProgram(gl_program);
        glDetachShader(gl_program, gl_shader_vertex);
        glDetachShader(gl_program, gl_shader_fragment);
        glDeleteShader(gl_shader_vertex);
        glDeleteShader(gl_shader_fragment);
    }
    { // err check
        GLint gl_status = 0;
        glGetProgramiv(gl_program, GL_LINK_STATUS, &gl_status);
        if (gl_status == GL_FALSE) {
            GLint length = 0;
            glGetProgramiv(gl_program, GL_INFO_LOG_LENGTH, &length);
            if (length) {
                char *log = (char *) malloc((size_t) length);
                if (log) {
                    glGetProgramInfoLog(gl_program, length, nullptr, log);
                    LOGE("glProgram program link err :\n%s\n", log);
                    free(log);
                }
                glDeleteProgram(gl_program);
            }
        }
    }
    return gl_program;
}

GLuint eglContext::gl_shader_create(GLenum gl_type, const char *gl_source) {
    GLuint gl_shader = glCreateShader(gl_type);
    { // shader compile
        glShaderSource(gl_shader, 1, &gl_source, nullptr);
        glCompileShader(gl_shader);
    }
    { // err check
        GLint gl_status = 0;
        glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &gl_status);
        if (gl_status == GL_FALSE) {
            GLint length = 0;
            glGetShaderiv(gl_shader, GL_INFO_LOG_LENGTH, &length);
            if (length) {
                char *log = (char *) malloc((size_t) length);
                if (log) {
                    glGetShaderInfoLog(gl_shader, length, nullptr, log);
                    if (gl_type == GL_VERTEX_SHADER) {
                        LOGE("glShader vertex shader compile err :\n%s\n", log);
                    } else {
                        LOGE("glShader fragment shader compile err :\n%s\n", log);
                    }
                    free(log);
                }
            }
        }
    }
    return gl_shader;
}

void eglContext::gl_check(int line) {
    GLenum err = glGetError();
    if (!err) return;
    LOGE("gl_check error=%d, line=%d", glGetError(), line);
}


