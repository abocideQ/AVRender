#include <jni.h>
#include "src/common.h"
#include "src/gl2_plane.h"

auto *m_p_gl2_plane = new gl2Plane();

void native_gl2_rgba_draw(JNIEnv *env, jobject *, jint type, jint w, jint h, jbyteArray data) {
    if (type == 0 || type == 1) {
        jboolean jCopy = false;
        auto * buffer = (uint8_t *) env->GetByteArrayElements(data,  &jCopy);
        env->DeleteLocalRef(data);
        m_p_gl2_plane->gl_rgba_draw_array(type == 0, w, h,buffer);
    } else if (type == 2) {
        m_p_gl2_plane->update_viewport(w, h);
    }
}

#define JNI_LENGTH(n) (sizeof(n)/sizeof((n)[0]))
const char *JNI_CLASS = {
        "com/av/render/AVRender"
};
JNINativeMethod JNI_METHODS[] = {
        {
                "native_gl2_rgba_draw",
                "(III[B)V",
                (void *) native_gl2_rgba_draw
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