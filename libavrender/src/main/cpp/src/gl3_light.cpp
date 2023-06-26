#include "gl3_light.h"

void gl3Light::update_viewport(int width, int height) {
    m_r_width = width;
    m_r_height = height;
    glViewport(0, 0, width, height);
}

void gl3Light::gl_light_draw(bool initial, int width, int height, uint8_t *data) {
    const char *gl_shader_vertex_plane_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    layout(location = 0) in vec4 aPosition;
                    layout(location = 1) in vec2 aTexCoord;
                    out vec2 TexCoord;
                    void main() {
                        gl_Position = vec4(aPosition.xyz, 1.0);
                        TexCoord = aTexCoord;
                    }
            );
    const char *gl_shader_fragment_plane_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    in vec2 TexCoord;
                    uniform sampler2D TexSample;
                    layout(location = 0) out vec4 fragColor;
                    void main() {
                        fragColor = texture(TexSample, vec2(TexCoord.x, 1.0 - TexCoord.y));
                    }
            );
    const char *gl_shader_fbo_plane_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    in vec2 TexCoord;
                    uniform sampler2D TexSample;
                    layout(location = 0) out vec4 fragColor;
                    void main() {
                        fragColor = texture(TexSample, TexCoord);
                    }
            );
    float loc_vertex[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };
    { // config
        if (initial) {
            // gl_program
            m_program[0] = gl_program_create(
                    gl_shader_vertex_plane_source,
                    gl_shader_fbo_plane_source
            );
            m_program[1] = gl_program_create(
                    gl_shader_vertex_plane_source,
                    gl_shader_fragment_plane_source
            );
            gl_check(__LINE__);
            if (m_program[0] == GL_NONE || m_program[1] == GL_NONE) return;
            // vbo & ebo & vao
            glGenVertexArrays(1, m_vao);
            glGenBuffers(3, m_vbo);
            glBindVertexArray(m_vao[0]);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_vertex, loc_vertex, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_vertex, loc_vertex, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                                  (void *) nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                                  (void *) (3 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            // texture
            glGenTextures(2, m_texture);
            for (unsigned int i: m_texture) {
                glBindTexture(GL_TEXTURE_2D, i);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            glBindTexture(GL_TEXTURE_2D, m_texture[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, m_texture[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            // fbo
            glGenFramebuffers(1, &m_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glBindTexture(GL_TEXTURE_2D, m_texture[1]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   m_texture[1], 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        gl_check(__LINE__);
    }
    {
        glEnable(GL_DEPTH_TEST);
    }
    { // offscreen draw
        { // use gl_program
            glUseProgram(m_program[0]);
        }
        { // vao
            glBindVertexArray(m_vao[0]);
        }
        { // texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture[0]);
            GLint tex_sample_index = glGetUniformLocation(m_program[0], "TexSample");
            glUniform1i(tex_sample_index, 0);
        }
        { // draw
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glViewport(0, 0, m_r_width, m_r_height);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        gl_check(__LINE__);
    }
    { // onscreen draw
//        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glClearColor(1.0, 1.0, 1.0, 1.0);
        { // use program
            glUseProgram(m_program[1]);
        }
        { // vao
            glBindVertexArray(m_vao[0]);
        }
        { // texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture[1]);
            GLint sample_tex_index = glGetUniformLocation(m_program[1], "TexSample");
            glUniform1i(sample_tex_index, 0);
        }
        { // draw
            glViewport(0, 0, m_r_width, m_r_height);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        gl_check(__LINE__);
    }
}

GLuint gl3Light::gl_program_create(const char *vertex, const char *fragment) {
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

GLuint gl3Light::gl_shader_create(GLenum gl_type, const char *gl_source) {
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

void gl3Light::gl_check(int line) {
    GLenum err = glGetError();
    if (!err) return;
    LOGE("gl_check error=%d, line=%d", glGetError(), line);
}