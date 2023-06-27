#include "gl3_light.h"

void gl3Light::update_viewport(int width, int height) {
    m_r_width = width;
    m_r_height = height;
    glViewport(0, 0, width, height);
}

void gl3Light::gl_light_draw_vao_fbo_camera(int width, int height, uint8_t *data) {
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
    const char *gl_shader_fbo_vex_plane_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    layout(location = 0) in vec4 aPosition;
                    layout(location = 1) in vec3 aNormal;
                    layout(location = 2) in vec2 aTexCoord;
                    uniform mat4 model;//模型: 世界空间
                    uniform mat4 view; //视图: 观察空间
                    uniform mat4 projection;//射影: 裁剪空间
                    out vec2 TexCoord;
                    out vec3 Normal;
                    void main() {
                        gl_Position = projection * view * model * aPosition;
                        TexCoord = aTexCoord;
                    }
            );
    const char *gl_shader_fbo_frag_plane_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    in vec2 TexCoord;
                    in vec3 Normal;
                    uniform sampler2D TexSample;
                    uniform vec3 lightPos;
                    layout(location = 0) out vec4 fragColor;
                    void main() {
//                        vec3 norm = normalize(Normal);
//                        vec3 lightDir = normalize(lightPos - FragPos);
                        fragColor = texture(TexSample, TexCoord);
                    }
            );
    float loc_fbo[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    glm::vec3 cube_pos[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
    };
    float loc_screen[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 1.0f,

            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    };
    if (m_r_width == 0) {
        return;
    }
    { // config
        if (!initial) {
            initial = true;
            // gl_program
            m_program[0] = gl_program_create(
                    gl_shader_fbo_vex_plane_source,
                    gl_shader_fbo_frag_plane_source
            );
            m_program[1] = gl_program_create(
                    gl_shader_vertex_plane_source,
                    gl_shader_fragment_plane_source
            );
            gl_check(__LINE__);
            if (m_program[0] == GL_NONE || m_program[1] == GL_NONE) return;
            // fbo: vbo & vao
            glGenVertexArrays(2, m_vao);
            glGenBuffers(5, m_vbo);
            glBindVertexArray(m_vao[0]);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_fbo, loc_fbo, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_fbo, loc_fbo, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_fbo, loc_fbo, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                                  (void *) nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                                  (void *) (3 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                                  (void *) (6 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            // screen: vbo & vao
            glBindVertexArray(m_vao[1]);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_screen, loc_screen, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[4]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_screen, loc_screen, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[3]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                                  (void *) nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[4]);
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_r_width, m_r_height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            // fbo
            glGenFramebuffers(1, &m_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glBindTexture(GL_TEXTURE_2D, m_texture[1]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   m_texture[1], 0);
            // fbo.rbo
            glGenRenderbuffers(1, &m_fbo_rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_r_width, m_r_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                      m_fbo_rbo);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGE("fbo != GL_FRAMEBUFFER_COMPLETE %d", __LINE__);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        gl_check(__LINE__);
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
        { // Camera
            m_rotation += 0.1f;
            // model(world)
            glm::mat4 model = glm::mat4(1.0);
            model = glm::translate(model, cube_pos[0]);
            model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.9, 0.9, 0.9));
            auto model_index = glGetUniformLocation(m_program[0], "model");
            glUniformMatrix4fv(model_index, 1, GL_FALSE, glm::value_ptr(model));
            // view(camera)
            glm::mat4 view = glm::mat4(1.0);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            view = glm::rotate(view, glm::radians(m_rotation), glm::vec3(1.0f, 1.0f, 0.0f));
            auto view_index = glGetUniformLocation(m_program[0], "view");
            glUniformMatrix4fv(view_index, 1, GL_FALSE, glm::value_ptr(view));
            // projection(crop)
            glm::mat4 proj = glm::mat4(1.0);
            proj = glm::perspective(glm::radians(45.0f), 9.0f / 16.0f, 1.0f, 1000.0f);
            auto proj_index = glGetUniformLocation(m_program[0], "projection");
            glUniformMatrix4fv(proj_index, 1, GL_FALSE, glm::value_ptr(proj));
        }
        { // draw
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGE("fbo != GL_FRAMEBUFFER_COMPLETE %d", __LINE__);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        gl_check(__LINE__);
    }
    { // onscreen draw
        { // use program
            glUseProgram(m_program[1]);
        }
        { // vao
            glBindVertexArray(m_vao[1]);
        }
        { // texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture[1]);
            GLint sample_tex_index = glGetUniformLocation(m_program[1], "TexSample");
            glUniform1i(sample_tex_index, 0);
        }
        { // draw
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 6);
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