/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  Vibration.c

Description: 

  This example shows how to generate a simple sinusoidal vibration effect
  for the haptic device.  The amplitude and frequency of the vibration effect
  can be adjusted.

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <stdio.h>
#include <assert.h>

#if defined(WIN32)
# include <windows.h>
# include <conio.h>
#else
# include "conio.h"
# include <ctype.h>
typedef int BOOL;
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

#ifndef USERDATA_H
#include "UserData.h"
#define USERDATA_H
#endif

namespace vibration {

static HDint gVibrationFreq = 100; /* Hz */
static HDdouble gVibrationAmplitude = 0.50; /* N */

static HDSchedulerHandle gCallbackHandle;

void mainLoop();
BOOL initDemo();

HDCallbackCode HDCALLBACK SetVibrationFreqCallback(void *data);
HDCallbackCode HDCALLBACK SetVibrationAmplitudeCallback(void *data);
HDCallbackCode HDCALLBACK VibrationCallback(void *data);

}  // namespace vibration