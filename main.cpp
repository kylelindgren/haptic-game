// Needed for Phantom Omni
#include "FrictionlessPlane.h"
#include "QueryDevice.h"
#include "Vibration.h"
//#include "UserData.h"

// Needed for Windows 32 API for sounds
#include <windows.h>
#include <iostream>
#include <mmsystem.h>
//#include "stdafx.h"

// Define sound files (wav only)
#define SCARY_SOUND (".\\audio\\background_scary.wav")
#define DOOR_SOUND (".\\..\\door_open_close.wav")
#define FOOTSTEPS_SOUND (".\\..\\footsteps.wav")


/*******************************************************************************
 * main function
   Initializes the device, creates a callback to handles plane forces, 
   terminates upon key press.
 ******************************************************************************/
int main(int argc, char* argv[])
{
	User* player0 = new User;
	void* data = static_cast<void*>(player0);
    HDErrorInfo error;

    // Initialize the default haptic device.
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    // Start the servo scheduler and enable forces.
    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start the scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

	// QueryDevice setup
    HDSchedulerHandle hUpdateHandle = 0;
    /* Schedule the main scheduler callback that updates the device state. */
    hUpdateHandle = hdScheduleAsynchronous(
        querydevice::updateDeviceCallback, 0, HD_MAX_SCHEDULER_PRIORITY);

    // Schedule the frictionless plane callback, which will then run at 
    // servoloop rates and command forces if the user penetrates the plane.
    HDCallbackCode hPlaneCallback = hdScheduleAsynchronous(
        frictionlessplane::FrictionlessPlaneCallback, data, HD_DEFAULT_SCHEDULER_PRIORITY);

	// Vibration setup
    //HDSchedulerHandle gCallbackHandle = 0;
	vibration::gCallbackHandle = hdScheduleAsynchronous(
        vibration::VibrationCallback, data, HD_MAX_SCHEDULER_PRIORITY);

    printf("Room example.\n");
    printf("Move the device around to feel the walls.\n");
    printf("Push hard against the plane to popthrough to the other side.\n");
    printf("Press the stylus button to output current position.\n");
    printf("Press any key to quit.\n\n");

	// Sample sound code
	// To make sound work, add an include for "winmm.lib" in the linker include section for the project file
	///*
	//PlaySound(TEXT("door_edited.wav"), NULL, SND_ASYNC);
	//*/

    while (!_kbhit())
    {       
		// Query the device
		querydevice::mainLoop();
		// Vibrate if needed
		//vibration::mainLoop();
		// Run wall checking
        if (!hdWaitForCompletion(hPlaneCallback, HD_WAIT_CHECK_STATUS))
        {
            fprintf(stderr, "\nThe main scheduler callback has exited\n");
            fprintf(stderr, "\nPress any key to quit.\n");
            getch();
            break;
        }
    }

    // Cleanup and shutdown the haptic device, cleanup all callbacks.
    hdStopScheduler();
    hdUnschedule(hPlaneCallback);
    hdDisableDevice(hHD);

    return 0;
}