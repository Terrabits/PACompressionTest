#!/usr/bin/env bash

source __FILE__

ROOT="$($__FILE__)"

-e
promake "$ROOT/Core/Core.pro"               "$ROOT/build"   all
promake "$ROOT/Test/CoreTest/CoreTest.pro"  "$ROOT/build" debug
promake "$ROOT/Application/Application.pro" "$ROOT/build" debug
