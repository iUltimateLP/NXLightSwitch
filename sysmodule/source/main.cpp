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

// Sysmodules should not use applet*
u32 __nx_applet_type = AppletType_None;

// Adjust heap size as needed
#define INNER_HEAP_SIZE 0x1e000 // 0x1e * 0x1000 // 0x80000
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

    std::ofstream ofs = std::ofstream();
    ofs.open("sdmc:/NXLightSwitch.txt");
    ofs << "Starting\n";

    static Thread updateThread;
    constexpr std::size_t updateThreadStackSize = 2 * 0x1000; // ams::os::MemoryPageSize is 0x1000
    static std::uint8_t updateThreadStack[updateThreadStackSize];
    static auto updateThreadFunc = +[](void* args) {
        int counter = 0;
        while (counter < 10)
        {
            svcSleepThread(1e+10); // 10 seconds

            std::ofstream* ofs = static_cast<std::ofstream*>(args);
            *ofs << "10 seconds passed. Counter: ";
            *ofs << counter;
            *ofs << ".\n";

            counter++;
        }
    };

    // LibnxError_BadInput
    Result r = threadCreate(&updateThread, updateThreadFunc, static_cast<void*>(&ofs), updateThreadStack, updateThreadStackSize, 0x3f, -2);
    if (!R_SUCCEEDED(r))
    {
        ofs << "Error creating thread: ";
        ofs << std::hex << "0x" << R_VALUE(r) << ": " << std::dec << R_DESCRIPTION(r) << std::endl;
    }
    else
    {
        ofs << "Created thread\n";
    }

    // KernelError_InvalidHandle
    r = threadStart(&updateThread);
    if (!R_SUCCEEDED(r))
    {
        ofs << "Error starting thread: ";
        ofs << std::hex << "0x" << R_VALUE(r) << ": " << std::dec << R_DESCRIPTION(r) << std::endl;
    }
    else
    {
        ofs << "Started thread\n";
    }

    // KernelError_InvalidHandle
    r = threadWaitForExit(&updateThread);
    if (!R_SUCCEEDED(r))
    {
        ofs << "Error waiting for thread: ";
        ofs << std::hex << "0x" << R_VALUE(r) << ": " << std::dec << R_DESCRIPTION(r) << std::endl;
    }
    else
    {
        ofs << "Waiting for thread\n";
    }

    // LibnxError_BadInput / KernelError_InvalidMemoryRange
    r = threadClose(&updateThread);
    if (!R_SUCCEEDED(r))
    {
        ofs << "Error closing thread: ";
        ofs << std::hex << "0x" << R_VALUE(r) << ": " << std::dec << R_DESCRIPTION(r) << std::endl;
    }
    else
    {
        ofs << "Closed thread\n";
    }

    ofs << "Ending.\n";
    ofs.close();

    // Deinitialization and resources clean up code can go here.
    return 0;
}
