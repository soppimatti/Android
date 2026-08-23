//
// Created by matti on 4/13/26.
//

#include "raymob.h"
#include "matti_many_2d_dp.h"
#include <android/log.h>
#include "matti_types.h"
#include "matti_derive.h"

#define TAG "DP_3.many_2d_dp"

typedef struct Many_DP_This
{
    float fieldOfView;
    float len;
    float deltaTime;
    float timeStamp;
    DP_Syst_t systems [10000];
    int noOfSystems;
    uint64_t tickCounter;
    int screenWidth;
    int screenHeight;
} Many_DP_This_t;

static Many_DP_This_t gThis;

static void update (DP_Syst_t *syst);
static void many_2d_dp_init (void);
static void many_2d_dp_deinit (void);
static void many_2d_dp_update (uint64_t tickCounter);
static void many_2d_dp_render (void);

//-----
void many_2d_dp_register_callbacks (DP_ObjFcns_t *objFcns)
{
    objFcns->dp_init = many_2d_dp_init;
    objFcns->dp_deinit = many_2d_dp_deinit;
    objFcns->dp_update = many_2d_dp_update;
    objFcns->dp_render = many_2d_dp_render;
}

//-----
static void many_2d_dp_init (void)
{
    LOGI ("many_2d_dp_init");

    gThis.len = 500.0f;
    gThis.timeStamp = 0.0f;
    gThis.deltaTime = 0.01f;
    gThis.noOfSystems = sizeof (gThis.systems)/sizeof (gThis.systems [0]);
    gThis.screenWidth = GetScreenWidth ();
    gThis.screenHeight = GetScreenHeight ();

    if (gThis.screenWidth == 2772)
    {
        gThis.len = 500.0f;
    }
    else
    {
        gThis.len = 700.0f;
    }
    float hsvH = 0.0f;
    float ii = 0.0f;
    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        ii = (float) i;
        hsvH = (ii/((float) gThis.noOfSystems))*360.0f;
        DP_Syst_t *syst = &(gThis.systems [i]);
        syst->anchorPos = (Vector3) {gThis.screenWidth/2, gThis.screenHeight/7, 0.0f};
        syst->rod1Pos = (Vector3) {0.0f, 0.0f, 0.0f};
        syst->rod2Pos = (Vector3) {0.0f, 0.0f, 0.0f};
        syst->color = ColorFromHSV (hsvH, 1.0f, 1.0f);

        Bob_t *bob1 = &(syst->bob1);
        bob1->angDeg = 80.0f;
        bob1->angRad = bob1->angDeg*DEG2RAD;
        bob1->angVel = 0;
        bob1->pos = (Vector3) {0.0f, 0.0f, 0.0f};

        Bob_t *bob2 = &(syst->bob2);
        bob2->angDeg = 80.0f + ((float) i)/10.0f;
        bob2->angDeg = 61.0f + (ii/((float) gThis.noOfSystems))*1.0001f;
        bob2->angRad = bob2->angDeg*DEG2RAD;
        bob2->angVel = 0;
        bob2->pos = (Vector3) {0.0f, 0.0f, 0.0f};
    }

    if (gThis.screenWidth == 2772)
    {
        gThis.fieldOfView = 90.0f;
    }
    else
    {
        gThis.fieldOfView = 60.0f;
    }

    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        DP_Syst_t *syst = &(gThis.systems [i]);
        Bob_t *b1 = &(syst->bob1);
        Bob_t *b2 = &(syst->bob2);

        for (int j = 0; j < 3000; j++)
        {
            update (syst);
        }
    }
}

//-----
static void many_2d_dp_deinit (void)
{
    LOGI ("many_2d_dp_deinit");
}

//-----
static void many_2d_dp_update (uint64_t tc)
{
//    LOGI ("many_2d_dp_update tc %lu", gThis.tickCounter);

    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        DP_Syst_t *syst = &(gThis.systems [i]);
        Bob_t *b1 = &(syst->bob1);
        Bob_t *b2 = &(syst->bob2);

        update (syst);
        if (gThis.tickCounter < 250)
        {
            update (syst);
            update (syst);
            update (syst);
            update (syst);
            update (syst);
            update (syst);
            update (syst);
            update (syst);
            update (syst);
            update (syst);
        }
    }

    gThis.tickCounter++;
}

//-----
static void many_2d_dp_render (void)
{
//    LOGI ("many_2d_dp_render");

    ClearBackground (GetColor (0x181818FF));

    Bob_t *b1;
    Bob_t *b2;
    BeginDrawing ();
    {
        for (int i = 0; i < gThis.noOfSystems; i++)
        {
            DP_Syst_t *syst = &(gThis.systems [i]);
            b1 = &(syst->bob1);
            b2 = &(syst->bob2);

            DrawLineEx ((Vector2) {syst->anchorPos.x, syst->anchorPos.y}, (Vector2) {b1->pos.x, b1->pos.y}, 4.0f, syst->color);
            DrawLineEx ((Vector2) {b1->pos.x, b1->pos.y}, (Vector2) {b2->pos.x, b2->pos.y}, 4.0f, syst->color);
        }
    }

    EndDrawing ();
}

//-----
static void update (DP_Syst_t *syst)
{
    Bob_t *b1 = &(syst->bob1);
    Bob_t *b2 = &(syst->bob2);

    der_euler (b1, b2, gThis.deltaTime);

    b1->angDeg = b1->angRad*RAD2DEG;
    b2->angDeg = b2->angRad*RAD2DEG;

    float adjAr1 = b1->angRad + RAD_90;
    float adjAr2 = b2->angRad + RAD_90;

    b1->pos.x = gThis.len*cosf (adjAr1) + syst->anchorPos.x;
    b1->pos.y = gThis.len*sinf (adjAr1) + syst->anchorPos.y;

    b2->pos.x = gThis.len*cosf (adjAr2) + b1->pos.x;
    b2->pos.y = gThis.len*sinf (adjAr2) + b1->pos.y;
}
