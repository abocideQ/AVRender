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

    external fun native_gl3_sky_box_draw(
        type: Int, w: Int, h: Int, bytes: ByteArray?,
        sky_w: Int, sky_h: Int,
        sky_right: ByteArray?,
        sky_left: ByteArray?,
        sky_top: ByteArray?,
        sky_bottom: ByteArray?,
        sky_front: ByteArray?,
        sky_back: ByteArray?
    )
}