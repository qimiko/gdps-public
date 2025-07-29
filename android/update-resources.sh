#!/bin/bash

curl -LO https://github.com/qimiko/geode/releases/download/nightly/resources.zip

rm -rf ./app/src/main/assets/geode.loader
unzip resources.zip -d ./app/src/main/assets/geode.loader

rm resources.zip
