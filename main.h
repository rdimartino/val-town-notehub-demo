// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include <Arduino.h>
#include <Notecard.h>

// Constants
#define secs1Min (60)
#define secs1Hour (60 * secs1Min)
#define ms1Sec (1000)
#define ms1Min (1000 * secs1Min)
#define ms1Hour (1000 * secs1Hour)

// C trickery to convert a number to a string
#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

// .ino
extern Notecard notecard;

// version.cpp
const char *productVersion(void);
const char *firmwareVersion(void);
const char *productVersionShort();

// dfu.cpp
void dfuShowPartitions(void);
void dfuPoll(bool force);

