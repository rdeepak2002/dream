#ifdef __APPLE__

#import <Cocoa/Cocoa.h>

extern "C" void changeTitleBarColor(NSWindow* window, double red, double green, double blue)
{
    window.titlebarAppearsTransparent = YES;
    window.backgroundColor = [NSColor colorWithRed:red green:green blue:blue alpha:1.0];
}

extern "C" void setWindowStyleMask(NSWindow* window)
{
    if ((window.styleMask & NSFullScreenWindowMask) == NSFullScreenWindowMask) {
        // case where we are full screen
    } else {
//        window.styleMask = NSResizableWindowMask
//                           | NSTitledWindowMask
//                           | NSFullSizeContentViewWindowMask
//                           | NSClosableWindowMask
//                           | NSMiniaturizableWindowMask;
    }
}

void foo(__attribute((ns_consumed)) id x) {
    NSLog(@"Something To Print");
}

extern "C" void addMenu(NSWindow* window)
{
//    NSMenu* menubar = [NSMenu new];
//    NSMenuItem* appMenuItem = [NSMenuItem new];
//    [menubar addItem:appMenuItem];
//    NSApplication.sharedApplication.mainMenu = menubar;
//    NSMenu* appMenu = [NSMenu new];
//
//    // save menu item
//    NSString* saveTitle = @"Save";
//    NSMenuItem* saveMenuItem = [[NSMenuItem alloc] initWithTitle:saveTitle
//                                                          action:@selector(foo:)
//                                                   keyEquivalent:@"s"];
//    [appMenu addItem:saveMenuItem];
//
//    // full screen menu item
//    NSString* toggleFullScreenTitle = @"Toggle Full Screen";
//    NSMenuItem* toggleFullScreenMenuItem = [[NSMenuItem alloc] initWithTitle:toggleFullScreenTitle
//                                                                      action:@selector(toggleFullScreen:)
//                                                               keyEquivalent:@"f"];
//    [appMenu addItem:toggleFullScreenMenuItem];
//
//    // quit menu item
//    NSString* quitTitle = [@"Quit " stringByAppendingString:@"Dream"];
//    NSMenuItem* quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
//                                                          action:@selector(terminate:)
//                                                   keyEquivalent:@"q"];
//    [appMenu addItem:quitMenuItem];
//
//    [appMenuItem setSubmenu:appMenu];
}

#endif