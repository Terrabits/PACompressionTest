#!/usr/bin/env bash

source __FILE__

ROOT="$($__FILE__)"
BUILD="$ROOT/build"
CORE="$ROOT/Core"
TEST="$ROOT/Test/CoreTest"
APP="$ROOT/Application"

set -e
promake "$CORE/Core.pro"       "$BUILD/debug/Core"          all
promake "$TEST/CoreTest.pro"   "$BUILD/debug/Test/CoreTest" debug
promake "$APP/Application.pro" "$BUILD/debug/Application"   debug
