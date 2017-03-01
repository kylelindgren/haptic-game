/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  FrictionlessPlane.cpp

Description: 

  This example demonstrates how to haptically render contact with an infinite
  frictionless plane.  The plane allows popthrough such that if the user 
  applies enough force against the plane, the plane will reverse its sidedness
  and allow the user to interact with it from the opposite side.

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <cstdio>
#include <cassert>
#include <iostream>

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include "UserData.h"

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

namespace frictionlessplane {

HDCallbackCode HDCALLBACK FrictionlessPlaneCallback(void *data);

}  // namespace frictionlessplane

