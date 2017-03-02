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
#include <windows.h>
#include <iostream>
#include <mmsystem.h>
#include "FrictionlessPlane.h"


namespace frictionlessplane {

/*******************************************************************************
 Haptic plane callback.  The plane is oriented along Y=0 and provides a 
 repelling force if the device attempts to penetrates through it.
*******************************************************************************/
HDCallbackCode HDCALLBACK FrictionlessPlaneCallback(void *data)
{
	User* player = static_cast<User*>(data);

	static double room_length_min = -50, room_length_max = 50,
				room_width_min = -75, room_width_max = 75,
				room_height_min = -30, room_height_max = 30;
	static double door_width = 25, door_height = 45;
	static double key_size = 15;

    // Stiffnes, i.e. k value, of the plane.  Higher stiffness results
    // in a harder surface.
    const double planeStiffness = .25;
	hduVector3Dd f = hduVector3Dd(0, 0, 0);

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

	// check if entering a door
	switch (player->room) {
	case A:
		// door center front of the room with the height starting at room height min
		if (fabs(position[0]) <= door_width/2 &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			position[2] >= room_length_max) {
				
				player->room = B;
				std::cout << "Entering room: " << player->room << std::endl;
				//PlaySound(TEXT("door_edited.wav"), NULL, SND_FILENAME);
				// TODO: implement room change sequence by applying Hook's Law. 
				// When a room is changed, a force will be applied that pushes the player to (0,0,0).
				// new_position = old_position - k*old_position. 
				//k less than 1.
				//while(position[0] != 0 && position[1] != 0 && position[2] != 0){
			    //f+= - 0.25*position;}
			
		} else if (position[0] >= room_width_max &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			fabs(position[2]) <= door_width/2) {
				player->room = D;
				std::cout << "Entering room: " << player->room << std::endl;
				//PlaySound(TEXT("door_edited.wav"), NULL, SND_FILENAME);

		}
		break;
	case B:
		if (fabs(position[0]) <= door_width/2 &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			position[2] <= room_length_min) {
				player->room = A;
				std::cout << "Entering room: " << player->room << std::endl;

		} else if (position[0] >= room_width_max &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			fabs(position[2]) <= door_width/2) {
				player->room = C;
				std::cout << "Entering room: " << player->room << std::endl;
				
		}
		break;
	case C:
		if (!player->has_key &&
			position[0] >= (room_width_max - 30) && position[0] <= (room_width_max - (30 - key_size)) &&
			position[1] <= (room_height_min + key_size) &&
			fabs(position[2]) <= key_size/2) {
				std::cout << "Grabbed Key" << std::endl;
				player->has_key = true;
		} else if (fabs(position[0]) <= door_width/2 &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			position[2] <= room_length_min) {
				player->room = D;
				std::cout << "Entering room: " << player->room << std::endl;
		} else if (position[0] <= room_width_min &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			position[2] <= door_width/2 && position[2] >= -door_width/2) {
				player->room = B;
				std::cout << "Entering room: " << player->room << std::endl;
		}
		break;
	case D:
		if (fabs(position[0]) <= door_width/2 &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			position[2] >= room_length_max) {
				player->room = C;
				std::cout << "Entering room: " << player->room << std::endl;
		} else if (position[0] >= room_width_max &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			fabs(position[2]) <= door_width/2 &&
			player->has_key) {
				std::cout << "Freedom!" << std::endl;
				player->has_key = false;
		} else if (position[0] <= room_width_min &&
			position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
			fabs(position[2]) <= door_width/2) {
				player->room = A;
				std::cout << "Entering room: " << player->room << std::endl;
		}
		break;
	default:
		break;
	}

    // If the user has penetrated the plane, set the device force to 
    // repel the user in the direction of the surface normal of the plane.
    // Penetration occurs if the plane is facing in +Y and the user's Y position
    // is negative, or vice versa.
	double penetrationDistance = 0;
    double k = planeStiffness;
	

	// update x force
	if (position[0] <= room_width_min || position[0] >= room_width_max) {
		directionFlag = (position[0] > 0) ? -1 : 1;
        penetrationDistance = fabs(position[0]) - room_width_max;
        hduVector3Dd forceDirection(directionFlag, 0, 0);

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
	// update y force
	if (position[1] <= room_height_min || position[1] >= room_height_max) {
		directionFlag = (position[1] > 0) ? -1 : 1;
        penetrationDistance = fabs(position[1]) - room_height_max;
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
	if (position[2] <= room_length_min || position[2] >= room_length_max) {
		directionFlag = (position[2] > 0) ? -1 : 1;
        penetrationDistance = fabs(position[2]) - room_length_max;
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

