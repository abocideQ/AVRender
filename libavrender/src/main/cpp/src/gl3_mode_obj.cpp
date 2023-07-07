#include "gl3_mode_obj.h"

#include <utility>

void gl3ModeObj::update_viewport(int width, int height) {
    m_r_width = width;
    m_r_height = height;
}

void gl3ModeObj::gl_mode_obj(int width, int height, std::string mode_path, std::string tex_path) {
    if (m_r_width == 0 || m_r_height == 0) {
        return;
    }
    { // config init
        if (!initial) {
            initial = true;
            m_vec_vertex = std::vector<std::vector<float>>();
            m_vec_texture = std::vector<std::vector<float>>();
            m_vec_indices = std::vector<std::vector<float>>();
            obj_parse(mode_path,
                      &m_vec_vertex,
                      &m_vec_texture,
                      &m_vec_indices);
            LOGE("vertex  size = %d", m_vec_vertex.size());
            LOGE("texture size = %d", m_vec_texture.size());
            LOGE("indices size = %d", m_vec_indices.size());
        }
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
        std::vector<std::string> result = string_split(line_str);
        if (result.empty()) {
            continue;
        }
        if (result[0] == "v" && result.size() > 3) {
            auto *vertex = new std::vector<float>();
            vertex->push_back(std::stof(result[1]));
            vertex->push_back(std::stof(result[2]));
            vertex->push_back(std::stof(result[3]));
            vec_vertex->push_back(*vertex);
        } else if (result[0] == "vt" && result.size() > 3) {
            auto *texture = new std::vector<float>();
            texture->push_back(std::stof(result[1]));
            texture->push_back(std::stof(result[2]));
            texture->push_back(std::stof(result[3]));
            vec_texture->push_back(*texture);
        } else if (result[0] == "f" && result.size() > 3) {
            auto *indices = new std::vector<float>();
            indices->push_back(std::stof(result[1]));
            indices->push_back(std::stof(result[2]));
            indices->push_back(std::stof(result[3]));
            vec_indices->push_back(*indices);
        }
    }
    file_model.close();
    return 0;
}

std::vector<std::string> gl3ModeObj::string_split(const std::string &content) {
    auto *result = new std::vector<std::string>();
    if (content.empty()){
        return *result;
    }
    auto *content_chr = content.c_str();
    std::string temp_str;
    for (int i = 0; i < content.size(); i++) {
        if (content_chr[i] == ' ') {
            if (!temp_str.empty()) {
                result->push_back(temp_str);
            }
            temp_str = "";
            continue;
        }
        temp_str += content_chr[i];
    }
    return *result;
}
