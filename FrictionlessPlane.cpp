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

#include "FrictionlessPlane.h"

namespace frictionlessplane {

/*******************************************************************************
 Haptic plane callback.  The plane is oriented along Y=0 and provides a 
 repelling force if the device attempts to penetrates through it.
*******************************************************************************/
HDCallbackCode HDCALLBACK FrictionlessPlaneCallback(void *data)
{
    // Stiffnes, i.e. k value, of the plane.  Higher stiffness results
    // in a harder surface.
    const double planeStiffness = .25;

    // Amount of force the user needs to apply in order to pop through
    // the plane.
    const double popthroughForceThreshold = 10.0;
    
    // Plane direction changes whenever the user applies sufficient
    // force to popthrough it.
    // 1 means the plane is facing +Y.
    // -1 means the plane is facing -Y.
    static int directionFlag = 1;

    //hdBeginFrame(hdGetCurrentDevice());

    // Get the position of the device.
    hduVector3Dd position;
    hdGetDoublev(HD_CURRENT_POSITION, position);

    // If the user has penetrated the plane, set the device force to 
    // repel the user in the direction of the surface normal of the plane.
    // Penetration occurs if the plane is facing in +Y and the user's Y position
    // is negative, or vice versa.
	double penetrationDistance = 0;
    double k = planeStiffness;
	hduVector3Dd f = hduVector3Dd(0, 0, 0);

	// update x force
	if (position[0] <= -75 || position[0] >= 75) {
		directionFlag = (position[0] > 0) ? -1 : 1;
        penetrationDistance = fabs(position[0]) - 75;
        hduVector3Dd forceDirection(directionFlag, 0, 0);

		// Hooke's law explicitly:
        hduVector3Dd x = penetrationDistance*forceDirection;
        f = k*x;

        // If the user applies sufficient force, pop through the plane
        // by reversing its direction.  Otherwise, apply the repel
        // force.
        if (f.magnitude() > popthroughForceThreshold)
        {
            f.set(0.0,0.0,0.0);
            directionFlag = -directionFlag;
        }
	}
	// update y force
	if (position[1] <= -20 || position[1] >= 20) {
		directionFlag = (position[1] > 0) ? -1 : 1;
        penetrationDistance = fabs(position[1]) - 20;
        hduVector3Dd forceDirection(0, directionFlag, 0);

		// Hooke's law explicitly:
        hduVector3Dd x = penetrationDistance*forceDirection;
        f += k*x;

        // If the user applies sufficient force, pop through the plane
        // by reversing its direction.  Otherwise, apply the repel
        // force.
        if (f.magnitude() > popthroughForceThreshold)
        {
            f.set(0.0,0.0,0.0);
            directionFlag = -directionFlag;
        }
	}
	// update z force
	if (position[2] <= -50 || position[2] >= 50) {
		directionFlag = (position[2] > 0) ? -1 : 1;
        penetrationDistance = fabs(position[2]) - 50;
        hduVector3Dd forceDirection(0, 0, directionFlag);

		// Hooke's law explicitly:
        hduVector3Dd x = penetrationDistance*forceDirection;
        f += k*x;

        // If the user applies sufficient force, pop through the plane
        // by reversing its direction.  Otherwise, apply the repel
        // force.
        if (f.magnitude() > popthroughForceThreshold)
        {
            f.set(0.0,0.0,0.0);
            directionFlag = -directionFlag;
        }
	}
		
    hdSetDoublev(HD_CURRENT_FORCE, f);

    hdEndFrame(hdGetCurrentDevice());

    // In case of error, terminate the callback.
    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Error detected during main scheduler callback\n");

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;  
        }
    }

    return HD_CALLBACK_CONTINUE;
}

}  // namespace frictionlessplane

