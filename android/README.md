# 1.9 GDPS Android

## Requirements

* android-ndk-r17c or below (must support gnustl)
* Geometry Dash **1.92** APK. 1.93 or above will _not_ work!
* Git

## Building

1. Grab the submodules if they haven't been grabbed already.

2. Setup a copy of the APK by providing it as the first argument in [setup-apk.sh](setup-apk.sh).  
   Example usage: `./setup-apk.sh GeometryDash192.apk`  

3. Open project in Android Studio and build from there.

### App Signing

Gradle will sign during the build process. A file called `signing.properties` must be created in the `app/` directory for Gradle to configure signing. The format of the file is as follows:

```
storeFile=$storeFile
storePassword=$storePassword
keyAlias=$keyAlias
keyPassword=$keyPassword
```

The development build type was created to allow building debuggable builds with a release key.

### Launcher Mode

The experimental launcher mode (accessible through the launcher build flavor) enables building the mod without a copy of Geometry Dash. During runtime, it will pull necessary files from an installed full version of the 1.9 GDPS. It is also possible to pull these files from an installed version of Geometry Dash 1.92, but this currently requires modifying the launcher source.

If building in launcher mode, step 2 can be skipped. See the [build-launcher](.github/workflows/build-launcher.yml) workflow for an example.

_Launcher mode is not recommended for general use_.

## Development

This project follows a split publicity model. The public repository will be updated upon each new release of the 1.9 GDPS. Only the `main` branch from the public repository will be synced. Commits made to the public repository will be synced to the private repository when they are made. Merging will be dealt with personally.

## Documentation

Explanations for some parts of the code can be found in the libaurav2's [docs folder](app/src/main/cpp/libaurav2/docs/).

## Licensing

This repository is licensed under the [Mozilla Public License v2.0](LICENSE).

## Asset Credits

* RobTop  
  Many of the `-uhd` assets in the `app/src/main/assets` folder come from the PC version of Geometry Dash 1.9.

* Aroo  
  Designed a majority of the textures that can be found in the `assets` folder.

* EPSSounds  
  Created the menu loop. Can be found at its [Newgrounds page](https://www.newgrounds.com/audio/listen/559315).

_Main level song credits and creators can be found ingame._