package com.geode.launcher.utils

import android.content.ActivityNotFoundException
import android.widget.Toast
import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.content.Intent
import android.os.Build
import android.provider.DocumentsContract
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import com.kyurime.geometryjump.R
import com.kyurime.geometryjump.UserDirectoryProvider
import java.io.File
import java.lang.ref.WeakReference
import kotlin.system.exitProcess

object GeodeUtils {
    private lateinit var activity: WeakReference<AppCompatActivity>
    var handleSafeArea: Boolean = false

    private var openFileResultLauncher: ActivityResultLauncher<String>? = null

    fun setContext(activity: AppCompatActivity) {
        this.activity = WeakReference(activity)

        openFileResultLauncher = activity.registerForActivityResult(ActivityResultContracts.GetContent()) { uri ->
            if (uri != null) {
                activity.contentResolver.openFileDescriptor(uri, "r")?.use { fd ->
                    val fileSize = fd.statSize

                    if (fileSize != -1L) {
                        selectFileDescriptorCallback(fd.fd, fileSize)

                        return@registerForActivityResult
                    }
                }
            }

            Toast.makeText(activity, R.string.no_file_selected, Toast.LENGTH_SHORT)
                .show()

            failedCallback()
        }
    }

    private external fun selectFileDescriptorCallback(fd: Int, size: Long)
    private external fun failedCallback()

    private fun showUnimplementedDialog() {
        this.activity.get()?.apply {
            runOnUiThread {
                Toast.makeText(
                    this,
                    this.getText(R.string.file_unimplemented),
                    Toast.LENGTH_SHORT
                ).show()
            }
        }
    }

    @JvmStatic
    fun writeClipboard(text: String) {
        activity.get()?.run {
            val manager = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
            val clip = ClipData.newPlainText("1.9 GDPS", text)
            manager.setPrimaryClip(clip)
        }
    }

    @JvmStatic
    fun readClipboard(): String {
        activity.get()?.run {
            val manager = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
            val clip = manager.primaryClip
            if (clip != null && clip.itemCount > 0) {
                return clip.getItemAt(0).coerceToText(this).toString()
            }
        }
        return ""
    }

    @JvmStatic
    fun openFolder(path: String): Boolean {
        val context = activity.get()!!

        val intent = Intent(Intent.ACTION_VIEW).apply {
            data = DocumentsContract.buildRootUri(
                "${context.packageName}.user", UserDirectoryProvider.ROOT
            )

            addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION or
                    Intent.FLAG_GRANT_WRITE_URI_PERMISSION or
                    Intent.FLAG_GRANT_PREFIX_URI_PERMISSION or
                    Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION
            )
        }

        return try {
            context.startActivity(intent)
            true
        } catch (_: ActivityNotFoundException) {
            println("anef")
            false
        }
    }

    @JvmStatic
    fun restartGame() {
        activity.get()?.run {
            packageManager.getLaunchIntentForPackage(packageName)?.also {
                val mainIntent = Intent.makeRestartActivityTask(it.component)
                startActivity(mainIntent)
                exitProcess(0)
            }
        }
    }

    @JvmStatic
    fun selectFileBytes(path: String): Boolean {
        return try {
            openFileResultLauncher?.launch("*/*") ?: return false
            true
        } catch (_: ActivityNotFoundException) {
            false
        }
    }

    @JvmStatic
    fun selectFile(path: String): Boolean {
        this.showUnimplementedDialog()
        return false
    }

    @JvmStatic
    fun selectFiles(path: String): Boolean {
        this.showUnimplementedDialog()
        return false
    }

    @JvmStatic
    fun selectFolder(path: String): Boolean {
        this.showUnimplementedDialog()
        return false
    }

    @JvmStatic
    fun createFile(path: String): Boolean {
        this.showUnimplementedDialog()
        return false
    }

    @JvmStatic
    fun getBaseDirectory(): String {
        val activity = activity.get()!!
        return activity.filesDir.canonicalPath
    }

    @JvmStatic
    fun getInternalDirectory(): String {
        val activity = activity.get()!!
        return File(activity.filesDir, "internal").canonicalPath
    }

    @JvmStatic
    fun getGameVersion(): String {
        return "1.920"
    }

    @JvmStatic
    fun getLaunchArguments(): String? {
        return null
    }

    private const val CAPABILITY_EXTENDED_INPUT = "extended_input"

    @JvmStatic
    fun reportPlatformCapability(capability: String?): Boolean {
        if (capability.isNullOrEmpty()) {
            return false
        }

        if (capability == CAPABILITY_EXTENDED_INPUT) {
            return true
        }

        return false
    }

    @JvmStatic
    fun loadInternalBinary(binaryName: String) {
        System.loadLibrary(binaryName)
    }

    @JvmStatic
    fun getScreenInsets(): IntArray? {
        if (!handleSafeArea || Build.VERSION.SDK_INT < Build.VERSION_CODES.P) {
            return null
        }

        return activity.get()?.run {
            val displayCutout = window.decorView.rootWindowInsets?.displayCutout ?: return null

            intArrayOf(displayCutout.safeInsetLeft, displayCutout.safeInsetBottom, displayCutout.safeInsetRight, displayCutout.safeInsetTop)
        }
    }
}