#ifdef __linux__

#if defined(__has_include)
    #if __has_include(<X11/Xlib.h>)
        #include <X11/Xlib.h>
    #endif
#else
    #error "Couldn't find X11/Xlib.h! Please install X11!"
#endif

#include <unistd.h>

#endif