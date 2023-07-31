#include "gl3_sea.h"

void gl3Sea::update_viewport(int width, int height) {
    m_r_width = width;
    m_r_height = height;
}

void gl3Sea::gl_sea() {
    if (m_r_width == 0 || m_r_height == 0) {
        return;
    }
    float loc_vertex[] = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
    };
    float loc_tex[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f
    };
    const char *gl_shader_vertex_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    layout(location = 0) in vec4 aPosition;
                    layout(location = 1) in vec2 aTexCoord;
                    out vec2 TexCoord;
                    void main() {
                        gl_Position = vec4(aPosition.xyz, 1.0);
                        TexCoord = (1.0 - aTexCoord);
                    }
            );
    const char *gl_shader_fragment_source =
            GL_SHADER_VERSION300
            GET_CHAR(
                    precision highp float;
                    in vec2 TexCoord;
                    uniform vec2 iResolution;
                    uniform float iTime;
                    layout(location = 0) out vec4 fragColor;
                    // var
                    const int NUM_STEPS = 8;
                    const float PI = 3.141592;
                    const float EPSILON = 1e-3;
                    float EPSILON_NRM = 0.0; // main() init
                    const int ITER_GEOMETRY = 3;
                    const int ITER_FRAGMENT = 5;
                    const float SEA_HEIGHT = 0.6;
                    const float SEA_CHOPPY = 4.0;
                    const float SEA_SPEED = 0.8;
                    const float SEA_FREQ = 0.16;
                    const vec3 SEA_BASE = vec3(0.0, 0.09, 0.18);
                    const vec3 SEA_WATER_COLOR = vec3(0.8, 0.9, 0.6) * 0.6;
                    const mat2 octave_m = mat2(1.6, 1.2, -1.2, 1.6);
                    float SEA_TIME = 0.0; // main() init
                    // math
                    mat3 fromEuler(vec3 ang) {
                        vec2 a1 = vec2(sin(ang.x), cos(ang.x));
                        vec2 a2 = vec2(sin(ang.y), cos(ang.y));
                        vec2 a3 = vec2(sin(ang.z), cos(ang.z));
                        mat3 m;
                        m[0] = vec3(a1.y * a3.y + a1.x * a2.x * a3.x,
                                    a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x);
                        m[1] = vec3(-a2.y * a1.x, a1.y * a2.y, a2.x);
                        m[2] = vec3(a3.y * a1.x * a2.x + a1.y * a3.x,
                                    a1.x * a3.x - a1.y * a3.y * a2.x, a2.y * a3.y);
                        return m;
                    }
                    float hash(vec2 p) {
                        float h = dot(p, vec2(127.1, 311.7));
                        return fract(sin(h) * 43758.5453123);
                    }
                    float noise(in vec2 p) {
                        vec2 i = floor(p);
                        vec2 f = fract(p);
                        vec2 u = f * f * (3.0 - 2.0 * f);
                        return -1.0 + 2.0 * mix(mix(hash(i + vec2(0.0, 0.0)),
                                                    hash(i + vec2(1.0, 0.0)), u.x),
                                                mix(hash(i + vec2(0.0, 1.0)),
                                                    hash(i + vec2(1.0, 1.0)), u.x), u.y);
                    }
                    // lighting
                    float diffuse(vec3 n, vec3 l, float p) {
                        return pow(dot(n, l) * 0.4 + 0.6, p);
                    }
                    float specular(vec3 n, vec3 l, vec3 e, float s) {
                        float nrm = (s + 8.0) / (PI * 8.0);
                        return pow(max(dot(reflect(e, n), l), 0.0), s) * nrm;
                    }
                    // sky
                    vec3 getSkyColor(vec3 e) {
                        e.y = (max(e.y, 0.0) * 0.8 + 0.2) * 0.8;
                        return vec3(pow(1.0 - e.y, 2.0), 1.0 - e.y, 0.6 + (1.0 - e.y) * 0.4) * 1.1;
                    }
                    // sea
                    float sea_octave(vec2 uv, float choppy) {
                        uv += noise(uv);
                        vec2 wv = 1.0 - abs(sin(uv));
                        vec2 swv = abs(cos(uv));
                        wv = mix(wv, swv, wv);
                        return pow(1.0 - pow(wv.x * wv.y, 0.65), choppy);
                    }
                    float map(vec3 p) {
                        float freq = SEA_FREQ;
                        float amp = SEA_HEIGHT;
                        float choppy = SEA_CHOPPY;
                        vec2 uv = p.xz;
                        uv.x *= 0.75;
                        float d = 0.0;
                        float h = 0.0;
                        for (int i = 0; i < ITER_GEOMETRY; i++) {
                            d = sea_octave((uv + SEA_TIME) * freq, choppy);
                            d += sea_octave((uv - SEA_TIME) * freq, choppy);
                            h += d * amp;
                            uv *= octave_m;
                            freq *= 1.9;
                            amp *= 0.22;
                            choppy = mix(choppy, 1.0, 0.2);
                        }
                        return p.y - h;
                    }
                    float map_detailed(vec3 p) {
                        float freq = SEA_FREQ;
                        float amp = SEA_HEIGHT;
                        float choppy = SEA_CHOPPY;
                        vec2 uv = p.xz;
                        uv.x *= 0.75;
                        float d = 0.0;
                        float h = 0.0;
                        for (int i = 0; i < ITER_FRAGMENT; i++) {
                            d = sea_octave((uv + SEA_TIME) * freq, choppy);
                            d += sea_octave((uv - SEA_TIME) * freq, choppy);
                            h += d * amp;
                            uv *= octave_m;
                            freq *= 1.9;
                            amp *= 0.22;
                            choppy = mix(choppy, 1.0, 0.2);
                        }
                        return p.y - h;
                    }
                    vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {
                        float fresnel = clamp(1.0 - dot(n, -eye), 0.0, 1.0);
                        fresnel = min(pow(fresnel, 3.0), 0.5);
                        vec3 reflected = getSkyColor(reflect(eye, n));
                        vec3 refracted = SEA_BASE + diffuse(n, l, 80.0) * SEA_WATER_COLOR * 0.12;
                        vec3 color = mix(refracted, reflected, fresnel);
                        float atten = max(1.0 - dot(dist, dist) * 0.001, 0.0);
                        color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
                        color += vec3(specular(n, l, eye, 60.0));
                        return color;
                    }
                    // tracing
                    vec3 getNormal(vec3 p, float eps) {
                        vec3 n;
                        n.y = map_detailed(p);
                        n.x = map_detailed(vec3(p.x + eps, p.y, p.z)) - n.y;
                        n.z = map_detailed(vec3(p.x, p.y, p.z + eps)) - n.y;
                        n.y = eps;
                        return normalize(n);
                    }
                    float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {
                        float tm = 0.0;
                        float tx = 1000.0;
                        float hx = map(ori + dir * tx);
                        if (hx > 0.0) {
                            p = ori + dir * tx;
                            return tx;
                        }
                        float hm = map(ori + dir * tm);
                        float tmid = 0.0;
                        for (int i = 0; i < NUM_STEPS; i++) {
                            tmid = mix(tm, tx, hm / (hm - hx));
                            p = ori + dir * tmid;
                            float hmid = map(p);
                            if (hmid < 0.0) {
                                tx = tmid;
                                hx = hmid;
                            } else {
                                tm = tmid;
                                hm = hmid;
                            }
                        }
                        return tmid;
                    }
                    vec3 getPixel(in vec2 coord, float time) {
                        vec2 uv = coord / iResolution.xy;
                        uv = uv * 2.0 - 1.0;
                        uv.x *= iResolution.x / iResolution.y;
                        // ray
                        vec3 ang = vec3(sin(time * 3.0) * 0.1, sin(time) * 0.2 + 0.3, time);
                        vec3 ori = vec3(0.0, 3.5, time * 5.0);
                        vec3 dir = normalize(vec3(uv.xy, -2.0));
                        dir.z += length(uv) * 0.14;
                        dir = normalize(dir) * fromEuler(ang);
                        // tracing
                        vec3 p;
                        heightMapTracing(ori, dir, p);
                        vec3 dist = p - ori;
                        vec3 n = getNormal(p, dot(dist, dist) * EPSILON_NRM);
                        vec3 light = normalize(vec3(0.0, 1.0, 0.8));
                        // color
                        return mix(
                                getSkyColor(dir),
                                getSeaColor(p, n, light, dir, dist),
                                pow(smoothstep(0.0, -0.02, dir.y), 0.2));
                    }
                    //main
                    void main() {
                        EPSILON_NRM = (0.1 / iResolution.x);
                        SEA_TIME = 1.0 + iTime * SEA_SPEED;
                        // 坐标转换
                        vec2 uv = TexCoord * iResolution;
                        // 时间转换
                        float time = iTime * 0.1;
                        // fragColor
                        vec3 color = getPixel(uv, time);
                        fragColor = vec4(pow(color, vec3(0.65)), 1.0);
                    }
            );
    if (!initial) {
        initial = true;
        // gl_program
        m_program = gl_program_create(
                gl_shader_vertex_source,
                gl_shader_fragment_source
        );
        gl_check(__LINE__);
        startTime = glfwGetTime();
    }
    // 初始化时记录起始时间
    currentTime = glfwGetTime() - startTime;
    { // use gl_program
        glUseProgram(m_program);
        gl_check(__LINE__);
    }
    { // bind Shader_attribute aPosition & aTexCoord
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) loc_vertex);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) loc_tex);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        gl_check(__LINE__);
    }
    { // uniform
        GLint i_resolution_indx = glGetUniformLocation(m_program, "iResolution");
        glUniform2f(i_resolution_indx, (float) m_r_width, (float) m_r_height);
        GLint i_time_indx = glGetUniformLocation(m_program, "iTime");
        glUniform1f(i_time_indx, (float) currentTime);
        gl_check(__LINE__);
    }
    { // drawArray
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(GL_NONE);
        gl_check(__LINE__);
    }
}

GLuint gl3Sea::gl_program_create(const char *vertex, const char *fragment) {
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

GLuint gl3Sea::gl_shader_create(GLenum gl_type, const char *gl_source) {
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

void gl3Sea::gl_check(int line) {
    GLenum err = glGetError();
    if (!err) return;
    LOGE("gl_check error=%d, line=%d", glGetError(), line);
}

double gl3Sea::glfwGetTime() {
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return duration_cast<std::chrono::duration<double>>(duration).count();
}
