package com.kyurime.geometryjump

import android.app.Activity
import android.content.*
import android.content.pm.ActivityInfo
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.OpenableColumns
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import androidx.documentfile.provider.DocumentFile
import androidx.lifecycle.coroutineScope
import com.customRobTop.BaseRobTopActivity
import com.customRobTop.JniToCpp
import com.geode.launcher.utils.GeodeUtils
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import org.cocos2dx.lib.Cocos2dxEditText
import org.cocos2dx.lib.Cocos2dxGLSurfaceView
import org.cocos2dx.lib.Cocos2dxHelper
import org.cocos2dx.lib.Cocos2dxRenderer
import org.fmod.FMOD
import java.io.*


open class BaseGeometryDashActivity : AppCompatActivity(), Cocos2dxHelper.Cocos2dxHelperListener {
    private var mGLSurfaceView: Cocos2dxGLSurfaceView? = null
    private val sTag = GeometryDashActivity::class.simpleName
    private var mIsRunning = false
    private var mIsOnPause = false
    private var mHasWindowFocus = false
    private var mReceiver: BroadcastReceiver? = null

    protected var mSourceDirOverride: String? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        setupUIState()

        SaveUtils.migrateSaveData(this)
        FMOD.init(this)
        Cocos2dxHelper.init(this, this)

        super.onCreate(savedInstanceState)

        BaseRobTopActivity.setCurrentActivity(this)

        setContentView(createView())

        val sourceDir = mSourceDirOverride ?: applicationInfo.sourceDir

        JniToCpp.setupHSSAssets(
            sourceDir,
            Environment.getExternalStorageDirectory().absolutePath
        )
        Cocos2dxHelper.nativeSetApkPath(sourceDir)

        registerReceiver()

        runTextureIntegrityCheck()

        GeodeUtils.setContext(this)
    }

    protected fun runTextureIntegrityCheck() {
        if (ModGlue.diedDuringLoad()) {
            val loadFilesSize = File(ModGlue.getTexturesDirectory()).listFiles()?.size ?: 0
            if (loadFilesSize > 0) {
                ModGlue.clearTexturesDirectory()
                Toast.makeText(
                    this,
                    "Textures reset due to a crash during load.",
                    Toast.LENGTH_LONG
                ).show()
            }
        }

        if (!BaseRobTopActivity.isLoaded) {
            // prevent accidentally overwriting values
            ModGlue.beginLoad()
        }
    }

    private fun createView(): FrameLayout {
        val frameLayoutParams = ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.MATCH_PARENT,
        )
        val frameLayout = ConstrainedFrameLayout(this)
        frameLayout.layoutParams = frameLayoutParams

        if (ModGlue.isScreenRestricted()) {
            frameLayout.aspectRatio = 1.86f
        }

        val editTextLayoutParams = ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.WRAP_CONTENT
        )
        val editText = Cocos2dxEditText(this)
        editText.layoutParams = editTextLayoutParams
        frameLayout.addView(editText)

        val glSurfaceView = Cocos2dxGLSurfaceView(this)

        this.mGLSurfaceView = glSurfaceView
        frameLayout.addView(this.mGLSurfaceView)

        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8)

        if (isAndroidEmulator()) {
            glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0)
        }

        glSurfaceView.initView()
        glSurfaceView.setCocos2dxRenderer(Cocos2dxRenderer(glSurfaceView))

        editText.inputType = 145
        glSurfaceView.cocos2dxEditText = editText

        return frameLayout
    }

    private fun setupUIState() {
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_USER_LANDSCAPE

        hideSystemUi()
    }

    private fun hideSystemUi() {
        WindowCompat.setDecorFitsSystemWindows(window, false)

        WindowCompat.getInsetsController(window, window.decorView).apply {
            hide(WindowInsetsCompat.Type.systemBars())
            systemBarsBehavior =
                WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        unregisterReceivers()
        FMOD.close()
    }

    private fun resumeGame() {
        mIsRunning = true
//        Cocos2dxHelper.onResume()
        mGLSurfaceView?.onResume()
    }

    private fun pauseGame() {
        mIsRunning = false
//        Cocos2dxHelper.onPause()
        mGLSurfaceView?.onPause()
    }

    override fun onResume() {
        super.onResume()
        mIsOnPause = false
        BaseRobTopActivity.isPaused = false
        if (mHasWindowFocus && !this.mIsRunning) {
            resumeGame()
        }
    }

    override fun onPause() {
        super.onPause()
        mIsOnPause = true
        BaseRobTopActivity.isPaused = true
        if (mIsRunning) {
            pauseGame()
        }
    }

    private fun registerReceiver() {
        unregisterReceivers()
        try {
            val filter = IntentFilter(Intent.ACTION_SCREEN_ON)
            filter.addAction(Intent.ACTION_SCREEN_OFF)
            filter.addAction(Intent.ACTION_USER_PRESENT)
            mReceiver = BaseRobTopActivity.ReceiverScreen()
            registerReceiver(mReceiver, filter)
        } catch (e: Exception) {
        }
    }

    private fun unregisterReceivers() {
        if (mReceiver != null) {
            unregisterReceiver(mReceiver)
            mReceiver = null
        }
    }

    override fun onWindowFocusChanged(hasWindowFocus: Boolean) {
        super.onWindowFocusChanged(hasWindowFocus)
        mHasWindowFocus = hasWindowFocus
        if (hasWindowFocus && !mIsOnPause) {
            resumeGame()
        }

        hideSystemUi()
    }

    override fun runOnGLThread(runnable: Runnable) {
        mGLSurfaceView?.queueEvent(runnable)
    }

    override fun showDialog(title: String, message: String) {
        runOnUiThread {
            AlertDialog.Builder(this)
                .setTitle(title)
                .setMessage(message)
                // the button shouldn't do anything but close
                .setPositiveButton(R.string.message_box_accept) { _, _ -> }
                .show()
        }
    }

    override fun showEditTextDialog(
        title: String,
        message: String,
        inputMode: Int,
        inputFlag: Int,
        returnType: Int,
        maxLength: Int
    ) {
        TODO("Not yet implemented")
    }


    private fun isAndroidEmulator(): Boolean {
        Log.d(sTag, "model=" + Build.MODEL)
        val product = Build.PRODUCT
        Log.d(sTag, "product=$product")
        var isEmulator = false
        if (product != null) {
            isEmulator = product == "sdk" || product.contains("_sdk") || product.contains("sdk_")
        }
        Log.d(sTag, "isEmulator=$isEmulator")
        return isEmulator
    }

    @Deprecated("Deprecated in Java")
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        if (resultCode != Activity.RESULT_OK) {
            return
        }

        when (requestCode) {
            ModGlue.REQUEST_CODE_LOAD_TEXTURE -> {
                data?.data?.also { uri ->
                    // preserve value of this for coroutine
                    val context = this
                    lifecycle.coroutineScope.launch(Dispatchers.IO) {
                        // make sure a 2.0 texture pack isn't being imported
                        val directory = DocumentFile.fromTreeUri(context, uri)
                        val hasNewTextures = directory?.listFiles()?.any {
                            it.name?.startsWith("GJ_GameSheet03") ?: false
                        }

                        if (hasNewTextures == true) {
                            runOnUiThread {
                                Toast.makeText(
                                    context,
                                    "This texture pack is incompatible with 1.9!",
                                    Toast.LENGTH_LONG
                                ).show()
                            }

                            runOnGLThread {
                                // can't forget this too
                                ModGlue.removeLoadingCircle()
                            }
                            return@launch
                        }

                        // copy file to get around strange jni folder perms (they're strange)
                        val textureDirectory = ModGlue.getTexturesDirectory()
                        val textureFile = File(textureDirectory)
                        if (textureFile.exists()) {
                            textureFile.deleteRecursively()
                        }
                        textureFile.mkdir()

                        // these are not capitalized in the data. they are lowercase for comparison
                        val filenameBlacklist = listOf("leveldata.plist", "achievementsdesc.plist", "mods.plist")
                        directory?.listFiles()?.forEach {
                            // skip directory access for now (it's useless anyways)
                            if (it.isFile) {
                                // ignore files that impact game progress
                                if (filenameBlacklist.contains(it.name?.lowercase())) {
                                    return@forEach
                                }

                                val inputStream = contentResolver.openInputStream(it.uri)
                                val outputStream = FileOutputStream(File(textureFile, it.name))

                                if (inputStream != null) {
                                    ModGlue.copyFile(inputStream, outputStream)
                                }
                            }
                        }

                        runOnGLThread {
                            ModGlue.removeLoadingCircle()
                            ModGlue.beginLoad()
                            ModGlue.onTextureDirectoryChosen()
                        }
                    }

                    runOnGLThread {
                        ModGlue.showLoadingCircle()
                    }
                }
            }
            ModGlue.REQUEST_CODE_EXPORT_LEVEL -> {
                data?.data?.also { uri ->
                    val levelPath = SaveUtils.getTemporaryLevelPath(this)
                    if (levelPath.isNullOrEmpty()) {
                        Toast.makeText(this, "Error: Data not saved across reload? Try again.", Toast.LENGTH_LONG).show()
                        return
                    }

                    val levelFile = File(levelPath)
                    if (!levelFile.exists()) {
                        Toast.makeText(this, "Error: Level not correctly exported. Try again.", Toast.LENGTH_LONG).show()
                        SaveUtils.clearTemporaryLevel(this)
                        return
                    }

                    val inputStream = FileInputStream(levelFile)
                    val outputStream = contentResolver.openOutputStream(uri)

                    if (outputStream != null) {
                        ModGlue.copyFile(inputStream, outputStream)

                        Toast.makeText(this, "Level successfully exported", Toast.LENGTH_SHORT).show()
                    }

                    SaveUtils.clearTemporaryLevel(this)
                }
            }
            ModGlue.REQUEST_CODE_IMPORT_LEVEL -> {
                data?.data?.also { uri ->
                    val levelFile = File.createTempFile("imported.gmd", null, cacheDir)

                    val filename = getFileName(this, uri)
                    if (filename?.endsWith(".gmd") != true) {
                        runOnUiThread {
                            Toast.makeText(
                                this,
                                "This isn't a .gmd file...",
                                Toast.LENGTH_LONG
                            ).show()
                        }
                        return
                    }

                    val inputStream = contentResolver.openInputStream(uri)
                    val outputStream = FileOutputStream(levelFile)

                    if (inputStream != null) {
                        ModGlue.copyFile(inputStream, outputStream)

                        runOnGLThread {
                            ModGlue.onLevelImported(levelFile.absolutePath)
                        }
                    }
                }
            }
        }
    }

    private fun getFileName(context: Context, uri: Uri): String? {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
            // the implicit cast is version gated for some reason
            // old devices won't send SCHEME_CONTENT anyways
            return uri.lastPathSegment
        }

        return when (uri.scheme) {
            ContentResolver.SCHEME_FILE -> uri.lastPathSegment
            ContentResolver.SCHEME_CONTENT -> {
                return context.contentResolver.query(uri, null, null, null, null)?.use {
                    val nameIndex = it.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                    it.moveToFirst()
                    return it.getString(nameIndex)
                }
            }
            else -> null
        }
    }
}