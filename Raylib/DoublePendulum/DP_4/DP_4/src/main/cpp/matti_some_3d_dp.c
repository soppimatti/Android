//
// Created by matti on 4/13/26.
//

#include "raymob.h"
#include "matti_one_3d_dp.h"
#include <android/log.h>
#include "matti_types.h"
#include "matti_derive.h"

#define TAG "DP_3.some_3d_dp"

typedef struct Some_DP_This
{
    Camera camera;
    float fieldOfView;
    float len;
    float deltaTime;
    DP_Syst_t systems [10];
    int noOfSystems;
    Model model;
    float camX;
    float camZ;
    float camRot;
    float camDist;
    Vector3 scale1;
    uint64_t tickCounter;
} Some_DP_This_t;

static Some_DP_This_t gThis;

static void update (Bob_t *b1, Bob_t *b2, DP_Syst_t *syst);
static void some_3d_dp_init (void);
static void some_3d_dp_deinit (void);
static void some_3d_dp_update (uint64_t tickCounter);
static void some_3d_dp_render (void);

//-----
void some_3d_dp_register_callbacks (DP_ObjFcns_t *objFcns)
{
    objFcns->dp_init = some_3d_dp_init;
    objFcns->dp_deinit = some_3d_dp_deinit;
    objFcns->dp_update = some_3d_dp_update;
    objFcns->dp_render = some_3d_dp_render;
}

//-----
static void some_3d_dp_init (void)
{
    LOGI ("some_3d_dp_init");
    gThis.len = 3.0f;
    gThis.deltaTime = 0.01f;
    gThis.noOfSystems = sizeof (gThis.systems)/sizeof (gThis.systems [0]);
    gThis.tickCounter = 0;

    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        DP_Syst_t *syst = &(gThis.systems [i]);
        syst->anchorPos = (Vector3) {0.0f, 0.0f, -5.0f + i};
        syst->rod1Pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};
        syst->rod2Pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};

        Bob_t *bob1 = &(syst->bob1);
        bob1->angDeg = 80.0f;
        bob1->angRad = bob1->angDeg*DEG2RAD;
        bob1->angVel = 0;
        bob1->pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};

        Bob_t *bob2 = &(syst->bob2);
        bob2->angDeg = 60.0f + ((float) i)/2.0f;
        bob2->angRad = bob2->angDeg*DEG2RAD;
        bob2->angVel = 0;
        bob2->pos = (Vector3) {0.0f, 0.0f, syst->anchorPos.z};
    }
    gThis.model = LoadModelFromMesh (GenMeshCube (gThis.len, 0.1f, 0.1f));

    gThis.fieldOfView = 40.0f;

    gThis.camera.position = (Vector3) {0.0f, 0.0f, 10.0f};
    gThis.camera.target = (Vector3) {3.0f, 0.0f, 0.0f };
    gThis.camera.up = (Vector3) {-1.0f, 0.0f, 0.0f};
    gThis.camera.fovy = gThis.fieldOfView;
    gThis.camera.projection = CAMERA_PERSPECTIVE;

    gThis.scale1 = (Vector3) {1.0f, 1.0f, 1.0f};

    gThis.camX = 0.0f;
    gThis.camZ = 0.0f;
    gThis.camRot = RAD_270;
    gThis.camDist = 15.0f;

    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        DP_Syst_t *syst = &(gThis.systems [i]);
        Bob_t *b1 = &(syst->bob1);
        Bob_t *b2 = &(syst->bob2);

        for (int j = 0; j < 2000; j++)
        {
            update (b1, b2, syst);
        }
    }
}

//-----
static void some_3d_dp_deinit (void)
{
    LOGI ("some_3d_dp_deinit");

    UnloadModel (gThis.model);
}

//-----
static void some_3d_dp_update (uint64_t tc)
{
//    LOGI ("some_3d_dp_update tc %lu", gThis.tickCounter);

    gThis.camX = gThis.camDist*cosf (gThis.camRot);
    gThis.camZ = gThis.camDist*sinf (gThis.camRot);
    gThis.camRot = gThis.camRot + 0.001;
//    gThis.camRot = 1.89f;
//    gThis.camera.position = (Vector3) {gThis.camX, gThis.camera.position.y, gThis.camZ};
    gThis.camera.position = (Vector3) {gThis.camera.position.x, gThis.camX, gThis.camZ};

    for (int i = 0; i < gThis.noOfSystems; i++)
    {
        DP_Syst_t *syst = &(gThis.systems [i]);
        Bob_t *b1 = &(syst->bob1);
        Bob_t *b2 = &(syst->bob2);

        update (b1, b2, syst);
        if (gThis.tickCounter < 1100)
        {
            update (b1, b2, syst);
            update (b1, b2, syst);
        }
    }

    gThis.tickCounter++;
}

//-----
static void some_3d_dp_render (void)
{
//    LOGI ("some_3d_dp_render");

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

                DrawModelEx (gThis.model, syst->rod1Pos, (Vector3) {0.0f, 0.0f, 1.0f}, b1->angDeg, gThis.scale1, GREEN);
                DrawModelEx (gThis.model, syst->rod2Pos, (Vector3) {0.0f, 0.0f, 1.0f}, b2->angDeg, gThis.scale1, YELLOW);

                DrawSphere (syst->anchorPos, 0.2f, GetColor (0x34A6F4FF));
                DrawSphereWires (syst->anchorPos, 0.2f, 16, 16, GetColor (0x155DFCFF));

                DrawSphere (b1->pos, 0.2f, GetColor (0xFF2056FF));
                DrawSphereWires (b1->pos, 0.2f, 16, 16, GetColor (0xAC253FFF));

                DrawSphere (b2->pos, 0.2f, GetColor (0x2AA63EFF));
                DrawSphereWires (b2->pos, 0.2f, 16, 16, GetColor (0x178236FF));
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
