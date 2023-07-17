#include "gl3_mode_obj.h"

#include <utility>

/**
 * todo obj模型中(顶点/纹理/法线)3者索引各不相同 无法直接使用ebo绘制 则该例只展示顶点数据
 * 即 面数据为 f 1/2/3 则不能直接使用ebo绘制 需 根据索引重排数据
 * 如 面数据为 f 1/1/1 则可直接使用ebo绘制
 */

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
            LOGE("gl init start");
            initial = true;
            m_vec_vertex = std::vector<float>();
            m_vec_texture = std::vector<float>();
            m_vec_indices = std::vector<std::vector<int>>();
            obj_parse(mode_path, &m_vec_vertex, &m_vec_texture, &m_vec_indices);
            LOGE("vertex size=%d", m_vec_vertex.size());
            LOGE("texture size=%d", m_vec_texture.size());
            LOGE("indices size=%d", m_vec_indices.size());
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
            glGenBuffers(2, m_fbo_vbo);
            glBindVertexArray(m_fbo_vao[0]);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[0]);
            glBufferData(GL_ARRAY_BUFFER,
                         (GLsizei) (sizeof(GLfloat) * m_vec_vertex.size()),
                         m_vec_vertex.data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[1]);
            glBufferData(GL_ARRAY_BUFFER,
                         (GLsizei) (sizeof(GLfloat) * m_vec_texture.size()),
                         m_vec_texture.data(),
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[0]);
            glVertexAttribPointer(0, 3,
                                  GL_FLOAT, GL_FALSE,
                                  3 * sizeof(GLfloat), (void *) nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, m_fbo_vbo[1]);
            glVertexAttribPointer(1, 2,
                                  GL_FLOAT, GL_FALSE,
                                  3 * sizeof(GLfloat), (void *) nullptr);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
            // offscreen ebo
            for (int i = 0; i < m_vec_indices.size(); i++) {
                m_fbo_ebo.push_back(0);
                glGenBuffers(1, &m_fbo_ebo[i]);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fbo_ebo[i]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             (GLsizei) (sizeof(GLint) * m_vec_indices[i].size()),
                             m_vec_indices[i].data(),
                             GL_STATIC_DRAW);
            }
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
            glBindVertexArray(0);
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
            LOGE("gl init end");
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
            model = glm::rotate(model, glm::radians(-75.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));
            auto model_index = glGetUniformLocation(m_fbo_program[0], "model");
            glUniformMatrix4fv(model_index, 1, GL_FALSE, glm::value_ptr(model));
            // view(camera)
            glm::mat4 view = glm::mat4(1.0);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
            auto view_index = glGetUniformLocation(m_fbo_program[0], "view");
            glUniformMatrix4fv(view_index, 1, GL_FALSE, glm::value_ptr(view));
            // projection(crop)
            float aspect = (float) m_r_width / (float) m_r_height;
            glm::mat4 proj = glm::mat4(1.0);
            proj = glm::perspective(glm::radians(45.0f), aspect, 1.0f, 1000.0f);
            auto proj_index = glGetUniformLocation(m_fbo_program[0], "projection");
            glUniformMatrix4fv(proj_index, 1, GL_FALSE, glm::value_ptr(proj));
        }
        { // draw
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[0]);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
//            glDrawArrays(GL_TRIANGLES, 0, (GLsizei) m_vec_vertex.size());
            for (int i = 0; i < m_vec_indices.size(); i++) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fbo_ebo[i]);
                glDrawElements(GL_TRIANGLES,
                               (GLsizei) m_vec_indices[i].size(),
                               GL_UNSIGNED_INT,
                               (const void *) nullptr);
            }
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
                          std::vector<float> *vec_vertex,
                          std::vector<float> *vec_texture,
                          std::vector<std::vector<int>> *vec_indices) {
    std::ifstream file_model(model_path);
    if (!file_model.is_open()) {
        return -1;
    }
    auto *indices = new std::vector<int>();
    float vertex_ratio;
    std::string line_str;
    while (std::getline(file_model, line_str)) {
        std::vector<std::string> ret = string_split(line_str, " ");
        if (ret.size() == 3) {
            if (ret[0].find('#') != std::string::npos &&
                ret[2].find("faces") != std::string::npos) {
                vec_indices->push_back(*indices);
                indices = new std::vector<int>();
            }
        }
        if (ret.empty() || ret.size() <= 3) {
            continue;
        }
        if (ret[0] == "v") {
            vec_vertex->push_back(std::stof(ret[1]));
            vec_vertex->push_back(std::stof(ret[2]));
            vec_vertex->push_back(std::stof(ret[3]));
            vertex_ratio = std::max(std::abs(std::stof(ret[1])), vertex_ratio);
            vertex_ratio = std::max(std::abs(std::stof(ret[2])), vertex_ratio);
            vertex_ratio = std::max(std::abs(std::stof(ret[3])), vertex_ratio);
        } else if (ret[0] == "vt") {
            vec_texture->push_back(std::stof(ret[1]));
            vec_texture->push_back(std::stof(ret[2]));
            vec_texture->push_back(std::stof(ret[3]));
        } else if (ret[0] == "f") {
            for (int i = 1; i < ret.size(); i++) {
                std::vector<std::string> ret_f = string_split(ret[i], "/");
                if (ret_f.size() < 3) {
                    continue;
                }
                indices->push_back(std::stoi(ret_f[0]));
                /**
                 * todo obj模型中 顶点/纹理/法线 3者索引各不相同 无法使用ebo绘制
                 */
                /*indices->push_back(std::stoi(ret_f[1]));
                indices->push_back(std::stoi(ret_f[2]));*/
            }
        }
    }
    // 顶点归一化
    for (auto &i: *vec_vertex) {
        i /= vertex_ratio;
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
        if (content_chr[i] == s[0]) {
            if (!tmp_str.empty()) {
                ret->push_back(tmp_str);
            }
            tmp_str = "";
            continue;
        }
        tmp_str += content_chr[i];
        if (i == content_len - 1) {
            if (!tmp_str.empty()) {
                ret->push_back(tmp_str);
            }
            tmp_str = "";
            continue;
        }
    }
    return *ret;
}
