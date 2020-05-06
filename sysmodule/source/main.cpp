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
#include <time.h>

// Include the NXLightSwitch headers
#include "logger.hpp"
#include "utils.hpp"
#include "worker.hpp"

using namespace nxlightswitch;

// Taken from libstratosphere for memory management
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

// Forward declaration for __libnx_init_time
extern "C" void __libnx_init_time(void);

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

    // Initialize the time module
    rc = timeInitialize();
    if (R_FAILED(rc))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_Time));
    }
    __libnx_init_time();

    // Initialize the set module
    rc = setInitialize();
    if (R_FAILED(rc))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_NotInitialized));
    }

    // Initialize the setsys module
    rc = setsysInitialize();
    if (R_FAILED(rc))
    {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_NotInitialized));
    }

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
    setsysExit();
    setExit();
    timeExit();
    smExit();
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    Logger::get()->clearLogFile();
    Logger::get()->log("Starting NXLightSwitch");

    // Create a new instance of our Worker which will handle the logic for this module
    Worker* worker = new Worker();

    // To check the time and update the system's theme, we need to run in a thread so we won't block the system
    // The threads are libnx' system
    static Thread workerThread;

    // Size of the memory stack for the thread. Partially taken off libstratosphere, it's important to align
    // the stack by 4KB (THREAD_STACK_ALIGNMENT) otherwise we will get a "Bad Input" error when creating the
    // thread.
    constexpr std::size_t workerThreadStackSize = 2 * MEMORY_PAGE_SIZE;
    alignas(THREAD_STACK_ALIGNMENT) static std::uint8_t workerThreadStack[workerThreadStackSize];

    // This is the function which runs in our thread
    static auto workerThreadFunc = +[](void* args) {
        // Get back the worker instance from the args we pass to this thread
        Worker* worker = static_cast<Worker*>(args);

        // This loop shouldn't end as we're always updating the worker
        while (true)
        {
            // Block the thread until we should perform our next check
            svcSleepThread(WORKER_UPDATE_INTERVAL);

            // Call the worker to perform the logic
            worker->DoWork();
        }
    };

    // Create the thread using the thread handle, stack and function above
    // We cast our worker instance to a void* so we can pass it to the thread
    // 0x3f is a special thread priority for background tasks
    // -2 indicates to run the thread on the default CPU core
    Result r = threadCreate(&workerThread, workerThreadFunc, static_cast<void*>(worker), workerThreadStack, workerThreadStackSize, 0x3f, -2);
    LOG_IF_ERROR(r);

    // Start the thread after we created it
    r = threadStart(&workerThread);
    LOG_IF_ERROR(r);

    // Now, this will completely block the execution of this sysmodule until the thread exits
    r = threadWaitForExit(&workerThread);
    LOG_IF_ERROR(r);

    // Close the thread because this part of the code is only reached when the thread exited already
    r = threadClose(&workerThread);
    LOG_IF_ERROR(r);

    return 0;
}
