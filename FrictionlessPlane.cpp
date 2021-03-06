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
				  room_width_min  = -75, room_width_max  = 75,
				  room_height_min = -30, room_height_max = 30;
	static double door_width = 25, door_height = 45;
	static double key_size = 15, cat_size = 20;
	static double pos_max;
	static HDdouble nominalMaxContinuousForce = player->max_force;

	//time variables
	static double opening_door_duration = 1.0;
	static time_t timer_now, timer_start;
	timer_now = time(NULL);

    // Stiffnes, i.e. k value, of the plane.  Higher stiffness results
    // in a harder surface.
    const double planeStiffness = 0.75;
	const double room_guidance = 1;
	hduVector3Dd f = hduVector3Dd(0, 0, 0);

	hdGetDoublev(HD_CURRENT_FORCE, f);

    // Amount of force the user needs to apply in order to pop through
    // the plane.
    const double popthroughForceThreshold = 10.0;
    
    // Plane direction changes whenever the user applies sufficient
    // force to popthrough it.
    // 1 means the plane is facing +Y.
    // -1 means the plane is facing -Y.
    static int directionFlag = 1;

    // Get the position of the device.
    hduVector3Dd position;
    hdGetDoublev(HD_CURRENT_POSITION, position);

	if (player->room_transition) {
		if (position[0] != 0 && position[1] != 0 && position[2] != 0) {
			// normalize force
			pos_max = 0;
			for (int i = 0; i < 2; i++) {
				if (fabs(position[i]) > pos_max)
					pos_max = fabs(position[i]);
			}
			f -= room_guidance*position;// / pos_max;
		} else {
			player->room_transition = false;
			// todo: play door closing sound
		}

	} else {
		// check if entering a door
		switch (player->room) {
		case A:
			// door center front of the room with the height starting at room height min
			if (fabs(position[0]) <= door_width/2 &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				position[2] >= room_length_max) {
					if (!player->opening_fb_door) {
						player->opening_fb_door = true;
						timer_start = time(NULL);
						PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_fb_door = false;
						player->room_transition = true;
						player->room = B;
						std::cout << "Entering room: " << player->room << std::endl;
						//PlaySound(NULL, NULL, SND_ASYNC);
						//PlaySound(TEXT("C:\\OpenHaptics\\Academic\\3.1\\new_checkout\\audio\\footsteps.wav"), NULL, SND_ASYNC);
					}
			
			} else if (position[0] >= room_width_max &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				fabs(position[2]) <= door_width/2) {
					if (!player->opening_lr_door) {
						player->opening_lr_door = true;
						timer_start = time(NULL);
						PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_lr_door = false;
						player->room_transition = true;
						player->room = D;
						std::cout << "Entering room: " << player->room << std::endl;
					}
			} else {  // no longer at a door or cat, stop vibration
				if (player->opening_fb_door || player->opening_lr_door)
					PlaySound(NULL, NULL, SND_ASYNC);
				player->opening_lr_door = false;
				player->opening_fb_door = false;
				player->petting_cat = false;
			}
			break;
		case B:
			if (fabs(position[0]) <= door_width/2 &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				position[2] <= room_length_min) {
					if (!player->opening_fb_door) {
						player->opening_fb_door = true;
						timer_start = time(NULL);
						PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_fb_door = false;
						player->room_transition = true;
						player->room = A;
						std::cout << "Entering room: " << player->room << std::endl;
					}

			} else if (position[0] >= room_width_max &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				fabs(position[2]) <= door_width/2) {
					if (!player->opening_lr_door) {
						player->opening_lr_door = true;
						timer_start = time(NULL);
						PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_lr_door = false;
						player->room_transition = true;
						player->room = C;
						std::cout << "Entering room: " << player->room << std::endl;
					}
			} else if (position[0] >= (room_width_max - cat_size) &&  // petting cat
					   position[1] <= 100 &&
					   position[2] >= (room_length_max - cat_size)) {
				if (!player->petting_cat) {
					PlaySound(TEXT("C:\\OpenHaptics\\Academic\\3.1\\new_checkout\\audio\\cat_purring.wav"), NULL, SND_ASYNC);
					player->petting_cat = true;
				}
			} else {
				if (player->opening_fb_door || player->opening_lr_door || player->petting_cat)
					PlaySound(NULL, NULL, SND_ASYNC);
				player->opening_lr_door = false;
				player->opening_fb_door = false;
				player->petting_cat = false;
			}
			break;
		case C:
			if (!player->has_key &&
				position[0] >= (room_width_max - 30) && position[0] <= (room_width_max - (30 - key_size)) &&
				position[1] <= (room_height_min + key_size) &&
				fabs(position[2]) <= key_size/2) {
					std::cout << "Grabbed Key" << std::endl;
					player->has_key = true;
					PlaySound(TEXT("C:\\OpenHaptics\\Academic\\3.1\\new_checkout\\audio\\key_edited.wav"), NULL, SND_ASYNC);
			} else if (fabs(position[0]) <= door_width/2 &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				position[2] <= room_length_min) {
					if (!player->opening_fb_door) {
						player->opening_fb_door = true;
						timer_start = time(NULL);
						PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_fb_door = false;
						player->room_transition = true;
						player->room = D;
						std::cout << "Entering room: " << player->room << std::endl;
					}
			} else if (position[0] <= room_width_min &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				position[2] <= door_width/2 && position[2] >= -door_width/2) {
					if (!player->opening_lr_door) {
						player->opening_lr_door = true;
						timer_start = time(NULL);
						PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_lr_door = false;
						player->room_transition = true;
						player->room = B;
						std::cout << "Entering room: " << player->room << std::endl;
					}
			} else {
				if (player->opening_fb_door || player->opening_lr_door)
					PlaySound(NULL, NULL, SND_ASYNC);
				player->opening_lr_door = false;
				player->opening_fb_door = false;
				player->petting_cat = false;
			}
			break;
		case D:
			if (fabs(position[0]) <= door_width/2 &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				position[2] >= room_length_max &&
				!player->free) {
					if (!player->opening_fb_door) {
						player->opening_fb_door = true;
						timer_start = time(NULL);
						PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_fb_door = false;
						player->room_transition = true;
						player->room = C;
						std::cout << "Entering room: " << player->room << std::endl;
					}
			} else if (position[0] >= room_width_max &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				fabs(position[2]) <= door_width/2) {
					if (player->has_key) {
						std::cout << "Freedom!" << std::endl;
						player->has_key = false;
						player->free = true;
						PlaySound(TEXT("C:\\OpenHaptics\\Academic\\3.1\\new_checkout\\audio\\applause.wav"), NULL, SND_ASYNC);
					} else if (!player->trying_locked_door && !player->free) {
						player->trying_locked_door = true;
						PlaySound(TEXT("C:\\OpenHaptics\\Academic\\3.1\\new_checkout\\audio\\door_locked.wav"), NULL, SND_ASYNC);
					}
			} else if (position[0] <= room_width_min &&
				position[1] <= (door_height/2 - (room_height_max - door_height/2)) &&
				fabs(position[2]) <= door_width/2 &&
				!player->free) {
					if (!player->opening_lr_door) {
						player->opening_lr_door = true;
						timer_start = time(NULL);
					PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
					} else if (difftime(timer_now, timer_start) > opening_door_duration) {
						player->opening_lr_door = false;
						player->room_transition = true;
						player->room = A;
						std::cout << "Entering room: " << player->room << std::endl;
					}
			} else {
				if (player->opening_fb_door || player->opening_lr_door || player->trying_locked_door)
					PlaySound(NULL, NULL, SND_ASYNC);
				player->opening_lr_door = false;
				player->opening_fb_door = false;
				player->petting_cat = false;
				player->trying_locked_door = false;
			}
			break;
		default:
			break;
		}
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
            //f.set(0.0,0.0,0.0);
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
            //f.set(0.0,0.0,0.0);
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
            //f.set(0.0,0.0,0.0);
            directionFlag = -directionFlag;
        }
	}
		
	//// Find the max continuous force that the device is capable of
	//// hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &nominalMaxContinuousForce);
	for (int i = 0; i < 3; i++) {
		if (f[i] > nominalMaxContinuousForce) {
			//std::cout << f << std::endl;
			f[i] = nominalMaxContinuousForce;
		} else if (f[i] < -nominalMaxContinuousForce) {
			//std::cout << f << std::endl;
			f[i] = -nominalMaxContinuousForce;
		}
	}
	
	// std::cout << f << std::endl;
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

