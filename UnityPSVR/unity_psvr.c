//
//  unity_psvr.c
//  UnityPSVR
//
//  Created by Kentaro Teramoto on 2016/10/18.
//  Copyright © 2016 Kentaro Teramoto. All rights reserved.
//

#include "unity_psvr.h"


struct hid_device_info* EnumerateDevices() {
    

    
    struct hid_device_info *devs;
    devs = hid_enumerate(0x0, 0x0);
    return devs;
}

void FreeDeviceInfos(struct hid_device_info* devs){
    hid_free_enumeration(devs);
}

unsigned short IsDeviceFound() {
    struct hid_device_info *devs, *cur_dev;
    unsigned short result = 0;
    
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    
    while (cur_dev) {
        if (cur_dev->vendor_id == PSVR_VID && cur_dev->product_id == PSVR_PID){
            result = 1;
        }
        cur_dev = cur_dev->next;
    }
    
    hid_free_enumeration(devs);
    
    return result;
}

struct PSVRContext* InitPSVR(){
    struct PSVRContext* ctx = (struct PSVRContext*)malloc(sizeof(struct PSVRContext));
    ctx->device = hid_open(PSVR_VID, PSVR_PID, NULL);
    ctx->currentState = (struct PSVRState*)malloc(sizeof(struct PSVRState));
    ctx->isTracking = 0;
    pthread_mutex_init(&ctx->mutex, NULL);
    
    
    return ctx;
}

void *loop (void *args) {
    struct PSVRContext* ctx = *(struct PSVRContext* *) args;
    int result;
    struct PSVRState* state = ctx->currentState;
    while(1){
    
        switch(ctx->stopLoop) {
            case 0:
                result = hid_read(ctx->device, buf, 70);

                if (ctx->isTracking == 1) {
                    // connection established
                    
                    yawAccel = (short)buf[21];
                    yaw += yawAccel;
                    
                    pthread_mutex_lock(&ctx->mutex);
                    
                    state->yawAccel = yawAccel;
                    state->pitchAccel = (short)buf[23];
                    state->rollAccel = (short)buf[25];

                    state->pitch = (short)buf[27];
                    state->yaw = yaw;
                    state->roll = (short)buf[29];
                    
                    state->xAccel = (short)buf[39];
                    state->yAccel = (short)buf[41];
                    state->zAccel = (short)buf[43];
                    
                    state->status = (unsigned short)buf[8];
                    
                    pthread_mutex_unlock(&ctx->mutex);
                
                } else if(buf[11] == 255) {
                    // receive data
                    ctx->isTracking = 1;
                } else {
                    // cannot receive data, so reconnect.
                    ctx->stopLoop = 2;
                }
                break;
                
            case 1:
                pthread_exit(ctx->threadStatus);
                break;
                
            case 2:
                state->status = 119;
                hid_set_nonblocking(ctx->device, 0);
                sleep(1);
                hid_close(ctx->device);
                sleep(1);
                ctx->device = hid_open(PSVR_VID, PSVR_PID, NULL);
                ctx->stopLoop = 0;
                sleep(1);
                hid_set_nonblocking(ctx->device, 1);

                break;
            default:
                break;
        }
     
    }
}

void StartTracking(struct PSVRContext* ctx) {
    yaw = 0;
    ctx->stopLoop = 0;
    pthread_create(&ctx->loopThread, NULL, loop, (void *)&ctx);
    hid_set_nonblocking(ctx->device, 1);
}

struct PSVRState* GetState(struct PSVRContext* ctx){
    if (ctx->isTracking == 1) {
        return ctx->currentState;
    } else {
        return NULL;
    }
}


void StopTracking(struct PSVRContext* ctx){
    hid_set_nonblocking(ctx->device, 0);
    ctx->stopLoop = 1;
    hid_close(ctx->device);
}

void ResetYaw(struct PSVRContext* ctx){
    yaw = 0;
}
