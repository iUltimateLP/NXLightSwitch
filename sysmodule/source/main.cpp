/*
    NXLightSwitch for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

// Include the main libnx system header, for Switch development
#include <switch.h>

// Include the NXLightSwitch headers
#include "logger.hpp"

using namespace nxlightswitch;

// Taken from Stratosphere
#define MEMORY_PAGE_SIZE 0x1000
#define THREAD_STACK_ALIGNMENT 4 * 1024 // 4kb

extern "C" 
{
    // Sysmodules should not use applet*
    u32 __nx_applet_type = AppletType_None;

    // Adjust heap size as needed
    #define INNER_HEAP_SIZE (0x1e * MEMORY_PAGE_SIZE)
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];

    // Not really needed probably
    u32 __nx_nv_transfermem_size = 0x15 * MEMORY_PAGE_SIZE;

    alignas(16) u8 __nx_exception_stack[MEMORY_PAGE_SIZE];
    u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
}

// Called by libnx to initialize the heap memory we get for this sysmodule
extern "C" void __libnx_initheap(void)
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
extern "C" void __attribute__((weak)) __appInit(void)
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
extern "C" void __attribute__((weak)) userAppExit(void);

// Called when the Switch requests this sysmodule to exit
extern "C" void __attribute__((weak)) __appExit(void)
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

    Logger::get()->log("Starting NXLightSwitch");

    static Thread updateThread;
    constexpr std::size_t updateThreadStackSize = 2 * MEMORY_PAGE_SIZE;
    alignas(THREAD_STACK_ALIGNMENT) static std::uint8_t updateThreadStack[updateThreadStackSize]; // ams::os::ThreadStackAlignment
    static auto updateThreadFunc = +[](void* args) {
        int counter = 0;
        while (counter < 10)
        {
            svcSleepThread(5e+9); // 5 seconds

            Logger::get()->log("5 seconds passed in thread. Counter: %d", counter);

            counter++;
        }
    };

    Result r = threadCreate(&updateThread, updateThreadFunc, NULL, updateThreadStack, updateThreadStackSize, 0x3f, -2);
    if (!R_SUCCEEDED(r))
    {
        Logger::get()->logError(r);
    }
    else
    {
        Logger::get()->log("Created thread");
    }

    r = threadStart(&updateThread);
    if (!R_SUCCEEDED(r))
    {
        Logger::get()->logError(r);
    }
    else
    {
        Logger::get()->log("Started thread");
    }

    r = threadWaitForExit(&updateThread);
    if (!R_SUCCEEDED(r))
    {
        Logger::get()->logError(r);
    }
    else
    {
        Logger::get()->log("Waiting for thread");
    }

    r = threadClose(&updateThread);
    if (!R_SUCCEEDED(r))
    {
        Logger::get()->logError(r);
    }
    else
    {
        Logger::get()->log("Closed thread");
    }

    // Deinitialization and resources clean up code can go here.
    return 0;
}
