#ifdef __APPLE__

#import <Cocoa/Cocoa.h>

extern "C" void changeTitleBarColor(NSWindow* window, double red, double green, double blue)
{
    window.titlebarAppearsTransparent = YES;
    window.backgroundColor = [NSColor colorWithRed:red green:green blue:blue alpha:1.0];
}

extern "C" void setWindowStyleMask(NSWindow* window)
{
    window.styleMask = NSResizableWindowMask
                       | NSTitledWindowMask
                       | NSFullSizeContentViewWindowMask
                       | NSClosableWindowMask
                       | NSMiniaturizableWindowMask;
}

#endif