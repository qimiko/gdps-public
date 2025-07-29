package com.kyurime.geometryjump

import android.os.Bundle

class GeometryDashActivity : BaseGeometryDashActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        System.loadLibrary(GJConstants.FMOD_LIB_NAME)
        System.loadLibrary(GJConstants.COCOS_LIB_NAME)

        super.onCreate(savedInstanceState)

        System.loadLibrary(GJConstants.MOD_CORE_LIB_NAME)
    }
}