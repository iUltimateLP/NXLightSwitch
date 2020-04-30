/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

// Sysmodules should not use applet*
u32 __nx_applet_type = AppletType_None;

// Adjust heap size as needed
#define INNER_HEAP_SIZE 0x80000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char   nx_inner_heap[INNER_HEAP_SIZE];

// Called by libnx to initialize the heap memory we get for this sysmodule
void __libnx_initheap(void)
{
    // Original NX Heap memory
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	// Fake Heap which we could use for our own data
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

// Initializes the sysmodule, used to init services
void __attribute__((weak)) __appInit(void)
{
    // Will hold the result of several service inits
    Result rc;

    // Initialize default Switch services and make sure it was successful
    rc = smInitialize();
    if (R_FAILED(rc))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
    }

    // Enable this if you want to use HID
    /*rc = hidInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));*/

    //Enable this if you want to use time
    /*rc = timeInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_Time));

    __libnx_init_time();*/

    // Initialize the filesystem service and make sure it was successful
    rc = fsInitialize();
    if (R_FAILED(rc))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));
    }

    // Mount the SDMC storage
    fsdevMountSdmc();
}

// Called (and not needed because this is a sysmodule) when the user tries to exit this app
void __attribute__((weak)) userAppExit(void);

// Called when the Switch requests this sysmodule to exit
void __attribute__((weak)) __appExit(void)
{
    // Cleanup and exit the services we opened
    fsdevUnmountAll();
    fsExit();
    //timeExit();
    //hidExit();
    smExit();
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Initialization code can go here.

    // Your code / main loop goes here.
    // If you need threads, you can use threadCreate etc.

    // Deinitialization and resources clean up code can go here.
    return 0;
}
