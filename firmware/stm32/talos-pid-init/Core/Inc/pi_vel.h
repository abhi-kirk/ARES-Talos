#ifndef PI_VEL_H
#define PI_VEL_H

#include <stdint.h>

typedef struct {
	float kp;
	float ki;
	float integ;
	float integ_limit;
	float u_limit;      // limit on |u| in "PWM microseconds offset"
	float deadband_us;  // minimum offset to oversome stiction (https://en.wikipedia.org/wiki/Stiction)
	uint8_t still_ctr;
} PI_Vel;

static inline void PI_Vel_Init(
		PI_Vel *c,
		float kp,
		float ki,
		float integ_limit,
		float u_limit,
		float deadband_us
) {
	c->kp = kp;
	c->ki = ki;
	c->integ = 0.0f;
	c->integ_limit = integ_limit;
	c->u_limit = u_limit;
	c->deadband_us = deadband_us;
	c->still_ctr = 0;
}

static inline void PI_Vel_Reset(PI_Vel *c) {
	c->integ = 0.0f;
}

// Returns u in microseconds offset from neutral (e.g., +80 => 1580us)
static inline float PI_Vel_Step(PI_Vel *c, float v_ref, float v_meas, float dt) {
	// error
	float e = v_ref - v_meas;

	// integrate
	c->integ += e * dt;

	if (c->integ >  c->integ_limit) c->integ =  c->integ_limit;
	if (c->integ < -c->integ_limit) c->integ = -c->integ_limit;

	// PI
	float u = c->kp * e + c->ki * c->integ;

	// clamp
	if (u >  c->u_limit) u =  c->u_limit;
	if (u < -c->u_limit) u = -c->u_limit;

	// deadband compensation (use v_ref sign, not u sign)
	const float VREF_EPS = 1e-3f;
	const float V_STILL  = 10.0f;     // ticks/s
	const uint8_t STILL_N = 5;        // ~50 ms if loop is 10 ms

	if (v_ref > VREF_EPS) {
	    if (v_meas < V_STILL) {
	        if (c->still_ctr < 255) c->still_ctr++;
	    } else {
	        c->still_ctr = 0;
	    }

	    if (c->still_ctr >= STILL_N && u < c->deadband_us) u = c->deadband_us;
	}
	else if (v_ref < -VREF_EPS) {
	    if (v_meas > -V_STILL) {
	        if (c->still_ctr < 255) c->still_ctr++;
	    } else {
	        c->still_ctr = 0;
	    }

	    if (c->still_ctr >= STILL_N && u > -c->deadband_us) u = -c->deadband_us;
	}
	else {
	    u = 0.0f;
	    c->integ = 0.0f;
	    c->still_ctr = 0;
	}

	return u;
}

#endif  // PI_VEL_H
