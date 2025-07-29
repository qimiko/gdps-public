# Crash Reporter

### ⚠️ The crash reporter and its supporting code was removed in u8.1.5.

The crash reporter internally uses Google's [Breakpad library](https://chromium.googlesource.com/breakpad/breakpad/) to dump crashes. As such, reading them requires an external tool. More details can be found at Breakpad's [getting started page](https://chromium.googlesource.com/breakpad/breakpad/+/HEAD/docs/getting_started_with_breakpad.md). Breakpad is enabled upon loading the mod, and is not disabled until the mod is unloaded.

Note that having Breakpad enabled will disable Android's default stacktraces. This may be unhelpful during development. As such, turning off `GDMOD_USE_CRASH_DUMPS` in CMake will disable the Breakpad initialization.

## Upload Endpoint

The upload endpoint is configured using the macro `GDMOD_CRASH_UPLOAD_URL` and is blank by default. The endpoint used in release is `https://api.xyze.dev/gd/crash?hash=%s-%s`.

The first parameter in the URL is the branch of the current build. The second is the Git hash of the current build. These are configured by CMake at build time. The endpoint sends a crash report in the body of the request as plain data.

## Response

The response is in the JSON format with the following values defined:

| Name    | Type    | Description |
| ------- | ------- | - |
| message | string  | Message to show in finish popup. |
| success | boolean | Whether or not the upload was a success. This determines the finish popup's title. Successful uploads will also delete the saved crash report. |

## Configuration

The crash reporter is configured by two compile time variables. See [build-config.md](build-config.md) for more details. Ingame crash uploading will be disabled if `GDMOD_CRASH_UPLOAD_URL` is not defined or is set to a blank string.