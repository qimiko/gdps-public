package com.kyurime.geometryjump

import android.annotation.SuppressLint
import android.content.Intent
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.content.res.AssetManager
import android.os.Bundle

const val ORIGINAL_PACKAGE_NAME = "com.kyurime.geometryjump"

class GeometryDashActivity : BaseGeometryDashActivity() {
    @SuppressLint("UnsafeDynamicallyLoadedCode")
    override fun onCreate(savedInstanceState: Bundle?) {
        if (!isPackageInstalled(ORIGINAL_PACKAGE_NAME)) {
            val launchIntent = Intent(this, LoadErrorActivity::class.java)
            launchIntent.flags = Intent.FLAG_ACTIVITY_CLEAR_TASK or Intent.FLAG_ACTIVITY_NEW_TASK

            startActivity(launchIntent)
        }

        val gdPackageInfo = packageManager.getPackageInfo(ORIGINAL_PACKAGE_NAME, 0)

        val gdNativeLibraryPath = "${gdPackageInfo.applicationInfo.nativeLibraryDir}/"

        try {
            addAssetsFromPackage(assets, gdPackageInfo)
        } catch (e: NoSuchMethodException) {
            e.printStackTrace()
        }

        System.load("$gdNativeLibraryPath/lib${GJConstants.FMOD_LIB_NAME}.so")
        System.load("$gdNativeLibraryPath/lib${GJConstants.COCOS_LIB_NAME}.so")
        System.loadLibrary(GJConstants.MOD_CORE_LIB_NAME)

        mSourceDirOverride = gdPackageInfo.applicationInfo.sourceDir

        super.onCreate(savedInstanceState)
    }

    fun isPackageInstalled(name: String): Boolean {
        return try {
            packageManager.getPackageInfo(name, 0) ?: return false
            true
        } catch (e: PackageManager.NameNotFoundException) {
            false
        }
    }

    @SuppressLint("DiscouragedPrivateApi")
    fun addAssetsFromPackage(assetManager: AssetManager, packageInfo: PackageInfo) {
        // this method is officially marked as deprecated but it is the only method we are allowed to reflect
        // (the source recommends replacing with AssetManager.setApkAssets(ApkAssets[], boolean) lol)
        val clazz = assetManager.javaClass
        val aspMethod = clazz.getDeclaredMethod("addAssetPath", String::class.java)
        aspMethod.invoke(assetManager, packageInfo.applicationInfo.sourceDir)
    }
}