
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

#include "platform/NativeBridge.h"

CAMetalLayer* getMetalLayer(NSWindow* window) {
    if (!window) {
        return nil;
    }

    NSView* view = window.contentView;

    [view setWantsLayer:YES];

    if ([view.layer isKindOfClass:[CAMetalLayer class]]) {
        return (CAMetalLayer*)view.layer;
    }

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    [view setLayer:metalLayer];

    return metalLayer;
}