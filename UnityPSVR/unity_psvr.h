//
//  unity_psvr.h
//  UnityPSVR
//
//  Created by Kentaro Teramoto on 2016/10/18.
//  Copyright © 2016 Kentaro Teramoto. All rights reserved.
//

#ifndef unity_psvr_h
#define unity_psvr_h

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "hidapi.h"

#define PSVR_VID 1356
#define PSVR_PID 2479

#endif /* unity_psvr_h */


unsigned char buf[70];
int yaw, yawAccel;

struct PSVRState {
    short yawAccel;
    short pitchAccel;
    short rollAccel;
    
    short yaw;
    short pitch;
    short roll;
    
    short xAccel;
    short yAccel;
    short zAccel;
    
    unsigned short status;
};

struct PSVRContext {
    struct  PSVRState* currentState;
    hid_device* device;
    pthread_t loopThread;
    unsigned short stopLoop;
    void* threadStatus;
    pthread_mutex_t mutex;
    unsigned short isTracking;
};

struct  hid_device_info*    EnumerateDevices();
void                        FreeDeviceInfos(struct hid_device_info* devs);

unsigned short              IsDeviceFound();

struct  PSVRContext*        InitPSVR();
void                        StartTracking(struct PSVRContext* ctx);
struct  PSVRState*          GetState(struct PSVRContext* ctx);
void                        StopTracking(struct PSVRContext* ctx);
void                        ResetYaw(struct PSVRContext* ctx);
