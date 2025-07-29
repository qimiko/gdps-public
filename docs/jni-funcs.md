# JNI Functions

The following lists extend the required functions from Geometry Dash 1.9.

Some JNI functions are required for the game to run, and will crash if not found.

## Static Methods

These methods should be provided in the Android code.
Their usages can be found in ["base/platform/android.cpp"](/src/base/platform/android.cpp).

`Lcom/kyurime/geometryjump/ModGlue;->isGeometryDashInstalled()Z`  
Returns true if package "com.robtopx.geometryjump" is installed.

`Lcom/kyurime/geometryjump/ModGlue;->keepScreenAwake()V`  
Adds the keep screen on flag for the application window.

`Lcom/kyurime/geometryjump/ModGlue;->removeScreenAwake()V`  
Clears the keep screen on flag for the application window.

`Lcom/kyurime/geometryjump/ModGlue;->showTexturePicker()V`  
Shows a texture file picker.

`Lcom/kyurime/geometryjump/ModGlue;->applyClassicPack()V`  
Applies the classic texture pack and reloads game.

`Lcom/kyurime/geometryjump/ModGlue;->wipeTexturesDirectory()V`  
Deletes all files in the custom textures directory and reloads game.

`Lcom/kyurime/geometryjump/ModGlue;->onExportLevel(Ljava/lang/String;)V`  
Single parameter is the filename of the level file. Shows a file picker to export a level outside of the game.

`Lcom/kyurime/geometryjump/ModGlue;->showLevelPicker()V`  
Shows a file picker to import a level inside the game.

`Lcom/kyurime/geometryjump/ModGlue;->loadedToMenu()V`  
Called after `MenuLayer::init` executes successfully. Indicates that loading did not fail.

`Lcom/kyurime/geometryjump/ModGlue;->getTexturesDirectory()Ljava/lang/String;`  
Returns a String containing the path to the custom textures directory. This will return a valid path, even if no texture pack is applied.

`Lcom/kyurime/geometryjump/ModGlue;->isControllerConnected()Z`  
Returns true if a controller is currently connected.

`Lcom/kyurime/geometryjump/ModGlue;->getSaveDirectory()Ljava/lang/String;`  
Returns the path to the save data directory. This is typically `/data/data/$APPLICATION_ID/`.

`Lcom/kyurime/geometryjump/ModGlue;->getLevelExportPath()Ljava/lang/String;`  
Returns the path to a temporary file to copy level data to when exporting.

`Lcom/kyurime/geometryjump/ModGlue;->getSecondaryAssetsDirectory()Ljava/lang/String;`  
If launcher mode is enabled, returns the path to a secondary assets directory that overrides the main assets. If launcher mode is not enabled, null is returned.

`Lcom/kyurime/geometryjump/ModGlue;->isLauncherBuild()Z`  
Returns true if launcher mode is enabled. This enables extra asset redirections in the mod code.

`Lcom/kyurime/geometryjump/ModGlue;->isScreenRestricted()Z`  
Returns true if aspect ratio restriction is enabled in the app preferences.

`Lcom/kyurime/geometryjump/ModGlue;->toggleIsScreenRestricted()V`  
Toggles the aspect ratio restriction option in the app preferences.

## Native Methods

These methods are defined by the mod code for use by the Android layer. Their definitions can be found in ["classes/callbacks/"](/src/classes/callbacks/).

`bool Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeKeyDown2(int keyCode, int modifiers);`  
Used for keyboard support. Indicates that a key (keyCode) is pressed.

`bool Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeKeyUp(int keyCode, int modifiers);`  
Used for keyboard support. Indicates that a key (keyCode) is released.

`bool Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeActionJoystickMove(float leftX, float leftY, float rightX, float rightY);`  
Used for joystick support. All values should be adjusted for deadzones prior to this function being called.

`void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeSurfaceChanged(int width, int height);`  
Used for screen resizing. Should not be called during surface initialization.

`void Java_com_kyurime_geometryjump_ModGlue_onLevelImported(jstring path);`  
Used for level file support. path is the path to the level file. Imports an xml level file (gmd) and changes scene to its info page.

`void Java_com_kyurime_geometryjump_ModGlue_onTextureDirectoryChosen();`  
Used for texture pack support. Reloads game textures.

`void Java_com_kyurime_geometryjump_ModGlue_showLoadingCircle();`  
Used for texture pack support. Shows a loading circle. This loading circle disables all interactions and cannot be removed by the user.

`void Java_com_kyurime_geometryjump_ModGlue_removeLoadingCircle();`  
Used for texture pack support. Removes a loading circle, allowing the user to control the game.