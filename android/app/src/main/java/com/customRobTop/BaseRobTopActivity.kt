package com.customRobTop

import android.annotation.SuppressLint
import android.app.Activity
import android.app.KeyguardManager
import android.content.*
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.net.Uri
import android.os.Build
import android.provider.Settings
import android.util.Log
import android.widget.Toast
import com.customRobTop.JniToCpp.resumeSound
import com.kyurime.geometryjump.GJConstants
import org.cocos2dx.lib.Cocos2dxGLSurfaceView.Companion.closeIMEKeyboard
import org.cocos2dx.lib.Cocos2dxGLSurfaceView.Companion.openIMEKeyboard
import java.lang.ref.WeakReference
import java.util.*


@Suppress("unused", "UNUSED_PARAMETER")
object BaseRobTopActivity {
    var isLoaded = false
    var blockBackButton = false
    private var keyboardActive = false

    var isPaused = false

    lateinit var me: WeakReference<Activity>
    private var shouldResumeSound = true

    fun setCurrentActivity(currentActivity: Activity) {
        me = WeakReference(currentActivity)
    }

    @SuppressLint("HardwareIds")
    @JvmStatic
    fun getUserID(): String {
        // this is how RobTop does it in 2.2, based on the meltdown leaks
        val androidId = Settings.Secure.getString(me.get()?.contentResolver, Settings.Secure.ANDROID_ID)
        return if ("9774d56d682e549c" != androidId) {
            UUID.nameUUIDFromBytes(androidId.toByteArray()).toString()
        } else return UUID.randomUUID().toString()
    }

    @JvmStatic
    fun isNetworkAvailable(): Boolean {
        val connectivityManager = me.get()?.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val activeNetwork = connectivityManager.activeNetwork
            val networkCapabilities =
                connectivityManager.getNetworkCapabilities(activeNetwork) ?: return false

            if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)) {
                return true
            }

            if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR)) {
                return true
            }

            if (networkCapabilities.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET)) {
                return true
            }
        } else {
            // old version sdk support
            val activeNetwork = connectivityManager.activeNetworkInfo
            if (activeNetwork?.isConnected == true && activeNetwork.isAvailable == true) {
                return true
            }
        }

        return false
    }

    @JvmStatic
    fun openURL(url: String) {
        Log.d("MAIN", "Open URL")
        me.get()?.runOnUiThread {
            try {
                me.get()?.startActivity(
                    Intent(
                        "android.intent.action.VIEW",
                        Uri.parse(url)
                    )
                )
            } catch (e: ActivityNotFoundException) {
                Toast.makeText(
                    me.get(),
                    "No activity found to open this URL.",
                    Toast.LENGTH_SHORT,
                ).show()
            }
        }
    }

    @JvmStatic
    fun sendMail(subject: String, body: String, to: String) {
        me.get()?.runOnUiThread {
            val i = Intent("android.intent.action.SEND")
            i.type = "message/rfc822"
            i.putExtra("android.intent.extra.EMAIL", arrayOf(to))
            i.putExtra("android.intent.extra.SUBJECT", subject)
            i.putExtra("android.intent.extra.TEXT", body)
            try {
                me.get()?.startActivity(Intent.createChooser(i, "Send mail..."))
            } catch (e: ActivityNotFoundException) {
                Toast.makeText(
                    me.get(),
                    "There are no email clients installed.",
                    Toast.LENGTH_SHORT,
                ).show()
            }
        }
    }

    @JvmStatic
    fun shouldResumeSound(): Boolean {
        return shouldResumeSound
    }

    @JvmStatic
    fun setKeyboardState(value: Boolean) {
        keyboardActive = value
    }

    @JvmStatic
    fun onToggleKeyboard() {
        me.get()?.runOnUiThread {
            if (keyboardActive) {
                openIMEKeyboard()
            } else {
                closeIMEKeyboard()
            }
        }
    }

    @JvmStatic
    fun loadingFinished() {
        isLoaded = true
    }

    // Everyplay doesn't even exist anymore lol
    @JvmStatic
    fun setupEveryplay() {}

    @JvmStatic
    fun isEveryplaySupported(): Boolean {
        return false
    }

    @JvmStatic
    fun isRecordingSupported(): Boolean {
        return false
    }

    @JvmStatic
    fun isRecordingPaused() : Boolean {
        return false
    }

    @JvmStatic
    fun isRecording() : Boolean {
        return false
    }

    @JvmStatic
    fun startRecording() {}

    @JvmStatic
    fun stopRecording() {}

    @JvmStatic
    fun resumeRecording() {}

    @JvmStatic
    fun pauseRecording() {}

    @JvmStatic
    fun playLastRecording() {}

    @JvmStatic
    fun showEveryplay() {}

    @JvmStatic
    fun setEveryplayMetadata(levelID: String, levelName: String) {}

    @JvmStatic
    fun onNativePause() {}

    @JvmStatic
    fun onNativeResume() {}

    // Google Play Games methods
    // for some reason the button is hidden in game
    @JvmStatic
    fun gameServicesIsSignedIn(): Boolean {
        return false
    }

    @JvmStatic
    fun gameServicesSignIn() { }

    @JvmStatic
    fun gameServicesSignOut() { }

    @JvmStatic
    fun unlockAchievement(id: String) {}

    @JvmStatic
    fun showAchievements() {}

    // advertisements stuff, useless for full version
    @JvmStatic
    fun enableBanner() {}

    @JvmStatic
    fun disableBanner() {}

    @JvmStatic
    fun showInterstitial() {}

    @JvmStatic
    fun cacheInterstitial() {}

    @JvmStatic
    fun hasCachedInterstitial(): Boolean {
        return false
    }

    @JvmStatic
    fun showRewardedVideo() {}

    @JvmStatic
    fun cacheRewardedVideo() {}

    @JvmStatic
    fun hasCachedRewardedVideo(): Boolean {
        return false
    }

    @JvmStatic
    fun queueRefreshBanner() {}

    @JvmStatic
    fun enableBannerNoRefresh() {}

    // for in game rate buttons, may implement in the future depending on how lazy I am
    // (requires Play Store library)
    @JvmStatic
    fun tryShowRateDialog(appName: String) {}

    @JvmStatic
    fun openAppPage() {
        me.get()?.runOnUiThread {
            JniToCpp.userDidAttemptToRateApp()
            try {
                me.get()?.startActivity(
                    Intent(
                        "android.intent.action.VIEW",
                        Uri.parse("market://details?id=${GJConstants.PACKAGE_NAME}")
                    )
                )
            } catch (e: ActivityNotFoundException) {
                Toast.makeText(
                    me.get(),
                    "No activity found to open this URL.",
                    Toast.LENGTH_SHORT,
                ).show()
            }
        }
    }

    @JvmStatic
    @Deprecated(
        message = "This method is not found on newer versions of the game.",
        level = DeprecationLevel.HIDDEN
    )
    fun logEvent(event: String) {}

    class ReceiverScreen : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when (intent.action) {
                Intent.ACTION_SCREEN_ON -> {
                    Log.d("TAG", "ACTION_SCREEN_ON")

                    val keyguardManager = me.get()?.getSystemService(Context.KEYGUARD_SERVICE) as KeyguardManager

                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                        if (!keyguardManager.isKeyguardLocked) {
                            shouldResumeSound = true
                        }
                    } else {
                        if (!keyguardManager.inKeyguardRestrictedInputMode()) {
                            shouldResumeSound = true
                        }
                    }

                    if (!isPaused && shouldResumeSound) {
                        resumeSound()
                    }
                }
                Intent.ACTION_SCREEN_OFF -> {
                    shouldResumeSound = false
                }
                Intent.ACTION_USER_PRESENT -> {
                    shouldResumeSound = true
                    if (!isPaused) {
                        resumeSound()
                    }
                }
            }
        }
    }
}