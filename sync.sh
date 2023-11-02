#!/bin/sh

OSX_DIR="/Users/etienne/Dev/of_v0.9.0_osx_release/apps/myApps/harp2"

CURRENT_DIR=$(pwd)

SHARED="shared"

BIN="bin"
DATA="data"

cp -a "$OSX_DIR/$SHARED" "$CURRENT_DIR"
cp -a "$OSX_DIR/$BIN/$DATA" "$CURRENT_DIR/$BIN"