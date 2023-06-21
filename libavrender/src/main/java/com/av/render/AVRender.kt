package com.av.render

class AVRender {

    companion object {
        init {
            System.loadLibrary("avrender")
        }
    }

    external fun native_gl2_rgba_draw(type: Int, w: Int, h: Int, bytes: ByteArray?)
}