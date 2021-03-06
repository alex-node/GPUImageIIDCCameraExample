#import <Foundation/Foundation.h>
#import <GPUImage/GPUImage.h>
#import <dc1394/dc1394.h>

/*
 This class will need to:
 - Connect to the IIDC camera
 - Deal with the default setting
 - Capture a frame to be processed by GPUImage
 - Do the color conversion
 - Use a ring buffer to get the frames
 - Try to avoid using notifications. Try to use callbacks or delegates.
*/

// Why are some of these instance variables and why are some properties? -JKC

// Have to expose some things:
// Frame Rate/FPS

// These are functions
void uyvy411_2vuy422(const unsigned char *the411Frame, unsigned char *the422Frame, const unsigned int width, const unsigned int height);
void yuv422_2vuy422(const unsigned char *theYUVFrame, unsigned char *the422Frame, const unsigned int width, const unsigned int height);

extern NSString *const GPUImageCameraErrorDomain;
//static void cameraFrameReadyCallback(dc1394camera_t *camera, void * data);

// Does this class subclass NSObject because it needs to be separate from GPUImage for licensing reasons?? -JKC
@interface GPUImageIIDCCamera : GPUImageOutput
{
    NSInteger numberOfCameraToUse;
    NSInteger sequentialMissedCameraFrames;
    BOOL frameGrabTimedOutOnce;
    id cameraDisconnectionObserver;
    
    // Camera settings
    NSInteger previousGain, previousExposure;
    float currentLuminance;
    NSUInteger frameIntervalCounter;
    BOOL isBlackflyCamera; // Special-case tweaks for that one camera type
    
    // libdc1394 variables for the firewire control
    uint32_t numCameras;
    char *device_name;
    dc1394_t * d;
    dc1394featureset_t features;
    
    // Dispatch Queue
    dispatch_queue_t cameraDispatchQueue;
    NSRunLoop *cameraFrameCallbackRunLoop;
    NSThread *cameraFrameCallbackThread;
    BOOL cameraShouldPoll;

    // Benchmarking
    NSUInteger numberOfFramesCaptured;
    CGFloat totalFrameTimeDuringCapture;
}

@property(readwrite) BOOL isCaptureInProgress;
@property(readwrite, nonatomic) BOOL isConnectedToCamera;
@property(readwrite, nonatomic) BOOL runBenchmark;

@property(readwrite, nonatomic) CGFloat luminanceSetPoint;
@property(readonly, nonatomic) CGSize frameSize;

// libdc1394 properties
// Need to figure out if these should be readwrite, readonly, nonatomic, etc... -JKC
@property(readwrite) dc1394framerate_t framerate;
@property(readwrite) dc1394video_mode_t videoMode;
@property(readonly) dc1394video_modes_t supportedVideoModes;
@property(readwrite, nonatomic) dc1394color_coding_t colorspace;
@property(readwrite) dc1394speed_t isoSpeed;
@property(readwrite) dc1394camera_t *camera;
@property(readwrite) dc1394operation_mode_t operationMode;


// Settings
@property(readwrite, nonatomic) NSInteger brightnessMin, brightnessMax, exposureMin, exposureMax, sharpnessMin, sharpnessMax, whiteBalanceMin, whiteBalanceMax, saturationMin, saturationMax, gammaMin, gammaMax, shutterMin, shutterMax, gainMin, gainMax;
@property(readwrite, nonatomic) NSInteger brightness, exposure, whiteBalanceU, whiteBalanceV, sharpness, saturation, gamma, shutter, gain;
@property(readwrite, nonatomic) CGRect regionOfInterest;

- (void)setWhiteBalance:(uint32_t)newWhiteBalanceU whiteBalanceV:(uint32_t)newWhiteBalanceV;

// Camera interface
- (BOOL)connectToCamera:(NSError **)error;
- (BOOL)readAllSettingLimits:(NSError **)error;
- (void)startCameraCapture;
- (void)stopCameraCapture;
- (BOOL)supportsVideoMode:(dc1394video_mode_t)mode;

// External device control
- (void)turnOnLEDLight;
- (void)turnOffLEDLight;

// Error handling methods
- (NSError *)errorForCameraDisconnection;

// Debugging methods
- (NSString*)stringForMode:(uint32_t)mode;

@end


