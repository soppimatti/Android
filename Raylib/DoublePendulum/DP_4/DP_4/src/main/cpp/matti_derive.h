//
// Created by matti on 4/6/26.
//

#ifndef DP_1_MATTI_DERIVE_H
#define DP_1_MATTI_DERIVE_H

#include "matti_types.h"

void der_init (float m1, float m2, float L1, float L2);
int der_rk (Bob_t *bob1, Bob_t *bob2, float dt);
int der_euler (Bob_t *bob1, Bob_t *bob2, float dt);

#endif //DP_1_MATTI_DERIVE_H
