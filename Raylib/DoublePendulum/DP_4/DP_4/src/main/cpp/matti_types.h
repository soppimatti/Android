//
// Created by matti on 4/6/26.
//

#ifndef DP_1_MATTI_TYPES_H
#define DP_1_MATTI_TYPES_H

#include "raymob.h"
#include "raymath.h"
#include <stdlib.h>

typedef struct Bob
{
    Vector3 pos;
    float angDeg;
    float angRad;
    float angVel;
    float angAcc;
//    float angRad22;
} Bob_t;

typedef void (*matti_dp_init_fcn) (void);
typedef void (*matti_dp_update_fcn) (uint64_t);
typedef void (*matti_dp_render_fcn) (void);
typedef void (*matti_dp_deinit_fcn) (void);

typedef struct DP_Syst
{
    Vector3 anchorPos;
    Vector3 rod1Pos;
    Vector3 rod2Pos;
    Bob_t bob1;
    Bob_t bob2;
    Color color;
} DP_Syst_t;

//-----
typedef struct DP_ObjFcns
{
    matti_dp_init_fcn dp_init;
    matti_dp_deinit_fcn dp_deinit;
    matti_dp_update_fcn dp_update;
    matti_dp_render_fcn dp_render;
} DP_ObjFcns_t;

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)

#define RAD_90 1.5707963705
#define RAD_180 3.1415927410
#define RAD_270 4.7123889923

#endif //DP_1_MATTI_TYPES_H
