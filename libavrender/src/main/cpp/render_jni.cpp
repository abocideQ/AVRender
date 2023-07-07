#include <jni.h>
#include "src/common.h"
#include "src/gl2_plane.h"
#include "src/gl3_plane.h"
#include "src/gl3_box.h"
#include "src/gl3_light.h"
#include "src/gl3_sky_box.h"
#include "src/gl3_mode_obj.h"

auto *m_p_gl2_plane = new gl2Plane();

void native_gl2_rgba_draw(JNIEnv *env, jobject *, jint draw_type, jint type, jint w, jint h,
                          jbyteArray data) {
    if (draw_type == 1) {
        if (type == 1) {
            jboolean jCopy = false;
            auto *buffer = (uint8_t *) env->GetByteArrayElements(data, &jCopy);
            m_p_gl2_plane->gl_rgba_draw_array(w, h, buffer);
        } else if (type == 2) {
            m_p_gl2_plane->update_viewport(w, h);
        }
    } else if (draw_type == 2) {
        if (type == 1) {
            jboolean jCopy = false;
            auto *buffer = (uint8_t *) env->GetByteArrayElements(data, &jCopy);
            m_p_gl2_plane->gl_rgba_draw_elements_vbo_fbo(w, h, buffer);
        } else if (type == 2) {
            m_p_gl2_plane->update_viewport(w, h);
        }
    }
}

auto *m_p_gl3_plane = new gl3Plane();

void native_gl3_rgba_draw(JNIEnv *env, jobject *, jint type, jint w, jint h,
                          jbyteArray data) {
    if (type == 1) {
        jboolean jCopy = false;
        auto *buffer = (uint8_t *) env->GetByteArrayElements(data, &jCopy);
        m_p_gl3_plane->gl_rgba_draw_vao_fbo_pbo_camera(w, h, buffer);
    } else if (type == 2) {
        m_p_gl3_plane->update_viewport(w, h);
    }
}

auto *m_p_gl3_box = new gl3Box();

void native_gl3_box_draw(JNIEnv *env, jobject *, jint type, jint w, jint h,
                         jbyteArray data) {
    if (type == 1) {
        jboolean jCopy = false;
        auto *buffer = (uint8_t *) env->GetByteArrayElements(data, &jCopy);
        m_p_gl3_box->gl_box_draw_vao_fbo_camera(w, h, buffer);
    } else if (type == 2) {
        m_p_gl3_box->update_viewport(w, h);
    }
}

auto *m_p_gl3_light = new gl3Light();

void native_gl3_light_draw(JNIEnv *env, jobject *, jint type, jint w, jint h,
                           jbyteArray data) {
    if (type == 1) {
        jboolean jCopy = false;
        auto *buffer = (uint8_t *) env->GetByteArrayElements(data, &jCopy);
        m_p_gl3_light->gl_light_draw_vao_fbo_camera(w, h, buffer);
    } else if (type == 2) {
        m_p_gl3_light->update_viewport(w, h);
    }
}

auto *m_p_gl3_sky = new gl3SkyBox();

void native_gl3_sky_box_draw(JNIEnv *env, jobject *, jint type, jint w, jint h, jbyteArray data) {
    if (type == 1) {
        jboolean jCopy = false;
        m_p_gl3_sky->gl_sky_box(w, h, (uint8_t *) env->GetByteArrayElements(data, &jCopy));
    } else if (type == 2) {
        m_p_gl3_sky->update_viewport(w, h);
    }
}

auto *m_p_gl3_mode = new gl3ModeObj();

void native_gl3_mode_obj_draw(JNIEnv *env, jobject *,
                              jint type,
                              jint w, jint h,
                              jstring mode_path,
                              jstring texture_path) {
    if (type == 1) {
        jboolean jCopy = false;
        auto *mode_path_ch = env->GetStringUTFChars(mode_path, &jCopy);
        auto *texture_path_ch = env->GetStringUTFChars(texture_path, &jCopy);
        auto mode_path_str = std::string(mode_path_ch);
        auto texture_path_str = std::string(texture_path_ch);
        env->ReleaseStringUTFChars(mode_path, mode_path_ch);
        env->ReleaseStringUTFChars(texture_path, texture_path_ch);
        m_p_gl3_mode->gl_mode_obj(w, h, mode_path_str, texture_path_str);
    } else if (type == 2) {
        m_p_gl3_mode->update_viewport(w, h);
    }
}

#define JNI_LENGTH(n) (sizeof(n)/sizeof((n)[0]))
const char *JNI_CLASS = {
        "com/av/render/AVRender"
};
JNINativeMethod JNI_METHODS[] = {
        {
                "native_gl2_rgba_draw",
                "(IIII[B)V",
                (void *) native_gl2_rgba_draw
        },
        {
                "native_gl3_rgba_draw",
                "(III[B)V",
                (void *) native_gl3_rgba_draw
        },
        {
                "native_gl3_box_draw",
                "(III[B)V",
                (void *) native_gl3_box_draw
        },
        {
                "native_gl3_light_draw",
                "(III[B)V",
                (void *) native_gl3_light_draw
        },
        {
                "native_gl3_sky_box_draw",
                "(III[B)V",
                (void *) native_gl3_sky_box_draw
        },
        {
                "native_gl3_mode_obj_draw",
                "(IIILjava/lang/String;Ljava/lang/String;)V",
                (void *) native_gl3_mode_obj_draw
        },
};

jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass jClazz = env->FindClass(JNI_CLASS);
    if (env->RegisterNatives(jClazz, JNI_METHODS, JNI_LENGTH(JNI_METHODS)) != JNI_OK) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}