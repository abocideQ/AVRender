package com.example.render

import android.annotation.SuppressLint
import android.graphics.BitmapFactory
import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import com.av.render.AVRender
import java.nio.ByteBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MainActivity : AppCompatActivity() {

    private val mByteArray = arrayListOf<ByteArray>()

    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        run {
            val container: LinearLayout = findViewById(R.id.ll_button)
            // gl2draw
            var button = Button(baseContext)
            button.text = "gl2DrawPlane"
            button.setOnClickListener { gl2DrawPlane(1, findViewById(R.id.fl_root)) }
            container.addView(button)
            // gl2draw_vbo_ebo_fbo
            button = Button(baseContext)
            button.text = "gl2DrawPlane_vbo_ebo_fbo"
            button.setOnClickListener { gl2DrawPlane(2, findViewById(R.id.fl_root)) }
            container.addView(button)
            // gl3draw_vao
            button = Button(baseContext)
            button.text = "gl3DrawPlane_vao"
            button.setOnClickListener { gl3DrawPlane(findViewById(R.id.fl_root)) }
            container.addView(button)
            // gl3draw_box
            button = Button(baseContext)
            button.text = "gl3DrawBox"
            button.setOnClickListener { gl3DrawBox(findViewById(R.id.fl_root)) }
            container.addView(button)
            // gl3draw_light
            button = Button(baseContext)
            button.text = "gl3DrawLight"
            button.setOnClickListener { gl3DrawLight(findViewById(R.id.fl_root)) }
            container.addView(button)
            // gl3draw_sky_box
            button = Button(baseContext)
            button.text = "gl3DrawSkyBox"
            button.setOnClickListener { gl3DrawSkyBox(findViewById(R.id.fl_root)) }
            container.addView(button)
        }
    }

    private fun gl2DrawPlane(drawType: Int, container: ViewGroup) {
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.room)
        val length: Int = bitmap.byteCount
        val data: ByteBuffer = ByteBuffer.allocate(length)
        bitmap.copyPixelsToBuffer(data)
        mByteArray.clear()
        mByteArray.add(data.array())
        val glRender = AVRender()
        val glSurfaceView = GLSurfaceView(container.context)
        glSurfaceView.setEGLContextClientVersion(2)
        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            }

            override fun onDrawFrame(gl: GL10?) {
                glRender.native_gl2_rgba_draw(
                    drawType,
                    1,
                    bitmap.width,
                    bitmap.height,
                    mByteArray[0]
                )
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                glRender.native_gl2_rgba_draw(drawType, 2, width, height, null)
            }
        })
        container.removeAllViews()
        container.addView(glSurfaceView)
    }

    private fun gl3DrawPlane(container: ViewGroup) {
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.room)
        val length: Int = bitmap.byteCount
        val data: ByteBuffer = ByteBuffer.allocate(length)
        bitmap.copyPixelsToBuffer(data)
        mByteArray.clear()
        mByteArray.add(data.array())
        val glRender = AVRender()
        val glSurfaceView = GLSurfaceView(container.context)
        glSurfaceView.setEGLContextClientVersion(3)
        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            }

            override fun onDrawFrame(gl: GL10?) {
                glRender.native_gl3_rgba_draw(1, bitmap.width, bitmap.height, mByteArray[0])
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                glRender.native_gl3_rgba_draw(2, width, height, null)
            }
        })
        container.removeAllViews()
        container.addView(glSurfaceView)
    }

    private fun gl3DrawBox(container: ViewGroup) {
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.desk)
        val length: Int = bitmap.byteCount
        val data: ByteBuffer = ByteBuffer.allocate(length)
        bitmap.copyPixelsToBuffer(data)
        mByteArray.clear()
        mByteArray.add(data.array())
        val glRender = AVRender()
        val glSurfaceView = GLSurfaceView(container.context)
        glSurfaceView.setEGLContextClientVersion(3)
        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            }

            override fun onDrawFrame(gl: GL10?) {
                glRender.native_gl3_box_draw(1, bitmap.width, bitmap.height, mByteArray[0])
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                glRender.native_gl3_box_draw(2, width, height, null)
            }
        })
        container.removeAllViews()
        container.addView(glSurfaceView)
    }

    private fun gl3DrawLight(container: ViewGroup) {
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.desk)
        val length: Int = bitmap.byteCount
        val data: ByteBuffer = ByteBuffer.allocate(length)
        bitmap.copyPixelsToBuffer(data)
        mByteArray.clear()
        mByteArray.add(data.array())
        val glRender = AVRender()
        val glSurfaceView = GLSurfaceView(container.context)
        glSurfaceView.setEGLContextClientVersion(3)
        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            }

            override fun onDrawFrame(gl: GL10?) {
                glRender.native_gl3_light_draw(1, bitmap.width, bitmap.height, mByteArray[0])
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                glRender.native_gl3_light_draw(2, width, height, null)
            }
        })
        container.removeAllViews()
        container.addView(glSurfaceView)
    }

    private fun gl3DrawSkyBox(container: ViewGroup) {
        val bitmaps = arrayOf(
            R.drawable.desk,
            R.drawable.sky_bottom,
        )
        var width = 0
        var height = 0
        var sky_w = 0
        var sky_h = 0
        mByteArray.clear()
        for (i in bitmaps.indices) {
            val bitmap = BitmapFactory.decodeResource(resources, bitmaps[i])
            if (i == 0) {
                width = bitmap.width
                height = bitmap.height
            } else {
                sky_w = bitmap.width
                sky_h = bitmap.width
            }
            val data: ByteBuffer = ByteBuffer.allocate(bitmap.byteCount)
            bitmap.copyPixelsToBuffer(data)
            mByteArray.add(data.array())
            bitmap.recycle()
            System.gc()
            data.clear()
        }
        val glRender = AVRender()
        val glSurfaceView = GLSurfaceView(container.context)
        glSurfaceView.setEGLContextClientVersion(3)
        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            }

            override fun onDrawFrame(gl: GL10?) {
                glRender.native_gl3_sky_box_draw(
                    1, width, height, mByteArray[0],
                    sky_w, sky_h,
                    mByteArray[1],
                    mByteArray[1],
                    mByteArray[1],
                    mByteArray[1],
                    mByteArray[1],
                    mByteArray[1]
                )
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                glRender.native_gl3_sky_box_draw(
                    2, width, height, null,
                    0, 0,
                    null,
                    null,
                    null,
                    null,
                    null,
                    null
                )
            }
        })
        container.removeAllViews()
        container.addView(glSurfaceView)
    }
}