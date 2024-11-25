#ifndef OS_HEADER_MAIN
#define OS_HEADER_MAIN
#ifdef __linux__

#if defined(__has_include)
    #if __has_include(<X11/Xlib.h>)
        #include <X11/Xlib.h>
    #endif
#else
    #warning "Couldn't find X11/Xlib.h! UI drawing function will be disabled! (To fix this install X11)"
#endif

#include <unistd.h>

ReturnType createWin(std::vector<std::string> args);

#endif
#endif