package com.kyurime.geometryjump

import android.annotation.SuppressLint
import android.content.Context
import java.io.File

object SaveUtils {
    val SAVE_FILES = arrayOf("CCGameManager.dat", "CCLocalLevels.dat")

    @SuppressLint("SdCardPath")
    private fun getOldSavePath(): String {
        return "/data/data/com.kyurime.geometryjump/"
    }

    /**
     * determines if save data should use the hardcoded /data/data/ path or
     * the path returned by context.filesDir
     * @return true if save data can be found in context.filesDir
     */
    private fun dataInModernLocation(): Boolean {
        val dataDir = getOldSavePath()

        return SAVE_FILES.none {
            // if any save files in the old location still exist, don't use the new one
            val saveFile = File(dataDir, it)
            saveFile.exists()
        }
    }

    private fun getGeodeSaveDir(context: Context): File {
        return File(context.filesDir, "save")
    }

    fun migrateSaveData(context: Context) {
        if (dataInModernLocation()) {
            return
        }

        val dataDir = getOldSavePath()
        val newDir = getGeodeSaveDir(context)

        SAVE_FILES.forEach {
            val saveFile = File(dataDir, it)
            val newFile = File(newDir, it)

            if (saveFile.exists()) {
                saveFile.copyTo(newFile)
                saveFile.delete()
            }
        }

        File(dataDir, "files").listFiles()?.forEach {
            if (it.extension == "mp3") {
                val newFile = File(newDir, it.name)

                it.copyTo(newFile)
                it.delete()
            }
        }
    }

    fun getSavePath(context: Context?) =
        if (context == null) {
            File(getOldSavePath())
        } else {
            getGeodeSaveDir(context)
        }

    /**
     * fetches the stored temporary level path, if one exists
     * @return null if no temporary level path is currently saved
     */
    fun getTemporaryLevelPath(context: Context): String? {
        context.apply {
            val sharedPref = getSharedPreferences(
                getString(R.string.preference_key), Context.MODE_PRIVATE)
            val importKey = getString(R.string.import_level_path)

            val existingPath = sharedPref.getString(importKey, null)
            if (!existingPath.isNullOrEmpty()) {
                return existingPath
            }
        }

        // just assume. idc
        return null
    }

    /**
     * fetches the stored temporary level path, or creates one if one doesn't exist
     */
    fun generateTemporaryLevel(context: Context?): String {
        context?.apply {
            val levelPath = getTemporaryLevelPath(context)
            if (!levelPath.isNullOrEmpty()) {
                return levelPath
            }

            val tempPath = File.createTempFile("exported.gmd", null, cacheDir).absolutePath

            val sharedPref = getSharedPreferences(
                getString(R.string.preference_key), Context.MODE_PRIVATE)

            with (sharedPref.edit()) {
                putString(getString(R.string.import_level_path), tempPath)
                apply()
            }

            return tempPath
        }

        // this probably shouldn't be reached ever
        return getSavePath(null).absolutePath + "/exported.gmd"
    }

    fun clearTemporaryLevel(context: Context) {
        val levelPath = getTemporaryLevelPath(context)
        if (levelPath.isNullOrEmpty()) {
            return
        }

        val levelFile = File(levelPath)
        if (levelFile.exists()) {
            levelFile.delete()
        }

        context.apply {
            val sharedPref = getSharedPreferences(
                getString(R.string.preference_key), Context.MODE_PRIVATE)

            with (sharedPref.edit()) {
                remove(getString(R.string.import_level_path))
                apply()
            }
        }
    }
}