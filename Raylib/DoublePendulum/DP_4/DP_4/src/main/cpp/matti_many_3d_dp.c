//
// Created by matti on 4/13/26.
//

#include "raymob.h"
#include "matti_many_3d_dp.h"
#include <android/log.h>
#include "matti_types.h"
#include "matti_derive.h"

#define TAG "DP_3.many_3d_dp"

typedef struct One_DP_This
{
    Camera camera;
    float fieldOfView;
    float len;
    float deltaTime;
    float timeStamp;
    DP_Syst_t systems [4000];
    int noOfSystems;
    Model model;
    float camX;
    float camZ;
    float camRot;
    float camDist;
    Vector3 scale1;
    uint64_t tickCounter;
    int screenWidth;
    int screenHeight;
} One_DP_This_t;

static One_DP_This_t gThis;

static void update (Bob_t *b1, Bob_t *b2, DP_Syst_t *syst);
static void many_3d_dp_init (void);
static void many_3d_dp_deinit (void);
static void many_3d_dp_update (uint64_t tickCounter);
static void many_3d_dp_render (void);

//-----
void many_3d_dp_register_callbacks (DP_ObjFcns_t *objFcns)
{
    objFcns->dp_init = many_3d_dp_init;
    objFcns->dp_deinit = many_3d_dp_deinit;
    objFcns->dp_update = many_3d_dp_update;
    objFcns->dp_render = many_3d_dp_render;
}

//-----
static void many_3d_dp_init (void)
{
    LOGI ("many_3d_dp_init");
    gThis.len = 3.0f;
    gThis.deltaTime = 0.01f;
    gThis.timeStamp = 0.0f;
    gThis.noOfSystems = sizeof (gThis.systems)/sizeof (gThis.systems [0]);
    gThis.tickCounter = 0;
    gThis.screenWidth = GetScreenWidth ();
    gThis.screenHeight = GetScreenHeight ();

    float hsvH = 0.0f;
    float ii = 0.0f;
    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        ii = (float) i;
        hsvH = (ii/((float) gThis.noOfSystems))*360.0f;
        DP_Syst_t *syst = &(gThis.systems [i]);
        syst->anchorPos = (Vector3) {0.0f, 0.0f, -20.0f + ((float) i)/35.0f};
        syst->rod1Pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};
        syst->rod2Pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};
        syst->color = ColorFromHSV (hsvH, 1.0f, 1.0f);

        Bob_t *bob1 = &(syst->bob1);
        bob1->angDeg = 80.0f;
        bob1->angRad = bob1->angDeg*DEG2RAD;
        bob1->angVel = 0;
        bob1->pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};

        Bob_t *bob2 = &(syst->bob2);
        bob2->angDeg = 80.0f + ((float) i)/10.0f;
        bob2->angDeg = 61.0f + (ii/((float) gThis.noOfSystems))*1.001f;
        bob2->angRad = bob2->angDeg*DEG2RAD;
        bob2->angVel = 0;
        bob2->pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};
    }

    gThis.model = LoadModelFromMesh (GenMeshCube (gThis.len, 0.1f, 0.1f));

    if (gThis.screenWidth == 2772)
    {
        gThis.fieldOfView = 90.0f;
    }
    else
    {
        gThis.fieldOfView = 60.0f;
    }

    gThis.camera.position = (Vector3) {0.0f, 0.0f, 20.0f};
    gThis.camera.target = (Vector3) {3.0f, -10.0f, 0.0f };
    gThis.camera.up = (Vector3) {-1.0f, 0.0f, 0.0f};
    gThis.camera.fovy = gThis.fieldOfView;
    gThis.camera.projection = CAMERA_PERSPECTIVE;

    gThis.scale1 = (Vector3) {1.0f, 1.0f, 1.0f};

    gThis.camX = 0.0f;
    gThis.camZ = 0.0f;
    gThis.camRot = -0.6f;
    gThis.camDist = 15.0f;

    gThis.camX = gThis.camDist*cosf (gThis.camRot);
    gThis.camZ = gThis.camDist*sinf (gThis.camRot);
    gThis.camera.position = (Vector3) {gThis.camera.position.x, gThis.camX, gThis.camZ};
//    gThis.camera.position = (Vector3) {gThis.camera.position.x, -15, 2};

    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        DP_Syst_t *syst = &(gThis.systems [i]);
        Bob_t *b1 = &(syst->bob1);
        Bob_t *b2 = &(syst->bob2);

        for (int j = 0; j < 4000; j++)
        {
            update (b1, b2, syst);
        }
    }
}

//-----
static void many_3d_dp_deinit (void)
{
    LOGI ("many_3d_dp_deinit");

    UnloadModel (gThis.model);
}

//-----
static void many_3d_dp_update (uint64_t tc)
{
//    LOGI ("many_3d_dp_update tc %lu", gThis.tickCounter);

    gThis.camX = gThis.camDist*cosf (gThis.camRot);
    gThis.camZ = gThis.camDist*sinf (gThis.camRot);
    if (gThis.camRot < 4.9)
    {
        gThis.camRot = gThis.camRot + 0.0005;
    }
    else
    {
        gThis.camRot = gThis.camRot + 0.0001;
    }

//    gThis.camera.position = (Vector3) {gThis.camera.position.x, gThis.camX, gThis.camZ};

    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        DP_Syst_t *syst = &(gThis.systems [i]);
        Bob_t *b1 = &(syst->bob1);
        Bob_t *b2 = &(syst->bob2);

        update (b1, b2, syst);
        if (gThis.tickCounter < 680)
        {
            update (b1, b2, syst);
            update (b1, b2, syst);
        }
    }

    gThis.tickCounter++;
}

//-----
static void many_3d_dp_render (void)
{
//    LOGI ("many_3d_dp_render");

    ClearBackground (GetColor (0x181818FF));

    BeginDrawing ();
    {
        BeginMode3D (gThis.camera);
        {
            for (int i = 0; i < gThis.noOfSystems; i++)
            {
                DP_Syst_t *syst = &(gThis.systems [i]);
                Bob_t *b1 = &(syst->bob1);
                Bob_t *b2 = &(syst->bob2);

                DrawModelEx (gThis.model, syst->rod1Pos, (Vector3) {0.0f, 0.0f, 1.0f}, b1->angDeg, gThis.scale1, syst->color);
                DrawModelEx (gThis.model, syst->rod2Pos, (Vector3) {0.0f, 0.0f, 1.0f}, b2->angDeg, gThis.scale1, syst->color);
            }
        }
        EndMode3D ();
    }

    EndDrawing ();
}

//-----
static void update (Bob_t *b1, Bob_t *b2, DP_Syst_t *syst)
{
    der_euler (b1, b2, gThis.deltaTime);

    b1->angDeg = b1->angRad*RAD2DEG;
    b2->angDeg = b2->angRad*RAD2DEG;

    b1->pos.x = gThis.len*cosf (b1->angRad);
    b1->pos.y = gThis.len*sinf (b1->angRad);

    b2->pos.x = gThis.len*cosf (b2->angRad) + b1->pos.x;
    b2->pos.y = gThis.len*sinf (b2->angRad) + b1->pos.y;

    float len2 = gThis.len/2.0f;
    syst->rod1Pos.x = len2*cosf (b1->angRad);
    syst->rod1Pos.y = len2*sinf (b1->angRad);
    syst->rod2Pos.x = len2*cosf (b2->angRad) + b1->pos.x;
    syst->rod2Pos.y = len2*sinf (b2->angRad) + b1->pos.y;
}
