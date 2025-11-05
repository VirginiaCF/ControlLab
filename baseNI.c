// Program by Virginia Corral Flores
// Created around 2009 for the Control Laboratory class
// ITESM Tech Institute

// The program is to be used in LAB WINDOWS CVI with its user interface where 
// the user interacts with elements like buttons, sliders, switches that 
// translate in actions through a National Instruments NI DAQ 6008 / 6016
// interface card connected via USB cable.

// Over this code, the programmer adds his/her own needed functions.



/* Example using DAQmx digital lines */
#include <formatio.h>
#include <NIDAQmx.h>
#include <DAQmxIOctrl.h>
#include <cvirte.h>
#include <userint.h>
#include "mine.h"
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

/* Global variables */
static int panelHandle;
int32 error=0;
TaskHandle di_taskHandle = 0, do_taskHandle = 0;
char errBuff[2048]={'\0'};

/* Main function where configuration takes place */
int main (int argc, char *argv[])
{
    if (InitCVIRTE (0, argv, 0) == 0)
        return -1; /* out of memory */
    if ((panelHandle = LoadPanel (0, "myproject.uir", PANEL)) < 0)
    return -1;

	//Create Tasks for every Digital Channel
    DAQmxErrChk (DAQmxCreateTask("",&di_taskHandle));
    DAQmxErrChk (DAQmxCreateTask("",&do_taskHandle));

    //Configure Channels
    DAQmxErrChk (DAQmxCreateDIChan(di_taskHandle,"Dev1/port0/line0:7","",
    DAQmx_Val_ChanForAllLines));
    DAQmxErrChk (DAQmxCreateDOChan(do_taskHandle,"Dev1/port1/line0:3","",
    DAQmx_Val_ChanForAllLines));

    //Start Tasks
    DAQmxErrChk (DAQmxStartTask(di_taskHandle)); // DAQmx Start Code
    DAQmxErrChk (DAQmxStartTask(do_taskHandle)); // DAQmx Start Code

    /* Event Driven code set */
    DisplayPanel (panelHandle);
    RunUserInterface ();
    DiscardPanel (panelHandle);

    // DAQmx Stop Code for Digital Channels
    if ( di_taskHandle!=0 ) {
        DAQmxStopTask(di_taskHandle);
        DAQmxClearTask(di_taskHandle);
    }
    if ( do_taskHandle!=0 ) {
        DAQmxStopTask(do_taskHandle);
        DAQmxClearTask(do_taskHandle);
    }
Error:
    if (DAQmxFailed(error) )
        MessagePopup("DAQmx Error",errBuff);
    return 0;
}

int CVICALLBACK QuitCallback (int panel, int control, int event,
            void *callbackData, int eventData1, int eventData2)
{
    switch (event) {
        case EVENT_COMMIT:
        //Turn OFF outputs before closing application.
        uInt8 DigitalO_data[7];
        int i;
        for (i=0; i<7; i++){
            DigitalO_data[i] = 0;
        }
        DAQmxWriteDigitalLines (do_taskHandle, 1, 1, 10.0,
            DAQmx_Val_GroupByChannel,
            DigitalO_data, &write, 0);
        QuitUserInterface (0);
        break;
    }
    return 0;
}

int CVICALLBACK ftimer (int panel, int control, int event,
            void *callbackData, int eventData1, int eventData2)
{
    uInt8 DigitalI_data[8];
    int32 read,bytesPerSamp;
    switch (event) {
        case EVENT_TIMER_TICK:
        DAQmxErrChk(DAQmxReadDigitalLines(di_taskHandle,1,10.0,
		    DAQmx_Val_GroupByChannel, DigitalI_data,8,&read,&bytesPerSamp,NULL));
        // DAQmx Read Code
}
Error:
    return 0;
}