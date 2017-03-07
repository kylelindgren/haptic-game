#include "Vibration.h"

namespace vibration {

/******************************************************************************
 Runs the main loop of the application.  Queries for keypresses to control
 the frequency of the vibration effect.
******************************************************************************/
void mainLoop()
{
    HDint nFrequency = gVibrationFreq;
    HDdouble amplitude;
    HDdouble maxAmplitude;
    int keypress;

    /* Start the amplitude at a fraction of the max continuous 
       force value.  Remember the max continuous force value to be
       used as the max amplitude. This cap prevents the user
       from increasing the amplitude to dangerous limits. */
    hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &amplitude);
    maxAmplitude = amplitude;
    amplitude *= 0.75;
    gVibrationAmplitude = amplitude;

    //while (HD_TRUE)
    //{
    //    if (_kbhit())
    //    {
    //        keypress = toupper(getch());

    //        if (keypress == 'Q')
    //        {
    //            break;
    //        }
    //        else if (keypress == '+' || keypress == '=')
    //        {
    //            ++nFrequency;
    //            if (nFrequency > 500)
    //                nFrequency = 500;
    //            
    //            hdScheduleSynchronous(SetVibrationFreqCallback, &nFrequency,
    //                                  HD_DEFAULT_SCHEDULER_PRIORITY);
    //        }
    //        else if (keypress == '-' || keypress == '_')
    //        {
    //            --nFrequency;
    //            if (nFrequency < 0)
    //                nFrequency = 0;
    //            
    //            hdScheduleSynchronous(SetVibrationFreqCallback, &nFrequency,
    //                                  HD_DEFAULT_SCHEDULER_PRIORITY);
    //        }
    //        else if (keypress == '[' || keypress == '{')
    //        {
    //            amplitude -= .1;
    //            if (amplitude < 0)
    //                amplitude = 0;
    //            hdScheduleSynchronous(SetVibrationAmplitudeCallback, &amplitude,
    //                                  HD_DEFAULT_SCHEDULER_PRIORITY);
    //        }
    //        else if (keypress == ']' || keypress == '}')
    //        {
    //            amplitude += .1;
    //            if (amplitude > maxAmplitude)
    //                amplitude = maxAmplitude;
    //            hdScheduleSynchronous(SetVibrationAmplitudeCallback, &amplitude,
    //                                  HD_DEFAULT_SCHEDULER_PRIORITY);
    //        }

    //        printf("Vibration Frequency: %d\n", nFrequency);
    //        printf("Vibration Amplitude: %lf\n", amplitude);
    //        printf("\n");
    //    }
    //    
    //    /* Check if the main scheduler callback has exited. */
    //    if (!hdWaitForCompletion(gCallbackHandle, HD_WAIT_CHECK_STATUS))
    //    {
    //        fprintf(stderr, "\nThe main scheduler callback has exited\n");
    //        fprintf(stderr, "\nPress any key to quit.\n");
    //        getch();
    //        return;
    //    }
    //}
}
/******************************************************************************
 Initlaizes the demo by preparing the user to hold the device..
******************************************************************************/
BOOL initDemo(void)
{
    HDErrorInfo error;
    int calibrationStyle;
    printf("Demo Instructions\n");

    hdGetIntegerv(HD_CALIBRATION_STYLE, &calibrationStyle);
    if (calibrationStyle & HD_CALIBRATION_AUTO || calibrationStyle & HD_CALIBRATION_INKWELL)
    {
        printf("Please prepare for starting the demo by \n");
        printf("holding the device handle firmly and\n\n");
        printf("Press any key to continue...\n");
        getch();
        return 1;
    }
    if (calibrationStyle & HD_CALIBRATION_ENCODER_RESET )
    {
        printf("Please prepare for starting the demo by \n");
        printf("holding the device handle firmly and \n\n");
        printf("Press any key to continue...\n");

        getch();

        hdUpdateCalibration(calibrationStyle);
        if (hdCheckCalibration() == HD_CALIBRATION_OK)
        {
            printf("Calibration complete.\n\n");
            return 1;
        }
        if (HD_DEVICE_ERROR(error = hdGetError()))
        {
            hduPrintError(stderr, &error, "Reset encoders reset failed.");
            return 0;           
        }
    }
}

/******************************************************************************
 Simulates a vibration using a sinusoidal wave along the Y axis.
******************************************************************************/
HDCallbackCode HDCALLBACK VibrationCallback(void *data)
{
	User* player = static_cast<User*>(data);

    static const hduVector3Dd direction_lr  = hduVector3Dd( 1, 0, 0 );
    static const hduVector3Dd direction_fb  = hduVector3Dd( 0, 0, 1 );
	static const hduVector3Dd direction_pet = hduVector3Dd( 0, 1, 0 );
    HDErrorInfo error;
    hduVector3Dd force;
    hduVector3Dd force_zero = hduVector3Dd(0, 0, 0);
    HDdouble instRate;
    static HDdouble timer = 0;
    static HDint    freq_pet = 25;  // wikipedia says this about average	
    static HDdouble amp_pet  = 0.75;
	static HDdouble nominalMaxContinuousForce = player->max_force;

	hdGetDoublev(HD_CURRENT_FORCE, force);

	/* Use the reciprocal of the instantaneous rate as a timer. */
	hdGetDoublev(HD_INSTANTANEOUS_UPDATE_RATE, &instRate);
	timer += 1.0 / instRate;
	
	/* Apply a sinusoidal force in the direction of motion. */
	if (player->opening_lr_door) {
		hduVecScale(force, direction_lr, gVibrationAmplitude * sin(timer * gVibrationFreq));
	} else if (player->opening_fb_door) {
		hduVecScale(force, direction_fb, gVibrationAmplitude * sin(timer * gVibrationFreq));
	} else if (player->petting_cat) {
		hduVecScale(force, direction_pet, amp_pet * sin(timer * freq_pet));
	}
	
	// Find the max continuous force that the device is capable of
	// hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &nominalMaxContinuousForce);
	for (int i = 0; i < 3; i++) {
		if (force[i] > nominalMaxContinuousForce) {
			//std::cout << force << std::endl;
			force[i] = nominalMaxContinuousForce;
		} else if (force[i] < -nominalMaxContinuousForce) {
			//std::cout << force << std::endl;
			force[i] = -nominalMaxContinuousForce;
		}
	}

	hdSetDoublev(HD_CURRENT_FORCE, force);


		/* Check if an error occurred while attempting to render the force. */
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, 
						"Error detected during scheduler callback.\n");

		if (hduIsSchedulerError(&error))
		{
			return HD_CALLBACK_DONE;
		}
	}


    return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
 Modifies the vibration frequency being used by the haptic thread.
******************************************************************************/
HDCallbackCode HDCALLBACK SetVibrationFreqCallback(void *data)
{
	//User* player = static_cast<User*>(data);
    HDint *nFrequency = (HDint *) data; //player->vibration_freq;
        
    gVibrationFreq = *nFrequency;        

    return HD_CALLBACK_DONE;
}

/******************************************************************************
 Modifies the vibration amplitude being used by the haptic thread.
******************************************************************************/
HDCallbackCode HDCALLBACK SetVibrationAmplitudeCallback(void *data)
{
	//User* player = static_cast<User*>(data);
    HDdouble *amplitude = (HDdouble *) data;//player->vibration_amp;
        
    gVibrationAmplitude = *amplitude;        

    return HD_CALLBACK_DONE;
}

/*****************************************************************************/

}  // namespace vibration
