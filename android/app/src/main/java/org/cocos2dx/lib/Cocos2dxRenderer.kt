package org.cocos2dx.lib

import android.opengl.GLSurfaceView
import android.os.Build
import android.view.Surface
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

@Suppress("unused", "KotlinJniMissingFunction")
class Cocos2dxRenderer(var glSurfaceView: Cocos2dxGLSurfaceView) : GLSurfaceView.Renderer {
    companion object {
        @JvmStatic
        fun setAnimationInterval(@Suppress("UNUSED_PARAMETER") animationInterval: Double) {
            // this function is useless for gd
        }

        @JvmStatic
        private external fun nativeDeleteBackward()

        @JvmStatic
        private external fun nativeGetContentText(): String

        @JvmStatic
        private external fun nativeInsertText(text: String)

        @JvmStatic
        private external fun nativeKeyDown(keyCode: Int): Boolean

        @JvmStatic
        private external fun nativeOnPause()

        @JvmStatic
        private external fun nativeOnResume()

        @JvmStatic
        private external fun nativeRender()

        @JvmStatic
        private external fun nativeTextClosed()

        @JvmStatic
        private external fun nativeTouchesBegin(id: Int, x: Float, y: Float)

        @JvmStatic
        private external fun nativeTouchesCancel(
            ids: IntArray,
            xs: FloatArray,
            ys: FloatArray
        )

        @JvmStatic
        private external fun nativeTouchesEnd(id: Int, x: Float, y: Float)

        @JvmStatic
        private external fun nativeTouchesMove(ids: IntArray, xs: FloatArray, ys: FloatArray)

        @JvmStatic
        private external fun nativeInit(width: Int, height: Int)

        // custom additions
        @JvmStatic
        private external fun nativeKeyUp(keyCode: Int, modifiers: Int): Boolean

        @JvmStatic
        private external fun nativeKeyDown2(keyCode: Int, modifiers: Int): Boolean

        @JvmStatic
        private external fun nativeActionScroll(scrollX: Float, scrollY: Float): Boolean

        @JvmStatic
        private external fun nativeActionJoystickMove(leftX: Float, leftY: Float, rightX: Float, rightY: Float): Boolean

        @JvmStatic
        private external fun nativeActionMouseMove(x: Float, y: Float): Boolean

        @JvmStatic
        private external fun nativeSurfaceChanged(width: Int, height: Int)
    }

    private var lastTickInNanoSeconds: Long = 0
    private var screenWidth = 0
    private var screenHeight = 0
    private var initFinished = false

    fun setScreenWidthAndHeight(surfaceWidth: Int, surfaceHeight: Int) {
        screenWidth = surfaceWidth
        screenHeight = surfaceHeight
    }

    override fun onSurfaceCreated(gl10: GL10?, eglConfig: EGLConfig?) {
        nativeInit(screenWidth, screenHeight)
        lastTickInNanoSeconds = System.nanoTime()
        initFinished = true
    }

    override fun onSurfaceChanged(gl10: GL10?, width: Int, height: Int) {
        if (initFinished) {
            nativeSurfaceChanged(width, height)
        }
    }

    override fun onDrawFrame(gl: GL10?) {
        nativeRender()
    }

    fun handleActionDown(id: Int, x: Float, y: Float) {
        nativeTouchesBegin(id, x, y)
    }

    fun handleActionUp(id: Int, x: Float, y: Float) {
        nativeTouchesEnd(id, x, y)
    }

    fun handleActionCancel(ids: IntArray, xs: FloatArray, ys: FloatArray) {
        nativeTouchesCancel(ids, xs, ys)
    }

    fun handleActionMove(ids: IntArray, xs: FloatArray, ys: FloatArray) {
        nativeTouchesMove(ids, xs, ys)
    }

    fun handleKeyDown(keyCode: Int, modifiers: Int) {
        nativeKeyDown2(keyCode, modifiers)
    }

    fun handleOnPause() {
        nativeOnPause()
    }

    fun handleOnResume() {
        nativeOnResume()
    }

    fun handleInsertText(text: String) {
        nativeInsertText(text)
    }

    fun handleDeleteBackward() {
        nativeDeleteBackward()
    }

    fun handleTextClosed() {
        nativeTextClosed()
    }

    fun getContentText(): String {
        return nativeGetContentText()
    }

    // custom additions
    fun handleKeyUp(keyCode: Int, modifiers: Int) {
        nativeKeyUp(keyCode, modifiers)
    }

    fun handleActionScroll(scrollX: Float, scrollY: Float) {
        nativeActionScroll(scrollX, scrollY)
    }

    fun handleActionJoystickMove(leftX: Float, leftY: Float, rightX: Float, rightY: Float) {
        nativeActionJoystickMove(leftX, leftY, rightX, rightY)
    }

    fun handleActionMouseMove(x: Float, y: Float) {
        nativeActionMouseMove(x, y)
    }
}