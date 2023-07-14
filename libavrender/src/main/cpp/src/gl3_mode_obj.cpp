#include "gl3_mode_obj.h"

#include <utility>

void gl3ModeObj::update_viewport(int width, int height) {
    m_r_width = width;
    m_r_height = height;
}

void gl3ModeObj::gl_mode_obj(int width, int height, std::string &mode_path, std::string &tex_path) {
    if (m_r_width == 0 || m_r_height == 0) {
        return;
    }
    const char *gl_shader_fbo_vex_plane_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    layout(location = 0) in vec4 aPosition;
                    layout(location = 1) in vec2 aTexCoord;
                    uniform mat4 model;
                    uniform mat4 view;
                    uniform mat4 projection;
                    out vec2 TexCoord;
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
                    uniform sampler2D TexSample;
                    layout(location = 0) out vec4 fragColor;
                    void main() {
                        fragColor = vec4(0.5, 0.5, 0.5, 1.0);
                    }
            );
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
    float loc_fbo[] = {
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
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    };
    float loc_screen[] = {
            -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 1.0f,

            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    };
    { // config init
        if (!initial) {
            initial = true;
            m_vec_vertex = std::vector<std::vector<float>>();
            m_vec_texture = std::vector<std::vector<float>>();
            m_vec_indices = std::vector<std::vector<float>>();
            obj_parse(mode_path, &m_vec_vertex, &m_vec_texture, &m_vec_indices);
            // gl_program
            m_fbo_program[0] = gl_program_create(
                    gl_shader_fbo_vex_plane_source,
                    gl_shader_fbo_frag_plane_source
            );
            m_program[0] = gl_program_create(
                    gl_shader_vertex_plane_source,
                    gl_shader_fragment_plane_source
            );
            gl_check(__LINE__);
            if (m_program[0] == GL_NONE || m_fbo_program[0] == GL_NONE) return;
            // fbo: vbo & vao
            glGenVertexArrays(1, m_fbo_vao);
            glGenBuffers(3, m_fbo_vbo);
            glBindVertexArray(m_fbo_vao[0]);
            /*glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[0]);
            glBufferData(GL_ARRAY_BUFFER,
                         (GLsizei) (sizeof(GLfloat) * m_vec_vertex.size()),
                         m_vec_vertex.data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[1]);
            glBufferData(GL_ARRAY_BUFFER,
                         (GLsizei) (sizeof(GLfloat) * m_vec_texture.size()),
                         m_vec_texture.data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fbo_vbo[2]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         (GLsizei) (sizeof(GLfloat) * m_vec_indices.size()),
                         m_vec_indices.data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[0]);
            glVertexAttribPointer(0, 3,
                                  GL_FLOAT, GL_FALSE,
                                  3 * sizeof(GLfloat), (void *) nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[1]);
            glVertexAttribPointer(1, 2,
                                  GL_FLOAT, GL_FALSE,
                                  3 * sizeof(GLfloat), (void *) nullptr);*/
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_fbo, loc_fbo, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_fbo, loc_fbo, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                                  (void *) nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[1]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                                  (void *) (3 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            // screen: vbo & vao
            glGenVertexArrays(1, m_vao);
            glGenBuffers(2, m_vbo);
            glBindVertexArray(m_vao[0]);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof loc_screen,
                         loc_screen,
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof loc_screen,
                         loc_screen,
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
            glVertexAttribPointer(0, 3,
                                  GL_FLOAT, GL_FALSE,
                                  5 * sizeof(GLfloat), (void *) nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
            glVertexAttribPointer(1, 2,
                                  GL_FLOAT, GL_FALSE,
                                  5 * sizeof(GLfloat), (void *) (3 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            // texture
            glGenTextures(1, m_fbo_tex);
            glBindTexture(GL_TEXTURE_2D, m_fbo_tex[0]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenTextures(1, m_tex);
            glBindTexture(GL_TEXTURE_2D, m_tex[0]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            for (int i = 0; i < 1; i++) {
                int w, h, nrChannels;
                unsigned char *data = stbi_load(tex_path.c_str(), &w, &h, &nrChannels, 0);
                if (!data) {
                    LOGE("stbi_load fail......");
                    break;
                }
                glBindTexture(GL_TEXTURE_2D, m_fbo_tex[0]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
                             0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            glBindTexture(GL_TEXTURE_2D, m_tex[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_r_width, m_r_height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            // fbo
            glGenFramebuffers(1, m_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[0]);
            glBindTexture(GL_TEXTURE_2D, m_tex[0]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   m_tex[0], 0);
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
    { // offscreen draw box
        { // use gl_program
            glUseProgram(m_fbo_program[0]);
        }
        { // vao
            glBindVertexArray(m_fbo_vao[0]);
        }
        { // texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_fbo_tex[0]);
            GLint tex_sample_index = glGetUniformLocation(m_fbo_program[0], "TexSample");
            glUniform1i(tex_sample_index, 0);
        }
        { // Camera
            m_rotation += 0.2;
            // model(world)
            glm::mat4 model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));
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
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[0]);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
//            glDrawElements(GL_TRIANGLES,
//                           (GLsizei) m_vec_indices.size(),
//                           GL_UNSIGNED_INT,
//                           (const void *) nullptr);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDisable(GL_DEPTH_TEST);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGE("fbo != GL_FRAMEBUFFER_COMPLETE %d", __LINE__);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        gl_check(__LINE__);
    }
    { // onscreen draw
        { // use program
            glUseProgram(m_program[0]);
        }
        { // vao
            glBindVertexArray(m_vao[0]);
        }
        { // texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_tex[0]);
            GLint sample_tex_index = glGetUniformLocation(m_program[0], "TexSample");
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

GLuint gl3ModeObj::gl_program_create(const char *vertex, const char *fragment) {
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

GLuint gl3ModeObj::gl_shader_create(GLenum gl_type, const char *gl_source) {
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

void gl3ModeObj::gl_check(int line) {
    GLenum err = glGetError();
    if (!err) return;
    LOGE("gl_check error=%d, line=%d", glGetError(), line);
}

int gl3ModeObj::obj_parse(const std::string &model_path,
                          std::vector<std::vector<float>> *vec_vertex,
                          std::vector<std::vector<float>> *vec_texture,
                          std::vector<std::vector<float>> *vec_indices) {
    std::ifstream file_model(model_path);
    if (!file_model.is_open()) {
        return -1;
    }
    std::string line_str;
    while (std::getline(file_model, line_str)) {
        std::vector<std::string> ret = string_split(line_str, " ");
        if (ret.empty()) {
            continue;
        }
        if (ret[0] == "v" && ret.size() > 3) {
            auto *vertex = new std::vector<float>();
            vertex->push_back(std::stof(ret[1]));
            vertex->push_back(std::stof(ret[2]));
            vertex->push_back(std::stof(ret[3]));
            vec_vertex->push_back(*vertex);
        } else if (ret[0] == "vt" && ret.size() > 3) {
            auto *texture = new std::vector<float>();
            texture->push_back(std::stof(ret[1]));
            texture->push_back(std::stof(ret[2]));
            texture->push_back(std::stof(ret[3]));
            vec_texture->push_back(*texture);
        } else if (ret[0] == "f" && ret.size() > 3) {
            for (int i = 1; i < ret.size(); i++) {
                std::vector<std::string> ret_f = string_split(ret[i], "/");
                if (ret_f.size() < 3) {
                    continue;
                }
                auto *indices = new std::vector<float>();
                indices->push_back(std::stof(ret_f[0]));
                indices->push_back(std::stof(ret_f[1]));
                indices->push_back(std::stof(ret_f[2]));
                vec_indices->push_back(*indices);
            }
        }
    }
    file_model.close();
    return 0;
}

std::vector<std::string> gl3ModeObj::string_split(const std::string &content, const char *s) {
    auto *ret = new std::vector<std::string>();
    if (content.empty()) {
        return *ret;
    }
    auto *content_chr = content.c_str();
    auto content_len = content.length();
    std::string tmp_str;
    for (int i = 0; i < content_len; i++) {
        if (content_chr[i] == s[0] || i == content_len - 1) {
            if (!tmp_str.empty()) {
                ret->push_back(tmp_str);
            }
            tmp_str = "";
            continue;
        }
        tmp_str += content_chr[i];
    }
    return *ret;
}
