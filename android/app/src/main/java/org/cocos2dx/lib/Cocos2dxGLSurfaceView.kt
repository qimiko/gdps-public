package org.cocos2dx.lib

import android.content.Context
import android.opengl.GLSurfaceView
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.view.InputDevice
import android.view.KeyEvent
import android.view.MotionEvent
import android.view.inputmethod.InputMethodManager
import androidx.annotation.RequiresApi
import com.customRobTop.BaseRobTopActivity
import com.kyurime.geometryjump.ModGlue
import kotlin.math.abs

private const val HANDLER_OPEN_IME_KEYBOARD = 2
private const val HANDLER_CLOSE_IME_KEYBOARD = 3

class Cocos2dxGLSurfaceView(context: Context) : GLSurfaceView(context) {
    companion object {
        lateinit var cocos2dxGLSurfaceView: Cocos2dxGLSurfaceView
        private lateinit var handler: Handler
        private lateinit var cocos2dxTextInputWrapper: Cocos2dxTextInputWrapper

        @JvmStatic
        fun openIMEKeyboard() {
            val msg = Message()
            msg.what = HANDLER_OPEN_IME_KEYBOARD
            msg.obj = cocos2dxGLSurfaceView.getContentText()
            handler.sendMessage(msg)
        }

        @JvmStatic
        fun closeIMEKeyboard() {
            val msg = Message()
            msg.what = HANDLER_CLOSE_IME_KEYBOARD
            handler.sendMessage(msg)
        }

        @RequiresApi(Build.VERSION_CODES.O)
        fun captureCursor() {
            cocos2dxGLSurfaceView.requestPointerCapture()
        }

        @RequiresApi(Build.VERSION_CODES.O)
        fun releaseCursor() {
            cocos2dxGLSurfaceView.queueEvent {
                cocos2dxGLSurfaceView.cocos2dxRenderer.handleKeyUp(96, 0)
            }

            cocos2dxGLSurfaceView.releasePointerCapture()
        }

        fun queueAccelerometer(x: Float, y: Float, z: Float, timestamp: Long) =
            cocos2dxGLSurfaceView.queueEvent {
                Cocos2dxAccelerometer.onSensorChanged(x, y, z, timestamp)
            }
    }

    private lateinit var cocos2dxRenderer: Cocos2dxRenderer

    var cocos2dxEditText: Cocos2dxEditText? = null
        set(value) {
            field = value

            field?.setOnEditorActionListener(cocos2dxTextInputWrapper)
            field?.cocos2dxGLSurfaceView = this
            requestFocus()
        }

    fun initView() {
        setEGLContextClientVersion(2)
        isFocusableInTouchMode = true
//        if (Build.VERSION.SDK_INT >= 11) {
        preserveEGLContextOnPause = true
//        }
        cocos2dxGLSurfaceView = this
        cocos2dxTextInputWrapper = Cocos2dxTextInputWrapper(this)
        Cocos2dxGLSurfaceView.handler = object : Handler(Looper.getMainLooper()) {
            override fun handleMessage(msg: Message) {
                when (msg.what) {
                    HANDLER_OPEN_IME_KEYBOARD -> {
                        if (cocos2dxEditText?.requestFocus() == true) {
                            cocos2dxEditText?.apply {
                                removeTextChangedListener(
                                    cocos2dxTextInputWrapper
                                )
                                setText("")
                                val text = msg.obj as String
                                append(text)
                                cocos2dxTextInputWrapper.setOriginText(text)
                                addTextChangedListener(
                                    cocos2dxTextInputWrapper
                                )
                            }
                            (context.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager).showSoftInput(
                                cocos2dxEditText,
                                0
                            )
                            Log.d("GLSurfaceView", "showSoftInput")
                            return
                        }
                        return
                    }
                    HANDLER_CLOSE_IME_KEYBOARD -> {
                        cocos2dxEditText?.apply {
                            removeTextChangedListener(
                                cocos2dxTextInputWrapper
                            )
                            (context.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager).hideSoftInputFromWindow(
                                windowToken,
                                0
                            )
                        }

                        requestFocus()
                        Log.d("GLSurfaceView", "HideSoftInput")
                        cocos2dxRenderer.handleTextClosed()
                        return
                    }
                    else -> return
                }
            }
        }
    }

    override fun onPause() {
        // clear load here, just in case
        ModGlue.loadedToMenu()

        queueEvent { cocos2dxRenderer.handleOnPause() }
        super.onPause()
    }

    override fun onResume() {
        super.onResume()
        queueEvent { cocos2dxRenderer.handleOnResume() }
    }

    override fun onCapturedPointerEvent(motionEvent: MotionEvent): Boolean {
        return when (motionEvent.action and MotionEvent.ACTION_MASK) {
            MotionEvent.ACTION_MOVE -> {
                val horizontalOffset: Float = motionEvent.x
                val verticalOffset: Float = motionEvent.y

                if (abs(horizontalOffset) <= 0.2f && abs(verticalOffset) <= 0.2f) {
                    return true
                }

                queueEvent {
                    cocos2dxRenderer.handleActionMouseMove(horizontalOffset, verticalOffset)
                }
                true
            }
            MotionEvent.ACTION_DOWN -> {
                // hardcode to the controller input down keys
                // (they behave similarly)
                queueEvent { cocos2dxRenderer.handleKeyDown(96, 0) }

                true
            }
            MotionEvent.ACTION_UP -> {
                queueEvent { cocos2dxRenderer.handleKeyUp(96, 0) }

                true
            }
            else -> true
        }
    }

    override fun onTouchEvent(motionEvent: MotionEvent): Boolean {
        val pointerNumber = motionEvent.pointerCount
        val ids = IntArray(pointerNumber)
        val xs = FloatArray(pointerNumber)
        val ys = FloatArray(pointerNumber)
        for (i in 0 until pointerNumber) {
            ids[i] = motionEvent.getPointerId(i)
            xs[i] = motionEvent.getX(i)
            ys[i] = motionEvent.getY(i)
        }
        return when (motionEvent.action and MotionEvent.ACTION_MASK) {
            MotionEvent.ACTION_DOWN -> {
                val idDown = motionEvent.getPointerId(0)
                val xDown = xs[0]
                val f = ys[0]
                queueEvent {
                    cocos2dxRenderer.handleActionDown(
                        idDown,
                        xDown,
                        f
                    )
                }
                true
            }
            MotionEvent.ACTION_UP -> {
                val idUp = motionEvent.getPointerId(0)
                val f2 = xs[0]
                val f3 = ys[0]
                queueEvent { cocos2dxRenderer.handleActionUp(idUp, f2, f3) }
                true
            }
            MotionEvent.ACTION_MOVE -> {
                queueEvent {
                    cocos2dxRenderer.handleActionMove(
                        ids,
                        xs,
                        ys
                    )
                }
                true
            }
            MotionEvent.ACTION_CANCEL -> {
                queueEvent {
                    cocos2dxRenderer.handleActionCancel(
                        ids,
                        xs,
                        ys
                    )
                }
                true
            }
            MotionEvent.ACTION_POINTER_DOWN -> {
                val indexPointerDown = motionEvent.action shr 8
                val idPointerDown = motionEvent.getPointerId(indexPointerDown)
                val xPointerDown = motionEvent.getX(indexPointerDown)
                val y = motionEvent.getY(indexPointerDown)
                queueEvent {
                    cocos2dxRenderer.handleActionDown(
                        idPointerDown,
                        xPointerDown,
                        y
                    )
                }
                true
            }
            MotionEvent.ACTION_POINTER_UP -> {
                val indexPointUp = motionEvent.action shr 8
                val idPointerUp = motionEvent.getPointerId(indexPointUp)
                val xPointerUp = motionEvent.getX(indexPointUp)
                val y2 = motionEvent.getY(indexPointUp)
                queueEvent {
                    cocos2dxRenderer.handleActionUp(
                        idPointerUp,
                        xPointerUp,
                        y2
                    )
                }
                true
            }
            else -> true
        }
    }

    // copied from the Android docs
    // ignores values inside "flat" range of joystick
    private fun getCenteredAxis(
        event: MotionEvent,
        device: InputDevice,
        axis: Int,
    ): Float {
        val range = device.getMotionRange(axis, event.source)

        range?.apply {
            val value = event.getAxisValue(axis)
            if (abs(value) > flat) {
                return value
            }
        }
        return 0f
    }

    override fun onGenericMotionEvent(event: MotionEvent?): Boolean {
        return when (event?.action?.and(MotionEvent.ACTION_MASK)) {
            MotionEvent.ACTION_SCROLL -> {
                val scrollX = event.getAxisValue(MotionEvent.AXIS_HSCROLL)
                val scrollY = event.getAxisValue(MotionEvent.AXIS_VSCROLL)

                queueEvent {
                    cocos2dxRenderer.handleActionScroll(scrollX, scrollY)
                }
                true
            }
            MotionEvent.ACTION_MOVE -> {
                val inputDevice = event.device

                val leftX = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_X)
                val leftY = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_Y)

                val rightX = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_Z)
                val rightY = getCenteredAxis(event, inputDevice, MotionEvent.AXIS_RZ)

                queueEvent {
                    cocos2dxRenderer.handleActionJoystickMove(leftX, leftY, rightX, rightY)
                }

                true
            }
            else -> super.onGenericMotionEvent(event)
        }
    }

    override fun onSizeChanged(
        newSurfaceWidth: Int,
        newSurfaceHeight: Int,
        oldSurfaceWidth: Int,
        oldSurfaceHeight: Int
    ) {
        if (!isInEditMode) {
            cocos2dxRenderer.setScreenWidthAndHeight(newSurfaceWidth, newSurfaceHeight)
        }
    }

    override fun onKeyDown(keyCode: Int, keyEvent: KeyEvent): Boolean {
        return when (keyCode) {
            KeyEvent.KEYCODE_VOLUME_DOWN, KeyEvent.KEYCODE_VOLUME_UP, KeyEvent.KEYCODE_MUTE -> {
                super.onKeyDown(keyCode, keyEvent)
            }
            else -> {
                if (keyEvent.repeatCount != 0 || BaseRobTopActivity.blockBackButton) {
                    return true
                }
                queueEvent { cocos2dxRenderer.handleKeyDown(keyCode, keyEvent.modifiers) }
                true

            }
        }
    }

    override fun onKeyUp(keyCode: Int, keyEvent: KeyEvent?): Boolean {
        return when (keyCode) {
            KeyEvent.KEYCODE_VOLUME_DOWN, KeyEvent.KEYCODE_VOLUME_UP, KeyEvent.KEYCODE_MUTE -> {
                super.onKeyDown(keyCode, keyEvent)
            }
            else -> {
                if (keyEvent?.repeatCount != 0 || BaseRobTopActivity.blockBackButton) {
                    return true
                }
                queueEvent { cocos2dxRenderer.handleKeyUp(keyCode, keyEvent.modifiers) }
                true
            }
        }
    }

    fun setCocos2dxRenderer(renderer: Cocos2dxRenderer) {
        this.cocos2dxRenderer = renderer
        setRenderer(this.cocos2dxRenderer)
    }

    private fun getContentText(): String {
        return cocos2dxRenderer.getContentText()
    }

    fun insertText(text: String) {
        queueEvent { cocos2dxRenderer.handleInsertText(text) }
    }

    fun deleteBackward() {
        queueEvent { cocos2dxRenderer.handleDeleteBackward() }
    }
}