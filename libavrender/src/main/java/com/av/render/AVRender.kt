package com.av.render

class AVRender {

    companion object {
        init {
            System.loadLibrary("avrender")
        }
    }

    external fun native_gl2_rgba_draw(drawType: Int, type: Int, w: Int, h: Int, bytes: ByteArray?)

    external fun native_gl3_rgba_draw(type: Int, w: Int, h: Int, bytes: ByteArray?)

    external fun native_gl3_box_draw(type: Int, w: Int, h: Int, bytes: ByteArray?)

    external fun native_gl3_light_draw(type: Int, w: Int, h: Int, bytes: ByteArray?)

    external fun native_gl3_sky_box_draw(type: Int, w: Int, h: Int, bytes: ByteArray?)

    external fun native_gl3_mode_obj_draw(type: Int, w: Int, h: Int, mode: String?, tex: String?)
}