#include "FrictionlessPlane.h"
#include "QueryDevice.h"



/*******************************************************************************
 * main function
   Initializes the device, creates a callback to handles plane forces, 
   terminates upon key press.
 ******************************************************************************/
int main(int argc, char* argv[])
{
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
        frictionlessplane::FrictionlessPlaneCallback, 0, HD_DEFAULT_SCHEDULER_PRIORITY);

    printf("Room example.\n");
    printf("Move the device around to feel the walls.\n");
    printf("Push hard against the plane to popthrough to the other side.\n");
    printf("Press the stylus button to output current position.\n");
    printf("Press any key to quit.\n\n");

    while (!_kbhit())
    {       
		// Query the device
		querydevice::mainLoop();
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