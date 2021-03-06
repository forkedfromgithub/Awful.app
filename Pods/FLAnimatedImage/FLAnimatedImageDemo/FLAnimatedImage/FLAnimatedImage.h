//
//  FLAnimatedImage.h
//  Flipboard
//
//  Created by Raphael Schaad on 7/8/13.
//  Copyright (c) 2013-2014 Flipboard. All rights reserved.
//


#import <UIKit/UIKit.h>

// Allow user classes conveniently just importing one header.
#import "FLAnimatedImageView.h"

@protocol FLAnimatedImageDebugDelegate;


// Logging
// If set to 0, disables integration with CocoaLumberjack Logger (only matters if CocoaLumberjack is installed).
#define FLLumberjackIntegrationEnabled 1
// If set to 1, enables NSLog logging (only matters #if DEBUG -- never for release builds).
#define FLDebugLoggingEnabled 0


//
//  An `FLAnimatedImage`'s job is to deliver frames in a highly performant way and works in conjunction with `FLAnimatedImageView`.
//  It subclasses `NSObject` and not `UIImage` because it's only an "image" in the sense that a sea lion is a lion.
//  It tries to intelligently choose the frame cache size depending on the image and memory situation with the goal to lower CPU usage for smaller ones, lower memory usage for larger ones and always deliver frames for high performant play-back.
//  Note: `posterImage`, `size`, `loopCount`, `delayTimes` and `frameCount` don't change after successful initialization.
//
@interface FLAnimatedImage : NSObject

@property (nonatomic, strong, readonly) UIImage *posterImage; // Guaranteed to be loaded; usually equivalent to `-imageLazilyCachedAtIndex:0`
@property (nonatomic, assign, readonly) CGSize size; // The `.posterImage`'s `.size`

@property (nonatomic, assign, readonly) NSUInteger loopCount; // 0 means repeating the animation indefinitely
@property (nonatomic, strong, readonly) NSArray *delayTimes; // Of type `NSTimeInterval` boxed in `NSNumber`s
@property (nonatomic, assign, readonly) NSUInteger frameCount; // Number of valid frames; equal to `[.delayTimes count]`

@property (nonatomic, assign, readonly) NSUInteger frameCacheSizeCurrent; // Current size of intelligently chosen buffer window; can range in the interval [1..frameCount]
@property (nonatomic, assign) NSUInteger frameCacheSizeMax; // Allow to cap the cache size; 0 means no specific limit (default)

// Intended to be called from main thread synchronously; will return immediately.
// If the result isn't cached, will return `nil`; the caller should then pause playback, not increment frame counter and keep polling.
// After an initial loading time, depending on `frameCacheSize`, frames should be available immediately from the cache.
- (UIImage *)imageLazilyCachedAtIndex:(NSUInteger)index;

// Pass either a `UIImage` or an `FLAnimatedImage` and get back its size
+ (CGSize)sizeForImage:(id)image;

// Designated initializer
// On success, returns a new `FLAnimatedImage` with all fields populated, on failure returns `nil` and an error will be logged.
- (instancetype)initWithAnimatedGIFData:(NSData *)data;

@property (nonatomic, strong, readonly) NSData *data; // The data the receiver was initialized with; read-only

#if DEBUG
// Only intended to report internal state for debugging
@property (nonatomic, weak) id<FLAnimatedImageDebugDelegate> debug_delegate;
@property (nonatomic, strong) NSMutableDictionary *debug_info; // To track arbitrary data (e.g. original URL, loading durations, cache hits, etc.)
#endif

@end


@interface FLWeakProxy : NSProxy

+ (instancetype)weakProxyForObject:(id)targetObject;

@end


#if DEBUG
@protocol FLAnimatedImageDebugDelegate <NSObject>

@optional

- (void)debug_animatedImage:(FLAnimatedImage *)animatedImage didUpdateCachedFrames:(NSIndexSet *)indexesOfFramesInCache;
- (void)debug_animatedImage:(FLAnimatedImage *)animatedImage didRequestCachedFrame:(NSUInteger)index;
- (CGFloat)debug_animatedImagePredrawingSlowdownFactor:(FLAnimatedImage *)animatedImage;

@end
#endif


#if COCOAPODS_VERSION_MAJOR_CocoaLumberjack == 2
#endif
// Try to detect and import CocoaLumberjack in all scenarious (library versions, way of including it, CocoaPods versions, etc.).
#if FLLumberjackIntegrationEnabled
    #if defined(__has_include)
        #if __has_include("<CocoaLumberjack/CocoaLumberjack.h>")
            #import <CocoaLumberjack/CocoaLumberjack.h>
        #elif __has_include("CocoaLumberjack.h")
            #import "CocoaLumberjack.h"
        #elif __has_include("<CocoaLumberjack/DDLog.h>")
            #import <CocoaLumberjack/DDLog.h>
        #elif __has_include("DDLog.h")
            #import "DDLog.h"
        #endif
    #elif defined(COCOAPODS_POD_AVAILABLE_CocoaLumberjack) || defined(__POD_CocoaLumberjack)
        #if COCOAPODS_VERSION_MAJOR_CocoaLumberjack == 2
            #import <CocoaLumberjack/CocoaLumberjack.h>
        #else
            #import <CocoaLumberjack/DDLog.h>
        #endif
    #endif

    #if defined(DDLogError) && defined(DDLogWarn) && defined(DDLogInfo) && defined(DDLogDebug) && defined(DDLogVerbose)
        #define FLLumberjackAvailable
    #endif
#endif

#if FLLumberjackIntegrationEnabled && defined(FLLumberjackAvailable)
    // Global log level for the whole library, not per-file.
    extern const int ddLogLevel;
    #define FLLogError(...)   DDLogError(__VA_ARGS__)
    #define FLLogWarn(...)    DDLogWarn(__VA_ARGS__)
    #define FLLogInfo(...)    DDLogInfo(__VA_ARGS__)
    #define FLLogDebug(...)   DDLogDebug(__VA_ARGS__)
    #define FLLogVerbose(...) DDLogVerbose(__VA_ARGS__)
#else
    #if FLDebugLoggingEnabled && DEBUG
        // CocoaLumberjack is disabled or not available, but we want to fallback to regular logging (debug builds only).
        #define FLLog(...) NSLog(__VA_ARGS__)
    #else
        // No logging at all.
        #define FLLog(...) ((void)0)
    #endif
    #define FLLogError(...)   FLLog(__VA_ARGS__)
    #define FLLogWarn(...)    FLLog(__VA_ARGS__)
    #define FLLogInfo(...)    FLLog(__VA_ARGS__)
    #define FLLogDebug(...)   FLLog(__VA_ARGS__)
    #define FLLogVerbose(...) FLLog(__VA_ARGS__)
#endif
