#include "raymob.h" // This header can replace 'raylib.h' and includes additional functions related to Android.
#include <android/log.h>
#include "matti_types.h"
#include "matti_one_3d_dp.h"
#include "matti_some_3d_dp.h"
#include "matti_many_3d_dp.h"
#include "matti_many_2d_dp.h"
#include "matti_derive.h"

#define TAG "DP_3.main"

#define NO_OF_OBJ_STATES 4

typedef struct DP_This
{
    DP_ObjFcns_t objFcns [NO_OF_OBJ_STATES];
    DP_ObjFcns_t *currObjFcns;
    int objFcnsIdx;
    uint64_t tickCounter;
    int isDragging;
    Vector2 touchPosDown;
    Vector2 touchPosUp;
} DP_This_t;

static DP_This_t gThis;

static int dp_handleTouch (void);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main (void)
{
    LOGI ("main");

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow (0, 0, "Double pendulum 3");
    SetTargetFPS (60);
    //--------------------------------------------------------------------------------------

    der_init (1.7f, 1.0f, 13.0f, 13.0f);

    one_3d_dp_register_callbacks (&(gThis.objFcns [0]));
    some_3d_dp_register_callbacks (&(gThis.objFcns [1]));
    many_3d_dp_register_callbacks (&(gThis.objFcns [2]));
    many_2d_dp_register_callbacks (&(gThis.objFcns [3]));

    gThis.tickCounter = 0;
    gThis.objFcnsIdx = 0;
    gThis.isDragging = 0;

    gThis.currObjFcns = &(gThis.objFcns [gThis.objFcnsIdx]);
    gThis.currObjFcns->dp_init ();

    // Main game loop
    while (!WindowShouldClose ())
    {
        gThis.tickCounter++;

        int isSwipe = dp_handleTouch ();
        if (isSwipe == -1)
        {
            LOGI ("Swipe L");

            gThis.currObjFcns->dp_deinit ();

            gThis.objFcnsIdx = (gThis.objFcnsIdx + 1) % NO_OF_OBJ_STATES;
            gThis.currObjFcns = &(gThis.objFcns [gThis.objFcnsIdx]);
            gThis.currObjFcns->dp_init ();
        }
/*
        if ((gThis.tickCounter % 1500) == 0)
        {
            gThis.currObjFcns->dp_deinit ();

            gThis.objFcnsIdx = (gThis.objFcnsIdx + 1) % NO_OF_OBJ_STATES;
            gThis.currObjFcns = &(gThis.objFcns [gThis.objFcnsIdx]);
            gThis.currObjFcns->dp_init ();
        }
*/
        //----------------------------------------------------------------------------------
        gThis.currObjFcns->dp_update (gThis.tickCounter);

        //----------------------------------------------------------------------------------
        gThis.currObjFcns->dp_render ();
    }

    //--------------------------------------------------------------------------------------
    CloseWindow ();
    //--------------------------------------------------------------------------------------

    return 0;
}

//-----
static int dp_handleTouch (void)
{
    int tCount = GetTouchPointCount ();
    if (tCount > 0)
    {
        if (gThis.isDragging == 0)
        {
            gThis.touchPosDown = GetTouchPosition (0);
            gThis.isDragging = 1;

            LOGI ("dp_handleTouch DN (%.2f,%.2f)", gThis.touchPosDown.x, gThis.touchPosDown.y);
        }
    }
    else
    {
        if (gThis.isDragging == 1)
        {
            gThis.touchPosUp = GetTouchPosition (0);
            gThis.isDragging = 0;

            float touchDiff = gThis.touchPosDown.x - gThis.touchPosUp.x;
            LOGI ("dp_handleTouch UP (%.2f,%.2f), %.2f", gThis.touchPosUp.x, gThis.touchPosUp.y, touchDiff);

            if (fabs (touchDiff) > 100)
            {
                if (touchDiff < 0)
                {
                    return 1;
                }
                else
                {
                    return -1;
                }
            }
        }
    }

    return 0;
}
