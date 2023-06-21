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
            val bt2gl2DrawPlane = Button(baseContext)
            bt2gl2DrawPlane.text = "gl2DrawPlane"
            bt2gl2DrawPlane.setOnClickListener { gl2DrawPlane(findViewById(R.id.fl_root)) }
            container.addView(bt2gl2DrawPlane)
        }
    }

    private fun gl2DrawPlane(container: ViewGroup) {
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
                glRender.native_gl2_rgba_draw(0, bitmap.width, bitmap.height, mByteArray[0])
            }

            override fun onDrawFrame(gl: GL10?) {
                glRender.native_gl2_rgba_draw(1, bitmap.width, bitmap.height, mByteArray[0])
            }

            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
                glRender.native_gl2_rgba_draw(2, width, height, null)
            }
        })
        container.removeAllViews()
        container.addView(glSurfaceView)
    }
}