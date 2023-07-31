#include <jni.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <thread>
#include "src/common.h"
#include "src/gl2_plane.h"
#include "src/gl3_plane.h"
#include "src/gl3_box.h"
#include "src/gl3_light.h"
#include "src/gl3_sky_box.h"
#include "src/gl3_mode_obj.h"
#include "src/egl_context.h"
#include "src/gl3_sea.h"

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
        m_p_gl3_mode->gl_mode_obj(0, 0, mode_path_str, texture_path_str);
    } else if (type == 2) {
        m_p_gl3_mode->update_viewport(w, h);
    }
}

eglContext *m_egl = new eglContext();

void native_egl_draw(JNIEnv *env, jobject *,
                     jint type,
                     jint w, jint h,
                     jobject surface,
                     jstring save_path_name) {
    if (type == 1) { // onscreen
        ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
        std::thread t = std::thread([=](ANativeWindow *w) {
            m_egl->onscreenWindowCreate(2, w);
            for (int i = 0; i < 10; i++) {
                m_egl->onscreenDraw();
                m_egl->onscreenWindowSwap();
            }
        }, win);
        t.detach();
    } else if (type == 2) { // offscreen
        jboolean jCopy = false;
        auto *save_path_name_ch = env->GetStringUTFChars(save_path_name, &jCopy);
        auto save_path_name_str = std::string(save_path_name_ch);
        env->ReleaseStringUTFChars(save_path_name, save_path_name_ch);
        std::thread t = std::thread([=](std::string str) {
            m_egl->offscreenWindowCreate(2, w, h);
            m_egl->offscreenDraw(str);
        }, save_path_name_str);
        t.detach();
    }
}

gl3Sea *m_gl3Sea = new gl3Sea();

void native_gl3_sea_draw(JNIEnv *env, jobject *,
                         jint jtype,
                         jint jw,
                         jint jh) {
    if (jtype == 1) {
        m_gl3Sea->gl_sea();
    } else if (jtype == 2) {
        m_gl3Sea->update_viewport(jw, jh);
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
        {
                "native_egl_draw",
                "(IIILjava/lang/Object;Ljava/lang/String;)V",
                (void *) native_egl_draw
        },
        {
                "native_gl3_sea_draw",
                "(III)V",
                (void *) native_gl3_sea_draw
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