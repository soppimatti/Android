//
// Created by matti on 4/6/26.
//

#include "matti_derive.h"

#include <stdio.h>
#include <math.h>

#define G 9.81

typedef struct
{
    double theta1;
    double theta2;
    double omega1;
    double omega2;
} State;

static float gM1;
static float gM2;
static float gL1;
static float gL2;

static State der_derivatives (State s, double m1, double m2, double L1, double L2);
static State der_add (State s, State k, double scale);
static void der_rk4_step (State *s, double dt, double m1, double m2, double L1, double L2);
static void der_euler_step (State *s, double dt, double m1, double m2, double l1, double l2);

// Compute derivatives
static State der_derivatives (State s, double m1, double m2, double L1, double L2)
{
    State d;

    double delta = s.theta2 - s.theta1;

    double denom1 = (m1 + m2)*L1 - m2*L1*cos(delta)*cos(delta);
    double denom2 = (L2/L1) * denom1;

    d.theta1 = s.omega1;
    d.theta2 = s.omega2;

    d.omega1 = (m2*L1*s.omega1*s.omega1*sin(delta)*cos(delta)
                + m2*G*sin(s.theta2)*cos(delta)
                + m2*L2*s.omega2*s.omega2*sin(delta)
                - (m1 + m2)*G*sin(s.theta1)) / denom1;

    d.omega2 = (-m2*L2*s.omega2*s.omega2*sin(delta)*cos(delta)
                + (m1 + m2)*(G*sin(s.theta1)*cos(delta)
                             - L1*s.omega1*s.omega1*sin(delta)
                             - G*sin(s.theta2))) / denom2;

    return d;
}

// Add two states: s + k * scale
static State der_add (State s, State k, double scale)
{
    State r;
    r.theta1 = s.theta1 + k.theta1*scale;
    r.theta2 = s.theta2 + k.theta2*scale;
    r.omega1 = s.omega1 + k.omega1*scale;
    r.omega2 = s.omega2 + k.omega2*scale;

    return r;
}

// RK4 step
static void der_rk4_step (State *s, double dt, double m1, double m2, double L1, double L2)
{
    State k1 = der_derivatives (*s, m1, m2, L1, L2);
    State k2 = der_derivatives (der_add (*s, k1, dt/2.0), m1, m2, L1, L2);
    State k3 = der_derivatives (der_add (*s, k2, dt/2.0), m1, m2, L1, L2);
    State k4 = der_derivatives (der_add (*s, k3, dt), m1, m2, L1, L2);

    s->theta1 += dt/6.0 * (k1.theta1 + 2*k2.theta1 + 2*k3.theta1 + k4.theta1);
    s->theta2 += dt/6.0 * (k1.theta2 + 2*k2.theta2 + 2*k3.theta2 + k4.theta2);
    s->omega1 += dt/6.0 * (k1.omega1 + 2*k2.omega1 + 2*k3.omega1 + k4.omega1);
    s->omega2 += dt/6.0 * (k1.omega2 + 2*k2.omega2 + 2*k3.omega2 + k4.omega2);
}

//-----
int der_rk (Bob_t *bob1, Bob_t *bob2, float dt)
{
    State s = {bob1->angRad, bob2->angRad, bob1->angVel, bob2->angVel};

    der_rk4_step (&s, dt, gM1, gM2, gL1, gL2);

    bob1->angRad = s.theta1;
    bob2->angRad = s.theta2;

    bob1->angVel = s.omega1;
    bob2->angVel = s.omega2;

    return 1;
}

//-----
int der_euler (Bob_t *bob1, Bob_t *bob2, float dt)
{
    State s = {bob1->angRad, bob2->angRad, bob1->angVel, bob2->angVel};

    der_euler_step (&s, dt, gM1, gM2, gL1, gL2);

    bob1->angRad = s.theta1;
    bob2->angRad = s.theta2;

    bob1->angVel = s.omega1;
    bob2->angVel = s.omega2;

    return 1;
}

//-----
void der_init (float m1, float m2, float L1, float L2)
{
    gM1 = m1;
    gM2 = m2;
    gL1 = L1;
    gL2 = L2;
}

//-----
static void der_euler_step (State *s, double dt, double m1, double m2, double l1, double l2)
{
    double theta1 = s->theta1;
    double theta2 = s->theta2;
    double omega1 = s->omega1;
    double omega2 = s->omega2;

    double delta = theta2 - theta1;

    double denom1 = (m1 + m2) * l1 - m2 * l1 * cos(delta) * cos(delta);
    double denom2 = (l2 / l1) * denom1;

    double a1 = (
                        m2 * l1 * omega1 * omega1 * sin(delta) * cos(delta) +
                        m2 * G * sin(theta2) * cos(delta) +
                        m2 * l2 * omega2 * omega2 * sin(delta) -
                        (m1 + m2) * G * sin(theta1)
                ) / denom1;

    double a2 = (
                        -m2 * l2 * omega2 * omega2 * sin(delta) * cos(delta) +
                        (m1 + m2) * G * sin(theta1) * cos(delta) -
                        (m1 + m2) * l1 * omega1 * omega1 * sin(delta) -
                        (m1 + m2) * G * sin(theta2)
                ) / denom2;

    // Euler integration
    s->omega1 += dt * a1;
    s->omega2 += dt * a2;
    s->theta1 += dt * s->omega1;
    s->theta2 += dt * s->omega2;
}
