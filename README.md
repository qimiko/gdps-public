# libaurav2

Core mod code for the 1.9 GDPS Android source.

## Requirements

* A compiled version of the [gdps fork](https://github.com/qimiko/geode/tree/gdps) of Geode SDK.
* Git

## Building

1. Build the gdps fork of the Geode SDK if you haven't already. You should set the environment variable `GEODE_SDK` to point to this fork, and add the Geode CLI to your path.

2. Setup CMake to point to the Android toolchain and the [CMakeLists.txt](CMakeLists.txt) in this directory.

3. Build like any other Geode mod.

## Development

Consider the public repository to be source-available. This repository will be updated upon each new release of the 1.9 GDPS.

## Documentation

Explanations for some parts of the code can be found in the [docs folder](docs/).

## Licensing

This repository is licensed under the [Mozilla Public License v2.0](LICENSE).
