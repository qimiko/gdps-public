package com.kyurime.geometryjump

import android.content.ActivityNotFoundException
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.hardware.input.InputManager
import android.os.Build
import android.os.FileUtils
import android.view.WindowManager
import android.widget.Toast
import androidx.core.view.InputDeviceCompat
import com.customRobTop.BaseRobTopActivity
import org.cocos2dx.lib.Cocos2dxGLSurfaceView
import java.io.File
import java.io.FileOutputStream
import java.io.InputStream
import java.io.OutputStream

object ModGlue {
    const val REQUEST_CODE_LOAD_TEXTURE = 2
    const val REQUEST_CODE_EXPORT_LEVEL = 3
    const val REQUEST_CODE_IMPORT_LEVEL = 4

    private const val DEFAULT_TEXTURE_DIRECTORY = "/data/data/${BuildConfig.APPLICATION_ID}/textures/"

    @JvmStatic
    external fun onTextureDirectoryChosen()

    @JvmStatic
    external fun onLevelImported(path: String)

    @JvmStatic
    external fun showLoadingCircle()

    @JvmStatic
    external fun removeLoadingCircle()

    @JvmStatic
    fun isControllerConnected(): Boolean {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
            return false
        }

        BaseRobTopActivity.me.get()?.apply {
            val inputManager = getSystemService(Context.INPUT_SERVICE) as InputManager

            return inputManager.inputDeviceIds.any {
                val inputSource = inputManager.getInputDevice(it)?.sources ?: 0
                inputSource and InputDeviceCompat.SOURCE_GAMEPAD == InputDeviceCompat.SOURCE_GAMEPAD
            }
        }

        return false
    }

    @JvmStatic
    fun isGeometryDashInstalled(): Boolean {
        return try {
            BaseRobTopActivity.me.get()?.packageManager?.getPackageInfo(GJConstants.PACKAGE_NAME, 0) ?: return false
            true
        } catch (e: PackageManager.NameNotFoundException) {
            false
        }
    }

    @JvmStatic
    fun getTexturesDirectory(): String {
        val baseDir = BaseRobTopActivity.me.get()?.getExternalFilesDir(null)?.path
            ?: return DEFAULT_TEXTURE_DIRECTORY

        return "$baseDir/textures/"
    }

    @JvmStatic
    fun showTexturePicker() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            Toast.makeText(
                BaseRobTopActivity.me.get(),
                "This feature is not supported on your device.",
                Toast.LENGTH_SHORT
            ).show()

            return
        }

        val exportIntent = Intent(Intent.ACTION_OPEN_DOCUMENT_TREE)
        BaseRobTopActivity.me.get()?.startActivityForResult(exportIntent, REQUEST_CODE_LOAD_TEXTURE)
    }

    fun clearTexturesDirectory() {
        val textureFile = File(getTexturesDirectory())
        if (textureFile.exists()) {
            textureFile.deleteRecursively()
        }
    }

    @JvmStatic
    fun wipeTexturesDirectory() {
        clearTexturesDirectory()

        beginLoad()
        onTextureDirectoryChosen()
    }

    @JvmStatic
    fun getLevelExportPath(): String {
        val context = BaseRobTopActivity.me.get()
        return SaveUtils.generateTemporaryLevel(context)
    }

    @JvmStatic
    fun onExportLevel(levelName: String) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
            Toast.makeText(
                BaseRobTopActivity.me.get(),
                "This feature is not supported on your device.",
                Toast.LENGTH_SHORT
            ).show()

            return
        }

        val intent = Intent(Intent.ACTION_CREATE_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = "application/prs.gmd+xml"
            putExtra(Intent.EXTRA_TITLE, "$levelName.gmd")
        }
        BaseRobTopActivity.me.get()?.startActivityForResult(intent, REQUEST_CODE_EXPORT_LEVEL)
    }

    @JvmStatic
    fun showLevelPicker() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
            Toast.makeText(
                BaseRobTopActivity.me.get(),
                "This feature is not supported on your device.",
                Toast.LENGTH_SHORT
            ).show()

            return
        }

        try {
            val intent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
                addCategory(Intent.CATEGORY_OPENABLE)
                type = "*/*"
            }

            BaseRobTopActivity.me.get()?.startActivityForResult(intent, REQUEST_CODE_IMPORT_LEVEL)
        } catch (e: ActivityNotFoundException) {
            Toast.makeText(
                BaseRobTopActivity.me.get(),
                "No activities found to handle this request.",
                Toast.LENGTH_SHORT).show()
        }
    }

    @JvmStatic
    fun keepScreenAwake() {
        BaseRobTopActivity.me.get()?.apply {
            runOnUiThread {
                window?.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
            }
        }
    }

    @JvmStatic
    fun removeScreenAwake() {
        BaseRobTopActivity.me.get()?.apply {
            runOnUiThread {
                window?.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
            }
        }
    }

    @JvmStatic
    fun captureCursor() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            Cocos2dxGLSurfaceView.captureCursor()
        }
    }

    @JvmStatic
    fun releaseCursor() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            Cocos2dxGLSurfaceView.releaseCursor()
        }
    }

    fun beginLoad() {
        val activity = BaseRobTopActivity.me.get() ?: return

        val sharedPref = activity.getSharedPreferences(
            activity.getString(R.string.preference_key), Context.MODE_PRIVATE)

        with (sharedPref.edit()) {
            putBoolean(activity.getString(R.string.is_loading_key), true)
            apply()
        }
    }

    fun diedDuringLoad(): Boolean {
        val activity = BaseRobTopActivity.me.get() ?: return false

        val sharedPref = activity.getSharedPreferences(
            activity.getString(R.string.preference_key), Context.MODE_PRIVATE)

        return sharedPref.getBoolean(activity.getString(R.string.is_loading_key), false)
    }

    @JvmStatic
    fun loadedToMenu() {
        val activity = BaseRobTopActivity.me.get() ?: return

        val sharedPref = activity.getSharedPreferences(
            activity.getString(R.string.preference_key), Context.MODE_PRIVATE)

        with (sharedPref.edit()) {
            putBoolean(activity.getString(R.string.is_loading_key), false)
            apply()
        }
    }

    @JvmStatic
    fun isScreenRestricted(): Boolean {
        val activity = BaseRobTopActivity.me.get() ?: return false

        val sharedPref = activity.getSharedPreferences(
            activity.getString(R.string.preference_key), Context.MODE_PRIVATE)

        return sharedPref.getBoolean(activity.getString(R.string.is_layout_restricted_key), false)
    }

    @JvmStatic
    fun toggleIsScreenRestricted() {
        val activity = BaseRobTopActivity.me.get() ?: return

        val sharedPref = activity.getSharedPreferences(
            activity.getString(R.string.preference_key), Context.MODE_PRIVATE)

        val currentOption = isScreenRestricted()

        with (sharedPref.edit()) {
            putBoolean(activity.getString(R.string.is_layout_restricted_key), !currentOption)
            apply()
        }
    }

    @JvmStatic
    fun applyClassicPack() {
        // cut down version of the main loading code
        val textureDirectory = getTexturesDirectory()

        val textureFile = File(textureDirectory)
        if (textureFile.exists()) {
            textureFile.deleteRecursively()
        }
        textureFile.mkdir()

        BaseRobTopActivity.me.get()?.assets?.apply {
            list("classic")?.forEach {
                val inputStream = open("classic/$it")
                val outputStream = FileOutputStream(File(textureFile, it))

                copyFile(inputStream, outputStream)
            }
        }

        beginLoad()
        onTextureDirectoryChosen()
    }

    @JvmStatic
    fun getSecondaryAssetsDirectory(): String? {
        if (BuildConfig.IS_LAUNCHER) {
            return BaseRobTopActivity.me.get()?.applicationInfo?.sourceDir
        }

        return null
    }

    @JvmStatic
    fun getSaveDirectory(): String {
        val context = BaseRobTopActivity.me.get()
        // undo the removal of the forward slash that file.path does
        return "${SaveUtils.getSavePath(context).path}/"
    }

    @JvmStatic
    fun isLauncherBuild(): Boolean {
        return BuildConfig.IS_LAUNCHER
    }

    fun copyFile(inputStream: InputStream, outputStream: OutputStream) {
        // gotta love copying
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            FileUtils.copy(inputStream, outputStream)
        } else {
            inputStream.use { input ->
                outputStream.use { output ->
                    val buffer = ByteArray(4 * 1024)
                    while (true) {
                        val byteCount = input.read(buffer)
                        if (byteCount < 0) break
                        output.write(buffer, 0, byteCount)
                    }
                    output.flush()
                }
            }
        }
    }
}