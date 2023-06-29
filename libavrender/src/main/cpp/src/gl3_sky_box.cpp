#include "gl3_sky_box.h"

void gl3SkyBox::update_viewport(int width, int height) {
    m_r_width = width;
    m_r_height = height;
    glViewport(0, 0, width, height);
}

void gl3SkyBox::gl_sky_box(int width, int height, uint8_t *data,
                           int sky_w, int sky_h,
                           uint8_t *sky_right,
                           uint8_t *sky_left,
                           uint8_t *sky_top,
                           uint8_t *sky_bottom,
                           uint8_t *sky_front,
                           uint8_t *sky_back) {
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
                    out vec3 FragPos;
                    out vec3 Normal;
                    void main() {
                        // texture & light
                        TexCoord = aTexCoord;
                        FragPos = vec3(model * aPosition);
                        Normal = mat3(transpose(inverse(model))) * aNormal;
                        // gl_pos
                        gl_Position = projection * view * model * aPosition;
                    }
            );
    const char *gl_shader_fbo_frag_plane_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    in vec2 TexCoord;
                    in vec3 Normal;
                    in vec3 FragPos;
                    uniform vec3 ambientLightColor;
                    uniform vec3 diffuseLightPos;
                    uniform vec3 diffuseLightColor;
                    uniform vec3 specularLightPos;
                    uniform vec3 specularLightColor;
                    uniform vec3 specularCameraPos;
                    uniform sampler2D TexSample;
                    layout(location = 0) out vec4 fragColor;
                    void main() {
                        // ambient
                        float ambientStrength = 0.1;
                        vec3 ambient = vec3(ambientStrength) * ambientLightColor;
                        // diffuse
                        float diffuseStrength = 0.8;
                        vec3 diffuseLightDir = normalize(diffuseLightPos - FragPos);
                        float diffuseDiff = max(dot(normalize(Normal), diffuseLightDir), 0.0);
                        vec3 diffuse = vec3(diffuseStrength) *
                                       vec3(diffuseDiff) *
                                       diffuseLightColor;
                        // specular
                        float specularStrength = 1.0;
                        vec3 specularCameraDir = normalize(specularCameraPos - FragPos);
                        vec3 specularLightDir = normalize(specularLightPos - FragPos);
                        vec3 specularLightReflect = reflect(-specularLightDir, normalize(Normal));
                        float specularDiff = 1.0;
                        float a = max(dot(specularCameraDir, specularLightReflect), 0.0);
                        for (int i = 0; i < 32; i++) {
                            specularDiff *= a;
                        }
                        vec3 specular = specularStrength * specularDiff * specularLightColor;
                        // fragColor
                        fragColor = texture(TexSample, TexCoord) *
                                    vec4(ambient + diffuse + specular, 1.0);
                    }
            );
    const char *gl_shader_fbo_vex_sky_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    layout(location = 0) in vec3 aPosition;
                    uniform mat4 projection;
                    uniform mat4 view;
                    out vec3 TexCoord;
                    void main() {
                        vec4 pos = projection * view * vec4(aPosition, 1.0);
                        gl_Position = pos.xyww;
                        TexCoord = aPosition;
                    }
            );
    const char *gl_shader_fbo_frag_sky_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    in vec3 TexCoord;
                    uniform samplerCube TexSample;
                    layout(location = 0) out vec4 fragColor;
                    void main() {
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
    float loc_sky_box[] = {
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
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
            m_program_sky = gl_program_create(
                    gl_shader_fbo_vex_sky_source,
                    gl_shader_fbo_frag_sky_source
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
            // sky : vbo & vao
            glGenVertexArrays(1, &m_vao_sky);
            glGenBuffers(1, &m_vbo_sky);
            glBindVertexArray(m_vao_sky);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo_sky);
            glBufferData(GL_ARRAY_BUFFER, sizeof loc_sky_box, loc_sky_box, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                                  (void *) nullptr);
            glEnableVertexAttribArray(0);
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
            // sky: texture
            glGenTextures(1, &m_texture_sky);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_sky);
            int i = 0;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGBA, sky_w, sky_h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, sky_left);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGBA, sky_w, sky_h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, sky_right);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGBA, sky_w, sky_h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, sky_top);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGBA, sky_w, sky_h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, sky_bottom);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGBA, sky_w, sky_h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, sky_front);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, 0, GL_RGBA, sky_w, sky_h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, sky_back);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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
    { // offscreen draw sky
        { // use program
            glUseProgram(m_program_sky);
        }
        gl_check(__LINE__);
        { // vao
            glBindVertexArray(m_vao_sky);
        }
        gl_check(__LINE__);
        { // texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_sky);
            GLint tex_sample_index = glGetUniformLocation(m_program_sky, "TexSample");
            glUniform1i(tex_sample_index, 0);
        }
        gl_check(__LINE__);
        {
            m_rotation++;
            // view(camera)
            glm::mat4 view = glm::mat4(1.0);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
            view = glm::rotate(view, glm::radians(m_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
            auto view_index = glGetUniformLocation(m_program_sky, "view");
            glUniformMatrix4fv(view_index, 1, GL_FALSE, glm::value_ptr(view));
            // projection(crop)
            glm::mat4 proj = glm::mat4(1.0);
            proj = glm::perspective(glm::radians(45.0f), 9.0f / 16.0f, 1.0f, 1000.0f);
            auto proj_index = glGetUniformLocation(m_program_sky, "projection");
            glUniformMatrix4fv(proj_index, 1, GL_FALSE, glm::value_ptr(proj));
        }
        gl_check(__LINE__);
        { // draw
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDisable(GL_DEPTH_TEST);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGE("fbo != GL_FRAMEBUFFER_COMPLETE %d", __LINE__);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        gl_check(__LINE__);
    }
    { // offscreen draw box
/*        { // use gl_program
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
            m_rotation += 0.5;
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
            *//*view = glm::rotate(view, glm::radians(m_rotation), glm::vec3(1.0f, 1.0f, 0.0f));*//*
            auto view_index = glGetUniformLocation(m_program[0], "view");
            glUniformMatrix4fv(view_index, 1, GL_FALSE, glm::value_ptr(view));
            // projection(crop)
            glm::mat4 proj = glm::mat4(1.0);
            proj = glm::perspective(glm::radians(45.0f), 9.0f / 16.0f, 1.0f, 1000.0f);
            auto proj_index = glGetUniformLocation(m_program[0], "projection");
            glUniformMatrix4fv(proj_index, 1, GL_FALSE, glm::value_ptr(proj));
        }
        { // Light
            glm::vec3 ambientLightColor = glm::vec3(1.0, 1.0, 1.0);
            glm::vec3 diffuseLightPos = glm::vec3(0.0, 0.0, 3.0);
            glm::vec3 diffuseLightColor = glm::vec3(1.0, 1.0, 1.0);
            glm::vec3 specularLightPos = glm::vec3(0.0, 0.0, 3.0);
            glm::vec3 specularLightColor = glm::vec3(1.0, 1.0, 1.0);
            glm::vec3 specularCameraPos = glm::vec3(0.0, 0.0, 3.0);
            auto ambientLightColor_i = glGetUniformLocation(m_program[0], "ambientLightColor");
            auto diffuseLightPos_i = glGetUniformLocation(m_program[0], "diffuseLightPos");
            auto diffuseLightColor_i = glGetUniformLocation(m_program[0], "diffuseLightColor");
            auto specularLightPos_i = glGetUniformLocation(m_program[0], "specularLightPos");
            auto specularLightColor_i = glGetUniformLocation(m_program[0], "specularLightColor");
            auto specularCameraPos_i = glGetUniformLocation(m_program[0], "specularCameraPos");
            glUniform3fv(ambientLightColor_i, 1, glm::value_ptr(ambientLightColor));
            glUniform3fv(diffuseLightPos_i, 1, glm::value_ptr(diffuseLightPos));
            glUniform3fv(diffuseLightColor_i, 1, glm::value_ptr(diffuseLightColor));
            glUniform3fv(specularLightPos_i, 1, glm::value_ptr(specularLightPos));
            glUniform3fv(specularLightColor_i, 1, glm::value_ptr(specularLightColor));
            glUniform3fv(specularCameraPos_i, 1, glm::value_ptr(specularCameraPos));
        }
        { // draw
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDisable(GL_DEPTH_TEST);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGE("fbo != GL_FRAMEBUFFER_COMPLETE %d", __LINE__);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        gl_check(__LINE__);*/
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

GLuint gl3SkyBox::gl_program_create(const char *vertex, const char *fragment) {
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

GLuint gl3SkyBox::gl_shader_create(GLenum gl_type, const char *gl_source) {
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

void gl3SkyBox::gl_check(int line) {
    GLenum err = glGetError();
    if (!err) return;
    LOGE("gl_check error=%d, line=%d", glGetError(), line);
}