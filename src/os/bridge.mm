#include "bridge.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

static CAMetalLayer* g_layer   = nil;
static NSWindow*     g_window  = nil;
static bool          g_quit    = false;

@interface AppDelegate : NSObject <NSApplicationDelegate>
@end
@implementation AppDelegate
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)app {
    return YES;
}
- (void)applicationWillTerminate:(NSNotification*)note {
    g_quit = true;
}
@end

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end
@implementation WindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    g_quit = true;
    return YES;
}
@end

void* create_chart_window(int w, int h, const char* title)
{
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    [NSApp setDelegate:appDelegate];
    [NSApp finishLaunching];

    NSRect frame = NSMakeRect(0, 0, w, h);
    g_window = [[NSWindow alloc]
        initWithContentRect:frame
        styleMask:NSWindowStyleMaskTitled |
                  NSWindowStyleMaskClosable |
                  NSWindowStyleMaskResizable
        backing:NSBackingStoreBuffered
        defer:NO];

    WindowDelegate* winDelegate = [[WindowDelegate alloc] init];
    [g_window setDelegate:winDelegate];

    g_layer = [CAMetalLayer layer];
    g_layer.device         = MTLCreateSystemDefaultDevice();
    g_layer.pixelFormat    = MTLPixelFormatBGRA8Unorm;
    g_layer.framebufferOnly = YES;

    NSView* view = [[NSView alloc] initWithFrame:frame];
    view.layer       = g_layer;
    view.wantsLayer  = YES;
    g_window.contentView = view;

    [g_window setTitle:[NSString stringWithUTF8String:title]];
    [g_window makeKeyAndOrderFront:nil];
    [g_window center];
    [NSApp activateIgnoringOtherApps:YES];

    return (__bridge void*)g_layer;
}

void* get_next_drawable()
{
    return (__bridge void*)[g_layer nextDrawable];
}

void* get_rendering_device()
{
    return (__bridge void*)g_layer.device;
}

void poll_application_events()
{
    NSEvent* e;
    while ((e = [NSApp nextEventMatchingMask:NSEventMaskAny
                                   untilDate:nil
                                      inMode:NSDefaultRunLoopMode
                                     dequeue:YES]))
        [NSApp sendEvent:e];
}

bool application_should_quit()
{
    return g_quit;
}
